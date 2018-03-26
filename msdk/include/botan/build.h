
#ifndef BOTAN_BUILD_CONFIG_H__
#define BOTAN_BUILD_CONFIG_H__

/* This file was automatically generated Sun Sep 18 11:50:23 2016 UTC */

#define BOTAN_VERSION_MAJOR 1
#define BOTAN_VERSION_MINOR 8
#define BOTAN_VERSION_PATCH 15

#ifndef BOTAN_DLL
  #define BOTAN_DLL 
#endif

/* Chunk sizes */
#define BOTAN_DEFAULT_BUFFER_SIZE 4096
#define BOTAN_MEM_POOL_CHUNK_SIZE 64*1024

/* BigInt toggles */
#define BOTAN_MP_WORD_BITS 32
#define BOTAN_KARAT_MUL_THRESHOLD 32
#define BOTAN_KARAT_SQR_THRESHOLD 32
#define BOTAN_PRIVATE_KEY_OP_BLINDING_BITS 64

/* PK key consistency checking toggles */
#define BOTAN_PUBLIC_KEY_STRONG_CHECKS_ON_LOAD 1
#define BOTAN_PRIVATE_KEY_STRONG_CHECKS_ON_LOAD 1
#define BOTAN_PRIVATE_KEY_STRONG_CHECKS_ON_GENERATE 1

/* Should we use GCC-style inline assembler? */
#if !defined(BOTAN_USE_GCC_INLINE_ASM) && defined(__GNUG__)
  #define BOTAN_USE_GCC_INLINE_ASM 1
#endif

#ifndef BOTAN_USE_GCC_INLINE_ASM
  #define BOTAN_USE_GCC_INLINE_ASM 0
#endif

/* Target identification and feature test macros */
#define BOTAN_TARGET_OS_IS_WINDOWS
#define BOTAN_TARGET_OS_HAS_WIN32_VIRTUAL_LOCK

#define BOTAN_TARGET_ARCH_IS_AMD64
#define BOTAN_TARGET_CPU_IS_LITTLE_ENDIAN
#define BOTAN_TARGET_UNALIGNED_LOADSTOR_OK 1



