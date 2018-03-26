#ifndef _AES_H_
#define _AES_H_

#define VDISK_SECTOR_SIZE 512

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

#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

struct aes_key_st {
	unsigned int rd_key[4 * (AES_MAXNR + 1)];
	int rounds;
};
typedef struct aes_key_st AES_KEY;

int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
			AES_KEY * key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
			AES_KEY * key);

void AES_encrypt(const unsigned char *in, unsigned char *out,
		 const AES_KEY * key);
void AES_decrypt(const unsigned char *in, unsigned char *out,
		 const AES_KEY * key);
void AES_cbc_encrypt(const unsigned char *in, unsigned char *out,
		     const unsigned long length, const AES_KEY * key,
		     unsigned char *ivec, const int enc);
void encrypt_sectors(int64_t sector_num, uint8_t * out_buf,
		     const uint8_t * in_buf, int nb_sectors,
		     int enc, const AES_KEY * key);
void encrypt_aes_blocks(uint8_t * out_buf, const uint8_t * in_buf,
			const unsigned long length, int enc,
			const AES_KEY * key);

#endif				/* _AES_H_ */
