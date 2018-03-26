#include "stdafx.h"
#include "RSA.h"
#include <memory.h>

typedef unsigned char*                  POINTER;
typedef unsigned long int               NN_DIGIT;
typedef unsigned short int              NN_HALF_DIGIT;

#define NN_DIGIT_BITS                   32
#define NN_HALF_DIGIT_BITS              16
#define NN_DIGIT_LEN                    (NN_DIGIT_BITS / 8)
#define MAX_NN_DIGITS                   ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)
#define MAX_NN_DIGIT                    0xFFFFFFFF
#define MAX_NN_HALF_DIGIT               0xFFFF

#define LOW_HALF(x)                     ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x)                    (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x)                 (((NN_DIGIT)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x)                    (unsigned int)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x)                   (unsigned int)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

#define NN_ASSIGN_DIGIT(a, b, digits)   {NN_AssignZero (a, digits); a[0] = b;}
#define R_memset(x, y, z)               memset(x, y, z)
#define R_memcpy(x, y, z)               memcpy(x, y, z)
#define R_memcmp(x, y, z)               memcmp(x, y, z)


/*
 * Assigns a = 0. 
 */
static void NN_AssignZero (NN_DIGIT *a, unsigned int digits)
{
	if(digits)
	{
		do
		{
			*a++ = 0;
		}while(--digits);
	}
}


/*
 * Returns the significant length of a in digits. 
 */
static unsigned int NN_Digits (NN_DIGIT *a, unsigned int digits)
{
	
	if(digits) 
	{
		digits--;
		do 
		{
			if(*(a+digits))
				break;
		}while(digits--);

		return(digits + 1);
	}
	
	return(digits);
}


/* 
 * Assigns a = b. 
 */
static void NN_Assign (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits)
{
	if(digits)
	{
		do
		{
			*a++ = *b++;
		}while(--digits);
	}
}

/* 
 * Returns sign of a - b.
 */
static int NN_Cmp (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits)
{
	
	if(digits)
	{
		do
		{
			digits--;
			if(*(a+digits) > *(b+digits))
				return(1);
			if(*(a+digits) < *(b+digits))
				return(-1);
		}while(digits);
	}
	
	return (0);
}

/*
 * Returns the significant length of a in bits, where a is a digit. 
 */
static unsigned int NN_DigitBits (NN_DIGIT a)
{
	unsigned int i;
	
	for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
		if (a == 0)
			break;
		
	return (i);
}

/*
 * Computes a * b, result stored in high and low. 
 */
static void dmult(NN_DIGIT a, NN_DIGIT b, NN_DIGIT *high, NN_DIGIT *low)
{
	NN_HALF_DIGIT al, ah, bl, bh;
	NN_DIGIT m1, m2, m, ml, mh, carry = 0;
	
	al = (NN_HALF_DIGIT)LOW_HALF(a);
	ah = (NN_HALF_DIGIT)HIGH_HALF(a);
	bl = (NN_HALF_DIGIT)LOW_HALF(b);
	bh = (NN_HALF_DIGIT)HIGH_HALF(b);
	
	*low = (NN_DIGIT) al*bl;
	*high = (NN_DIGIT) ah*bh;
	
	m1 = (NN_DIGIT) al*bh;
	m2 = (NN_DIGIT) ah*bl;
	m = m1 + m2;
	
	if(m < m1)
        carry = 1L << (NN_DIGIT_BITS / 2);
	
	ml = (m & MAX_NN_HALF_DIGIT) << (NN_DIGIT_BITS / 2);
	mh = m >> (NN_DIGIT_BITS / 2);
	
	*low += ml;
	
	if(*low < ml)
		carry++;
	
	*high += carry + mh;
}

/*
 * Computes a = b * 2^c (i.e., shifts left c bits), returning carry.
 * Requires c < NN_DIGIT_BITS. 
 */
