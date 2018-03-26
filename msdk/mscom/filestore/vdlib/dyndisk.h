#include "aes.h"
#include "vdisk.h"

#ifndef _DYNDISK_H_
#define _DYNDISK_H_


#ifdef WIN32
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
# include <io.h>
#else
# include <stdint.h>
# include <unistd.h>
#endif

#define L2_CACHE_SIZE 16
#define CRYPT_BLOCK_SIZE 16

typedef struct dyndisk_state {
	void *fd;
	int cluster_bits;
	int cluster_size;
	int cluster_sectors;
	int l2_bits;
	int l2_size;
	int l1_size;
	uint64_t cluster_offset_mask;
	uint64_t l1_table_offset;
	uint64_t *l1_table;
	uint64_t *l2_cache;
	uint64_t l2_cache_offsets[L2_CACHE_SIZE];
	uint32_t l2_cache_counts[L2_CACHE_SIZE];
	uint8_t *cluster_cache;
	uint8_t *cluster_data;
	uint64_t cluster_cache_offset;
	uint32_t crypt_method;	/* current crypt method, 0 if no key yet */
	uint32_t crypt_method_header;
	AES_KEY aes_encrypt_key;
	AES_KEY aes_decrypt_key;
	int64_t total_sectors;
} dyndisk_state;

int dyndisk_create(void *fd, struct rsdisk_header *hdr);
int dyndisk_open(struct dyndisk_state *s, struct rsdisk_header *hdr);
int dyndisk_read(struct dyndisk_state *s, int64_t sector_num,
		 uint8_t * buf, int nb_sectors);
int dyndisk_write(struct dyndisk_state *s, int64_t sector_num,
		  const uint8_t * buf, int nb_sectors);
void dyndisk_close(struct dyndisk_state *s);

#endif /* _DYNDISK_H_ */
