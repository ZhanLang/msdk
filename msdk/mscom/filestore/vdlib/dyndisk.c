#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>

#include "fops.h"
#include "dyndisk.h"

int dyndisk_open(struct dyndisk_state *s, struct rsdisk_header *hdr)
{
	int shift;

	if (hdr->d_hdr.size <= 1 || hdr->d_hdr.cluster_bits < 9)
		goto fail;

	s->crypt_method = hdr->crypt_method;
	s->crypt_method_header = s->crypt_method;
	s->cluster_bits = hdr->d_hdr.cluster_bits;
	s->cluster_size = 1 << s->cluster_bits;
	s->cluster_sectors = 1 << (s->cluster_bits - 9);
	s->l2_bits = hdr->d_hdr.l2_bits;
	s->l2_size = 1 << s->l2_bits;
	s->total_sectors = hdr->d_hdr.size / VDISK_SECTOR_SIZE;
	s->cluster_offset_mask = (1LL << (63 - s->cluster_bits)) - 1;

	/* vdisk_file_read the level 1 table */
	shift = s->cluster_bits + s->l2_bits;
	s->l1_size = (hdr->d_hdr.size + (1LL << shift) - 1) >> shift;

	s->l1_table_offset = hdr->d_hdr.l1_table_offset;
	s->l1_table = malloc(s->l1_size * sizeof(uint64_t));
	if (!s->l1_table)
		goto fail;
	vdisk_file_lseek(s->fd, s->l1_table_offset, SEEK_SET);
	if (vdisk_file_read(s->fd, s->l1_table, s->l1_size * sizeof(uint64_t))
	    != s->l1_size * sizeof(uint64_t))
		goto fail;
	/* alloc L2 cache */
	s->l2_cache = malloc(s->l2_size * L2_CACHE_SIZE * sizeof(uint64_t));
	if (!s->l2_cache)
		goto fail;
	s->cluster_cache = malloc(s->cluster_size);
	if (!s->cluster_cache)
		goto fail;
	s->cluster_data = malloc(s->cluster_size);
	if (!s->cluster_data)
		goto fail;
	s->cluster_cache_offset = -1;

	return 0;

 fail:
	free(s->l1_table);
	free(s->l2_cache);
	free(s->cluster_cache);
	free(s->cluster_data);
	return -1;
}

uint64_t get_cluster_offset(struct dyndisk_state * s, uint64_t offset,
			    int allocate, int n_start, int n_end)
{
	int min_index, i, j, l1_index, l2_index;
	uint64_t l2_offset, *l2_table, cluster_offset;
	uint32_t min_count;
	int new_l2_table;

	l1_index = offset >> (s->l2_bits + s->cluster_bits);
	l2_offset = s->l1_table[l1_index];
	new_l2_table = 0;
	if (!l2_offset) {
		if (!allocate)
			return 0;
		/* allocate a new l2 entry */
		l2_offset = vdisk_file_lseek(s->fd, 0, SEEK_END);
		/* round to cluster size */
		l2_offset =
		    (l2_offset + s->cluster_size - 1) & ~(s->cluster_size - 1);
		/* update the L1 entry */
		s->l1_table[l1_index] = l2_offset;
		vdisk_file_lseek(s->fd,
				 s->l1_table_offset +
				 l1_index * sizeof(l2_offset), SEEK_SET);
		if (vdisk_file_write(s->fd, &l2_offset, sizeof(l2_offset)) !=
		    sizeof(l2_offset))
			return 0;
		new_l2_table = 1;
	}
	for (i = 0; i < L2_CACHE_SIZE; i++) {
		if (l2_offset == s->l2_cache_offsets[i]) {
			/* increment the hit count */
			if (++s->l2_cache_counts[i] == 0xffffffff) {
				for (j = 0; j < L2_CACHE_SIZE; j++) {
					s->l2_cache_counts[j] >>= 1;
				}
			}
			l2_table = s->l2_cache + (i << s->l2_bits);
			goto found;
		}
	}
	/* not found: load a new entry in the least used one */
	min_index = 0;
	min_count = 0xffffffff;
	for (i = 0; i < L2_CACHE_SIZE; i++) {
		if (s->l2_cache_counts[i] < min_count) {
			min_count = s->l2_cache_counts[i];
			min_index = i;
		}
	}
	l2_table = s->l2_cache + (min_index << s->l2_bits);
	vdisk_file_lseek(s->fd, l2_offset, SEEK_SET);
	if (new_l2_table) {
		memset(l2_table, 0, s->l2_size * sizeof(uint64_t));
		if (vdisk_file_write
		    (s->fd, l2_table,
		     s->l2_size * sizeof(uint64_t)) !=
		    s->l2_size * sizeof(uint64_t))
			return 0;
	} else {
		if (vdisk_file_read
		    (s->fd, l2_table,
		     s->l2_size * sizeof(uint64_t)) !=
		    s->l2_size * sizeof(uint64_t))
			return 0;
	}
	s->l2_cache_offsets[min_index] = l2_offset;
	s->l2_cache_counts[min_index] = 1;
 found:
	l2_index = (offset >> s->cluster_bits) & (s->l2_size - 1);
	cluster_offset = l2_table[l2_index];
	if (!cluster_offset) {
		if (!allocate)
			return 0;
		/* allocate a new cluster */
		cluster_offset = vdisk_file_lseek(s->fd, 0, SEEK_END);
		if (allocate == 1) {
			/* round to cluster size */
			cluster_offset = (cluster_offset + s->cluster_size - 1) & ~(s->cluster_size - 1);
			vdisk_file_truncate(s->fd, cluster_offset + s->cluster_size);
		}
		/* update L2 table */
		l2_table[l2_index] = cluster_offset;
		vdisk_file_lseek(s->fd,
				 l2_offset + l2_index * sizeof(cluster_offset),
				 SEEK_SET);
		if (vdisk_file_write
		    (s->fd, &cluster_offset,
		     sizeof(cluster_offset)) != sizeof(cluster_offset))
			return 0;
	}
	return cluster_offset;
}