static NN_DIGIT NN_LShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits)
{
	NN_DIGIT temp, carry = 0;
	unsigned int t;
	
	if(c < NN_DIGIT_BITS)
		if(digits)
		{
			t = NN_DIGIT_BITS - c;
			
			do 
			{
				temp = *b++;
				*a++ = (temp << c) | carry;
				carry = c ? (temp >> t) : 0;
			}while(--digits);
		}
		
	return (carry);
}

/*
 *
 */
static NN_DIGIT subdigitmult(NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, unsigned int digits)
{
	NN_DIGIT borrow, thigh, tlow;
	unsigned int i;
	
	borrow = 0;
	
	if(c != 0) 
	{
		for(i = 0; i < digits; i++) 
		{
			dmult(c, d[i], &thigh, &tlow);
			if((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
				borrow = 1;
			else
				borrow = 0;
			if((a[i] -= tlow) > (MAX_NN_DIGIT - tlow))
				borrow++;
			borrow += thigh;
		}
	}
	
	return (borrow);
}

/*
 * Computes a = b - c. Returns borrow.
 *
 * Lengths: a[digits], b[digits], c[digits].
 */
static NN_DIGIT NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
	NN_DIGIT temp, borrow = 0;
	
	if(digits)
		do
		{
			/*
			 * Bug fix 16/10/95 - JSK, code below removed, caused bug with
			 * Sun Compiler SC4.
		     *
		     * if((temp = (*b++) - borrow) == MAX_NN_DIGIT)
		     * temp = MAX_NN_DIGIT - *c++;
             */
			
			temp = *b - borrow;
            b++;
            if(temp == MAX_NN_DIGIT) 
			{
                temp = MAX_NN_DIGIT - *c;
                c++;
            }
			else
			{
				/* Patch to prevent bug for Sun CC */
                if((temp -= *c) > (MAX_NN_DIGIT - *c))
					borrow = 1;
				else
					borrow = 0;
                c++;
            }
			*a++ = temp;
		}while(--digits);
		
	return(borrow);
}

/*
 * Computes a = b + c. Returns carry.
 *
 * Lengths: a[digits], b[digits], c[digits].
 */
static NN_DIGIT NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
	NN_DIGIT temp, carry = 0;
	
	if(digits)
		do 
		{
			if((temp = (*b++) + carry) < carry)
				temp = *c++;
            else
			{
				/* Patch to prevent bug for Sun CC */
                if((temp += *c) < *c)
					carry = 1;
				else
					carry = 0;
                c++;
            }
			*a++ = temp;
		}while(--digits);
		
		return (carry);
}

/*
 * Computes a = c div 2^c (i.e., shifts right c bits), returning carry.
 * Requires: c < NN_DIGIT_BITS. 
 */
static NN_DIGIT NN_RShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits)
{
	NN_DIGIT temp, carry = 0;
	unsigned int t;
	
	if(c < NN_DIGIT_BITS)
		if(digits) 
		{
			
			t = NN_DIGIT_BITS - c;
			
			do 
			{
				digits--;
				temp = *(b+digits);
				*(a+digits) = (temp >> c) | carry;
				carry = c ? (temp << t) : 0;
			}while(digits);
		}
		
	return (carry);
}


/* 
 * Computes a = c div d and b = c mod d.
 *
 * Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
 * Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
 * dDigits < MAX_NN_DIGITS.
 */
