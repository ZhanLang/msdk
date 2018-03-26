#ifndef _RSA_H_
#define _RSA_H_


#define MAX_RSA_MODULUS_BITS                           1024
#define MAX_RSA_MODULUS_LEN                            ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS                             ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN                              ((MAX_RSA_PRIME_BITS + 7) / 8)

#define RE_DATA                                        0xFFFFFFFF
#define RE_LEN                                         0xFFFFFFFE
#define RE_SUCCESS                                     0x00000000

/* RSA public and private key. */
typedef struct {
	unsigned short int bits;                           /* length in bits of modulus */
	unsigned char modulus[MAX_RSA_MODULUS_LEN];        /* modulus */
	unsigned char exponent[MAX_RSA_MODULUS_LEN];       /* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct {
	unsigned short int bits;                           /* length in bits of modulus */
	unsigned char modulus[MAX_RSA_MODULUS_LEN];        /* modulus */
	unsigned char publicExponent[MAX_RSA_MODULUS_LEN]; /* public exponent */
	unsigned char exponent[MAX_RSA_MODULUS_LEN];       /* private exponent */
	unsigned char prime[2][MAX_RSA_PRIME_LEN];         /* prime factors */
	unsigned char primeExponent[2][MAX_RSA_PRIME_LEN]; /* exponents for CRT */
	unsigned char coefficient[MAX_RSA_PRIME_LEN];      /* CRT coefficient */
} R_RSA_PRIVATE_KEY;

int RSAPublicDecrypt(unsigned char *output, unsigned int *outputLen, unsigned char *input, unsigned int inputLen, R_RSA_PUBLIC_KEY *publicKey);
int RSAPrivateEncrypt(unsigned char *output, unsigned int *outputLen, unsigned char *input, unsigned int inputLen, R_RSA_PRIVATE_KEY *privateKey);


#endif