int dyndisk_read(struct dyndisk_state *s, int64_t sector_num, uint8_t * buf,
		 int nb_sectors)
{
	int ret, index_in_cluster, n;
	uint64_t cluster_offset;

	while (nb_sectors > 0) {
		cluster_offset =
		    get_cluster_offset(s, sector_num << 9, 0, 0, 0);
		index_in_cluster = sector_num & (s->cluster_sectors - 1);
		n = s->cluster_sectors - index_in_cluster;
		if (n > nb_sectors)
			n = nb_sectors;
		if (!cluster_offset) {
			memset(buf, 0, VDISK_SECTOR_SIZE * n);
		} else {
			vdisk_file_lseek(s->fd,
					 cluster_offset +
					 index_in_cluster * VDISK_SECTOR_SIZE,
					 SEEK_SET);
			ret = vdisk_file_read(s->fd, buf, n * VDISK_SECTOR_SIZE);
			if (ret != n * VDISK_SECTOR_SIZE)
				return -1;
			if (s->crypt_method)
				encrypt_sectors(sector_num, buf, buf, n, 0,
						&s->aes_decrypt_key);
		}
		nb_sectors -= n;
		sector_num += n;
		buf += n * VDISK_SECTOR_SIZE;
	}
	return 0;
}

int dyndisk_write(struct dyndisk_state *s, int64_t sector_num,
		  const uint8_t * buf, int nb_sectors)
{
	int ret, index_in_cluster, n;
	uint64_t cluster_offset;

	while (nb_sectors > 0) {
		index_in_cluster = sector_num & (s->cluster_sectors - 1);
		n = s->cluster_sectors - index_in_cluster;
		if (n > nb_sectors)
			n = nb_sectors;
		cluster_offset = get_cluster_offset(s, sector_num << 9, 1,
						    index_in_cluster,
						    index_in_cluster + n);
		if (!cluster_offset)
			return -1;
		vdisk_file_lseek(s->fd,
				 cluster_offset +
				 index_in_cluster * VDISK_SECTOR_SIZE, SEEK_SET);
		if (s->crypt_method) {
			encrypt_sectors(sector_num, s->cluster_data, buf, n, 1,
					&s->aes_encrypt_key);
			ret =
			    vdisk_file_write(s->fd, s->cluster_data,
					     n * VDISK_SECTOR_SIZE);
		} else {
			ret = vdisk_file_write(s->fd, buf, n * VDISK_SECTOR_SIZE);
		}
		if (ret != n * VDISK_SECTOR_SIZE)
			return -1;
		nb_sectors -= n;
		sector_num += n;
		buf += n * VDISK_SECTOR_SIZE;
	}
	s->cluster_cache_offset = -1;	/* disable compressed cache */
	return 0;
}

void dyndisk_close(struct dyndisk_state *s)
{
	free(s->l1_table);
	free(s->l2_cache);
	free(s->cluster_cache);
	free(s->cluster_data);
	vdisk_file_close(s->fd);
}

int dyndisk_create(void *fd, struct rsdisk_header *hdr)
{
	int ret;
	int header_size, l1_size, i, shift;
	uint64_t tmp;

	if (!fd)
		return -1;

	header_size = sizeof(struct rsdisk_header);
	header_size = (header_size + CRYPT_BLOCK_SIZE - 1) & ~(CRYPT_BLOCK_SIZE - 1);

	hdr->d_hdr.cluster_bits = 20;
	hdr->d_hdr.l2_bits = 17;

	header_size = (header_size + VDISK_SECTOR_SIZE - 1) & ~(VDISK_SECTOR_SIZE - 1);
	shift = hdr->d_hdr.cluster_bits + hdr->d_hdr.l2_bits;
	l1_size = ((hdr->d_hdr.size) + (1LL << shift) - 1) >> shift;

	hdr->d_hdr.l1_table_offset = header_size;

	/* vdisk_file_write all the data */
	ret = vdisk_file_truncate(fd, header_size);
	if (ret != 0)
		return ret;

	vdisk_file_lseek(fd, header_size, SEEK_SET);
	tmp = 0;
	for (i = 0; i < l1_size; i++) {
		vdisk_file_write(fd, &tmp, sizeof(tmp));
	}

	return 0;
}