static void NN_Div (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits)
{
	NN_DIGIT ai, cc[2*MAX_NN_DIGITS+1], dd[MAX_NN_DIGITS], s;
	NN_DIGIT t[2], u, v, *ccptr;
	NN_HALF_DIGIT aHigh, aLow, cHigh, cLow;
	int i;
	unsigned int ddDigits, shift;
	
	ddDigits = NN_Digits (d, dDigits);
	if(ddDigits == 0)
		return;
	
	shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
	NN_AssignZero (cc, ddDigits);
	cc[cDigits] = NN_LShift (cc, c, shift, cDigits);
	NN_LShift (dd, d, shift, ddDigits);
	s = dd[ddDigits-1];
	
	NN_AssignZero (a, cDigits);
	
	for (i = cDigits-ddDigits; i >= 0; i--)
	{
		if (s == MAX_NN_DIGIT)
			ai = cc[i+ddDigits];
		else
		{
			ccptr = &cc[i+ddDigits-1];
			
			s++;
			cHigh = (NN_HALF_DIGIT)HIGH_HALF (s);
			cLow = (NN_HALF_DIGIT)LOW_HALF (s);
			
			*t = *ccptr;
			*(t+1) = *(ccptr+1);
			
			if (cHigh == MAX_NN_HALF_DIGIT)
				aHigh = (NN_HALF_DIGIT)HIGH_HALF (*(t+1));
			else
				aHigh = (NN_HALF_DIGIT)(*(t+1) / (cHigh + 1));
			u = (NN_DIGIT)aHigh * (NN_DIGIT)cLow;
			v = (NN_DIGIT)aHigh * (NN_DIGIT)cHigh;
			if ((*t -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
				t[1]--;
			*(t+1) -= HIGH_HALF (u);
			*(t+1) -= v;
			
			while ((*(t+1) > cHigh) ||
				((*(t+1) == cHigh) && (*t >= TO_HIGH_HALF (cLow)))) {
				if ((*t -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow))
					t[1]--;
				*(t+1) -= cHigh;
				aHigh++;
			}
			
			if (cHigh == MAX_NN_HALF_DIGIT)
				aLow = (NN_HALF_DIGIT)LOW_HALF (*(t+1));
			else
				aLow =
				(NN_HALF_DIGIT)((TO_HIGH_HALF (*(t+1)) + HIGH_HALF (*t)) / (cHigh + 1));
			u = (NN_DIGIT)aLow * (NN_DIGIT)cLow;
			v = (NN_DIGIT)aLow * (NN_DIGIT)cHigh;
			if ((*t -= u) > (MAX_NN_DIGIT - u))
				t[1]--;
			if ((*t -= TO_HIGH_HALF (v)) > (MAX_NN_DIGIT - TO_HIGH_HALF (v)))
				t[1]--;
			*(t+1) -= HIGH_HALF (v);
			
			while ((*(t+1) > 0) || ((*(t+1) == 0) && *t >= s))
			{
				if ((*t -= s) > (MAX_NN_DIGIT - s))
					t[1]--;
				aLow++;
			}
			
			ai = TO_HIGH_HALF (aHigh) + aLow;
			s--;
		}
		
		cc[i+ddDigits] -= subdigitmult(&cc[i], &cc[i], ai, dd, ddDigits);
		
		while (cc[i+ddDigits] || (NN_Cmp (&cc[i], dd, ddDigits) >= 0)) 
		{
			ai++;
			cc[i+ddDigits] -= NN_Sub (&cc[i], &cc[i], dd, ddDigits);
		}
		
		a[i] = ai;
	}
	
	NN_AssignZero (b, dDigits);
	NN_RShift (b, cc, shift, ddDigits);
}

/*
 * Computes a = b * c.
 *
 * Lengths: a[2*digits], b[digits], c[digits].
 * Assumes digits < MAX_NN_DIGITS.
 */
static void NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
	NN_DIGIT t[2*MAX_NN_DIGITS];
	NN_DIGIT dhigh, dlow, carry;
	unsigned int bDigits, cDigits, i, j;
	
	NN_AssignZero (t, 2 * digits);
	
	bDigits = NN_Digits (b, digits);
	cDigits = NN_Digits (c, digits);
	
	for (i = 0; i < bDigits; i++)
	{
		carry = 0;
		if(*(b+i) != 0)
		{
			for(j = 0; j < cDigits; j++)
			{
				dmult(*(b+i), *(c+j), &dhigh, &dlow);
				if((*(t+(i+j)) = *(t+(i+j)) + carry) < carry)
					carry = 1;
				else
					carry = 0;
				if((*(t+(i+j)) += dlow) < dlow)
					carry++;
				carry += dhigh;
			}
		}
		*(t+(i+cDigits)) += carry;
	}
	
	
	NN_Assign(a, t, 2 * digits);
}

/*
 * Computes a = b mod c.
 *
 * Lengths: a[cDigits], b[bDigits], c[cDigits].
 * Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
 */
static void NN_Mod (NN_DIGIT *a, NN_DIGIT *b, unsigned int bDigits, NN_DIGIT *c, unsigned int cDigits)
{
    NN_DIGIT t[2 * MAX_NN_DIGITS];
	NN_Div (t, a, b, bDigits, c, cDigits);
}

/*
 * Computes a = b * c mod d.
 *
 * Lengths: a[digits], b[digits], c[digits], d[digits].
 * Assumes d > 0, digits < MAX_NN_DIGITS.
 */
static void NN_ModMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, unsigned int digits)
{
    NN_DIGIT t[2*MAX_NN_DIGITS];
	
	NN_Mult (t, b, c, digits);
    NN_Mod (a, t, 2 * digits, d, digits);
}