/* Module definitions */
#define BOTAN_HAS_ADLER32
#define BOTAN_HAS_AES
#define BOTAN_HAS_ALGORITHM_FACTORY
#define BOTAN_HAS_ANSI_X919_MAC
#define BOTAN_HAS_ARC4
#define BOTAN_HAS_ASN1
#define BOTAN_HAS_AUTO_SEEDING_RNG
#define BOTAN_HAS_BASE64_CODEC
#define BOTAN_HAS_BIGINT
#define BOTAN_HAS_BIGINT_MATH
#define BOTAN_HAS_BLOCK_CIPHER
#define BOTAN_HAS_BLOWFISH
#define BOTAN_HAS_CAST
#define BOTAN_HAS_CBC
#define BOTAN_HAS_CBC_MAC
#define BOTAN_HAS_CFB
#define BOTAN_HAS_CIPHER_MODEBASE
#define BOTAN_HAS_CIPHER_MODE_PADDING
#define BOTAN_HAS_CMAC
#define BOTAN_HAS_CMS
#define BOTAN_HAS_CRC24
#define BOTAN_HAS_CRC32
#define BOTAN_HAS_CRYPTO_BOX
#define BOTAN_HAS_CTR
#define BOTAN_HAS_CTS
#define BOTAN_HAS_DEFAULT_ENGINE
#define BOTAN_HAS_DES
#define BOTAN_HAS_DIFFIE_HELLMAN
#define BOTAN_HAS_DLIES
#define BOTAN_HAS_DL_GROUP
#define BOTAN_HAS_DL_PUBLIC_KEY_FAMILY
#define BOTAN_HAS_DSA
#define BOTAN_HAS_EAX
#define BOTAN_HAS_ECB
#define BOTAN_HAS_ELGAMAL
#define BOTAN_HAS_EME1
#define BOTAN_HAS_EME_PKCS1v15
#define BOTAN_HAS_EMSA1
#define BOTAN_HAS_EMSA1_BSI
#define BOTAN_HAS_EMSA2
#define BOTAN_HAS_EMSA3
#define BOTAN_HAS_EMSA4
#define BOTAN_HAS_EMSA_RAW
#define BOTAN_HAS_ENGINES
#define BOTAN_HAS_ENTROPY_SRC_CAPI
#define BOTAN_HAS_ENTROPY_SRC_WIN32
#define BOTAN_HAS_FILTERS
#define BOTAN_HAS_FORK_256
#define BOTAN_HAS_GOST_28147_89
#define BOTAN_HAS_GOST_34_11
#define BOTAN_HAS_HASH_ID
#define BOTAN_HAS_HAS_160
#define BOTAN_HAS_HEX_CODEC
#define BOTAN_HAS_HMAC
#define BOTAN_HAS_HMAC_RNG
#define BOTAN_HAS_IDEA
#define BOTAN_HAS_IF_PUBLIC_KEY_FAMILY
#define BOTAN_HAS_KASUMI
#define BOTAN_HAS_KDF1
#define BOTAN_HAS_KDF2
#define BOTAN_HAS_KDF_BASE
#define BOTAN_HAS_KEYPAIR_TESTING
#define BOTAN_HAS_LIBSTATE_MODULE
#define BOTAN_HAS_LION
#define BOTAN_HAS_LUBY_RACKOFF
#define BOTAN_HAS_MARS
#define BOTAN_HAS_MD2
#define BOTAN_HAS_MD4
#define BOTAN_HAS_MD5
#define BOTAN_HAS_MDX_HASH_FUNCTION
#define BOTAN_HAS_MGF1
#define BOTAN_HAS_MISTY1
#define BOTAN_HAS_MUTEX_NOOP
#define BOTAN_HAS_MUTEX_WIN32
#define BOTAN_HAS_MUTEX_WRAPPERS
#define BOTAN_HAS_NOEKEON
#define BOTAN_HAS_NYBERG_RUEPPEL
#define BOTAN_HAS_OFB
#define BOTAN_HAS_OID_LOOKUP
#define BOTAN_HAS_OPENPGP_CODEC
#define BOTAN_HAS_PARALLEL_HASH
#define BOTAN_HAS_PASSWORD_BASED_ENCRYPTION
#define BOTAN_HAS_PBE_PKCS_V15
#define BOTAN_HAS_PBE_PKCS_V20
#define BOTAN_HAS_PBKDF1
#define BOTAN_HAS_PBKDF2
#define BOTAN_HAS_PEM_CODEC
#define BOTAN_HAS_PGPS2K
#define BOTAN_HAS_PK_PADDING
#define BOTAN_HAS_PUBLIC_KEY_CRYPTO
#define BOTAN_HAS_RANDPOOL
#define BOTAN_HAS_RC2
#define BOTAN_HAS_RC5
#define BOTAN_HAS_RC6
#define BOTAN_HAS_RIPEMD_128
#define BOTAN_HAS_RIPEMD_160
#define BOTAN_HAS_RSA
#define BOTAN_HAS_RUNTIME_BENCHMARKING
#define BOTAN_HAS_RW
#define BOTAN_HAS_SAFER
#define BOTAN_HAS_SALSA20
#define BOTAN_HAS_SEED
#define BOTAN_HAS_SELFTESTS
#define BOTAN_HAS_SERPENT
#define BOTAN_HAS_SHA1
#define BOTAN_HAS_SHA2
#define BOTAN_HAS_SKEIN_512
#define BOTAN_HAS_SKIPJACK
#define BOTAN_HAS_SQUARE
#define BOTAN_HAS_SSL3_MAC
#define BOTAN_HAS_SSL_V3_PRF
#define BOTAN_HAS_STREAM_CIPHER
#define BOTAN_HAS_TEA
#define BOTAN_HAS_TIGER
#define BOTAN_HAS_TIMER
#define BOTAN_HAS_TIMER_WIN32
#define BOTAN_HAS_TLS_V10_PRF
#define BOTAN_HAS_TURING
#define BOTAN_HAS_TWOFISH
#define BOTAN_HAS_UTIL_FUNCTIONS
#define BOTAN_HAS_WHIRLPOOL
#define BOTAN_HAS_WID_WAKE
#define BOTAN_HAS_X509
#define BOTAN_HAS_X931_RNG
#define BOTAN_HAS_X942_PRF
#define BOTAN_HAS_XTEA
#define BOTAN_HAS_XTS

/* Local configuration options */


/*
endprot@endprot-PC ran 'configure.py --cc=msvc --disable-shared'

Target
-------
Compiler: cl.exe /MD /O2 
Arch: amd64/amd64
OS: windows

Modules
-------
adler32
aes
algo_factory
alloc
arc4
asn1
auto_rng
base64
benchmark
bigint
block
blowfish
buf_comp
cast
cbc
cbc_mac
cfb
cmac
cms
crc24
crc32
cryptoapi_rng
cryptobox
ctr
cts
datastor
def_engine
des
dh
dl_algo
dl_group
dlies
dsa
eax
ecb
elgamal
eme1
eme_pkcs
emsa1
emsa1_bsi
emsa2
emsa3
emsa4
emsa_raw
engine
entropy
filters
fork256
gost_28147
gost_3411
has160
hash
hash_id
hex
hmac
hmac_rng
idea
if_algo
kasumi
kdf
kdf1
kdf2
keypair
libstate
lion
lubyrack
mac
mars
md2
md4
md5
mdx_hash
mem_pool
mgf1
misty1
mode_pad
modes
monty_generic
mp_generic
mulop_generic
mutex
noekeon
noop_mutex
nr
numbertheory
ofb
oid_lookup
openpgp
par_hash
pbe
pbes1
pbes2
pbkdf1
pbkdf2
pem
pgps2k
pk_codecs
pk_pad
pubkey
randpool
rc2
rc5
rc6
rmd128
rmd160
rng
rsa
rw
s2k
safer
salsa20
seed
selftest
serpent
sha1
sha2
skein
skipjack
square
ssl3mac
ssl_prf
stream
sym_algo
system_alloc
tea
tiger
timer
tls_prf
turing
twofish
utils
whirlpool
wid_wake
win32_crit_section
win32_query_perf_ctr
win32_stats
x509
x919_mac
x931_rng
x942_prf
xtea
xts
*/

#endif