/* 
 * Computes a = b^c mod d.
 *
 * Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
 * Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS.
 */
static void NN_ModExp (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits)
{
    NN_DIGIT bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
    int i;
	unsigned int ciBits, j, s;
	
	/* Store b, b^2 mod d, and b^3 mod d. */
	NN_Assign (bPower[0], b, dDigits);
	NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
    NN_ModMult (bPower[2], bPower[1], b, d, dDigits);
	
    NN_ASSIGN_DIGIT (t, 1, dDigits);
	
	cDigits = NN_Digits (c, cDigits);
    for (i = cDigits - 1; i >= 0; i--)
	{
		ci = c[i];
		ciBits = NN_DIGIT_BITS;
		
		/* Scan past leading zero bits of most significant digit. */
		if (i == (int)(cDigits - 1)) 
		{
			while (! DIGIT_2MSB (ci))
			{
				ci <<= 2;
				ciBits -= 2;
			}
        }
		
        for (j = 0; j < ciBits; j += 2, ci <<= 2) 
		{
			/* Compute t = t^4 * b^s mod d, where s = two MSB's of ci. */
            NN_ModMult (t, t, t, d, dDigits);
            NN_ModMult (t, t, t, d, dDigits);
            if ((s = DIGIT_2MSB (ci)) != 0)
				NN_ModMult (t, t, bPower[s-1], d, dDigits);
        }
    }
	
	NN_Assign (a, t, dDigits);
}

/*
 * Decodes character string b into a, where character string is ordered
 * from most to least significant.
 *
 * Lengths: a[digits], b[len].
 * Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
 * significant bytes are truncated.)
 */
static void NN_Decode (NN_DIGIT *a, unsigned int digits, unsigned char *b, unsigned int len)
{
	NN_DIGIT t;
	unsigned int i, u;
	int j;
	
	for (i = 0, j = len - 1; i < digits && j >= 0; i++)
	{
		t = 0;
		for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
            t |= ((NN_DIGIT)b[j]) << u;
        a[i] = t;
	}
	
	for (; i < digits; i++)
		a[i] = 0;
}

/* 
 * Encodes b into character string a, where character string is ordered
 * from most to least significant.
 *
 * Lengths: a[len], b[digits].
 * Assumes NN_Bits (b, digits) <= 8 * len. (Otherwise most significant
 * digits are truncated.)
 */
static void NN_Encode (unsigned char *a, unsigned int len, NN_DIGIT *b, unsigned int digits)
{
	NN_DIGIT t;
    unsigned int i, u;
	int j;
	
	for (i = 0, j = len - 1; i < digits && j >= 0; i++)
	{
		t = b[i];
		for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
			a[j] = (unsigned char)(t >> u);
	}
	
	for (; j >= 0; j--)
		a[j] = 0;
}

/*
 *  Raw RSA public-key operation. Output has same length as modulus.
 *  Requires input < modulus.
 */
static int rsapublicfunc(unsigned char *output, unsigned int *outputLen, unsigned char *input, unsigned int inputLen, R_RSA_PUBLIC_KEY *publicKey)
{
    NN_DIGIT c[MAX_NN_DIGITS], e[MAX_NN_DIGITS], m[MAX_NN_DIGITS], n[MAX_NN_DIGITS];
    unsigned int eDigits, nDigits;

    /* decode the required RSA function input data */
    NN_Decode(m, MAX_NN_DIGITS, input, inputLen);
    NN_Decode(n, MAX_NN_DIGITS, publicKey->modulus, MAX_RSA_MODULUS_LEN);
    NN_Decode(e, MAX_NN_DIGITS, publicKey->exponent, MAX_RSA_MODULUS_LEN);

    nDigits = NN_Digits(n, MAX_NN_DIGITS);
    eDigits = NN_Digits(e, MAX_NN_DIGITS);

    if(NN_Cmp(m, n, nDigits) >= 0)
        return(RE_DATA);

    *outputLen = (publicKey->bits + 7) / 8;

    /* Compute c = m^e mod n.  To perform actual RSA calc.*/
    NN_ModExp(c, m, e, eDigits, n, nDigits);

    /* encode output to standard form */
    NN_Encode(output, *outputLen, c, nDigits);

    /* Clear sensitive information. */
    R_memset((POINTER)c, 0, sizeof(c));
    R_memset((POINTER)m, 0, sizeof(m));

    return(RE_SUCCESS);
}

/*
 *  Raw RSA private-key operation. Output has same length as modulus.
 *  Requires input < modulus.
 */
static int rsaprivatefunc(unsigned char *output, unsigned int *outputLen, unsigned char *input, unsigned int inputLen, R_RSA_PRIVATE_KEY *privateKey)
{
    NN_DIGIT  c[MAX_NN_DIGITS],   cP[MAX_NN_DIGITS], cQ[MAX_NN_DIGITS],
             dP[MAX_NN_DIGITS],   dQ[MAX_NN_DIGITS], mP[MAX_NN_DIGITS],
             mQ[MAX_NN_DIGITS],    n[MAX_NN_DIGITS],  p[MAX_NN_DIGITS],
			 q[MAX_NN_DIGITS],  qInv[MAX_NN_DIGITS],  t[MAX_NN_DIGITS];
    unsigned int cDigits, nDigits, pDigits;

    /* decode required input data from standard form */
    NN_Decode(c, MAX_NN_DIGITS, input, inputLen); 

    /* private key data */
    NN_Decode(p, MAX_NN_DIGITS, privateKey->prime[0], MAX_RSA_PRIME_LEN);
    NN_Decode(q, MAX_NN_DIGITS, privateKey->prime[1], MAX_RSA_PRIME_LEN);
    NN_Decode(dP, MAX_NN_DIGITS, privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
    NN_Decode(dQ, MAX_NN_DIGITS, privateKey->primeExponent[1], MAX_RSA_PRIME_LEN);
    NN_Decode(n, MAX_NN_DIGITS, privateKey->modulus, MAX_RSA_MODULUS_LEN);
    NN_Decode(qInv, MAX_NN_DIGITS, privateKey->coefficient, MAX_RSA_PRIME_LEN);

    /* work out lengths of input components */
    cDigits = NN_Digits(c, MAX_NN_DIGITS);
    pDigits = NN_Digits(p, MAX_NN_DIGITS);
    nDigits = NN_Digits(n, MAX_NN_DIGITS);


    if(NN_Cmp(c, n, nDigits) >= 0)
        return(RE_DATA);

    *outputLen = (privateKey->bits + 7) / 8;

    /* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has length at most pDigits, i.e., p > q.) */
    NN_Mod(cP, c, cDigits, p, pDigits);
    NN_Mod(cQ, c, cDigits, q, pDigits);

    NN_AssignZero(mP, nDigits);
    NN_ModExp(mP, cP, dP, pDigits, p, pDigits);

    NN_AssignZero(mQ, nDigits);
    NN_ModExp(mQ, cQ, dQ, pDigits, q, pDigits);

    /* Chinese Remainder Theorem: m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ. */
    if(NN_Cmp(mP, mQ, pDigits) >= 0)
	{
        NN_Sub(t, mP, mQ, pDigits);
    }
	else
	{
        NN_Sub(t, mQ, mP, pDigits);
        NN_Sub(t, p, t, pDigits);
    }

    NN_ModMult(t, t, qInv, p, pDigits);
    NN_Mult(t, t, q, pDigits);
    NN_Add(t, t, mQ, nDigits);

    /* encode output to standard form */
    NN_Encode (output, *outputLen, t, nDigits);

    /* Clear sensitive information. */
    R_memset((POINTER)c, 0, sizeof(c));
    R_memset((POINTER)cP, 0, sizeof(cP));
    R_memset((POINTER)cQ, 0, sizeof(cQ));
    R_memset((POINTER)dP, 0, sizeof(dP));
    R_memset((POINTER)dQ, 0, sizeof(dQ));
    R_memset((POINTER)mP, 0, sizeof(mP));
    R_memset((POINTER)mQ, 0, sizeof(mQ));
    R_memset((POINTER)p, 0, sizeof(p));
    R_memset((POINTER)q, 0, sizeof(q));
    R_memset((POINTER)qInv, 0, sizeof(qInv));
    R_memset((POINTER)t, 0, sizeof(t));
    return(RE_SUCCESS);
}

/* RSA decryption, according to RSADSI's PKCS #1. */
int RSAPublicDecrypt(unsigned char *output, unsigned int *outputLen, unsigned char *input, unsigned int inputLen, R_RSA_PUBLIC_KEY *publicKey)
{
    int status;
    unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
    unsigned int i, modulusLen, pkcsBlockLen;

    modulusLen = (publicKey->bits + 7) / 8;

    if(inputLen > modulusLen)
        return(RE_LEN);

    status = rsapublicfunc(pkcsBlock, &pkcsBlockLen, input, inputLen, publicKey);
    if(status)
        return(status);

    if(pkcsBlockLen != modulusLen)
        return(RE_LEN);

    /* Require block type 1. */
    if((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
        return(RE_DATA);

    for(i = 2; i < modulusLen-1; i++)
        if(*(pkcsBlock+i) != 0xff)
            break;

    /* separator check */

    if(pkcsBlock[i++] != 0)
        return(RE_DATA);

    *outputLen = modulusLen - i;

    if(*outputLen + 11 > modulusLen)
        return(RE_DATA);

    R_memcpy((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);

    /* Clear sensitive information. */
    R_memset((POINTER)pkcsBlock, 0, sizeof(pkcsBlock));

    return(RE_SUCCESS);
}

/* RSA encryption, according to RSADSI's PKCS #1. */
int RSAPrivateEncrypt(unsigned char *output, unsigned int *outputLen, unsigned char *input, unsigned int inputLen, R_RSA_PRIVATE_KEY *privateKey)
{
    int status;
    unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
    unsigned int i, modulusLen;

    modulusLen = (privateKey->bits + 7) / 8;

    if(inputLen + 11 > modulusLen)
        return (RE_LEN);

    *pkcsBlock = 0;
    /* block type 1 */
    *(pkcsBlock+1) = 1;

    for (i = 2; i < modulusLen - inputLen - 1; i++)
        *(pkcsBlock+i) = 0xff;

    /* separator */
    pkcsBlock[i++] = 0;

    R_memcpy((POINTER)&pkcsBlock[i], (POINTER)input, inputLen);

    status = rsaprivatefunc(output, outputLen, pkcsBlock, modulusLen, privateKey);

    /* Clear sensitive information. */
    R_memset((POINTER)pkcsBlock, 0, sizeof(pkcsBlock));

    return(status);
}
