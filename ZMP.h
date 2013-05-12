// Qin ZHOU's Multiple Precision Library
#ifndef ZMP_H_INCLUDED
#define ZMP_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define MAXN 4096
#define MAXL MAXN/32
#define NUM 1024 //54  // number of bits of our desired prime limit
#define RANGE 0
#define MAX_U32 0xFFFFFFFF

typedef struct timeval Timeval;

typedef struct bigNum{
	u_int32_t num[MAXL];
	int bits;
} BigNum;

typedef struct bigNum2{
	BigNum quotient;	// shang
	BigNum remainder;	// yu shu
} BigNum2;

int rand_range(int min_n, int max_n);

bool printBigNum(BigNum n);

u_int32_t checkBit(BigNum a, int bit);

int compareBigNum(BigNum bn1, BigNum bn2);

bool initBigNum(BigNum *ret);

BigNum copyBigNum(BigNum n);

BigNum copyBigNum2(BigNum n, int s_index, int range);

BigNum rdm(int bits);

BigNum rdm2(BigNum minN, BigNum maxN);

int countIntBits(int num);

BigNum sub(BigNum bn1, BigNum bn2, BigNum modN);

BigNum sub_N(BigNum bn1, BigNum bn2);

BigNum int2BigNum(int num, BigNum modN);

BigNum int2BigNum_N(int num);

bool even(BigNum m);

BigNum shiftR(BigNum m);

BigNum shiftL_N(BigNum a, int d);

BigNum shiftL(BigNum a, int d, BigNum modN);	// with errors! Don't use!

BigNum add_N(BigNum a, BigNum b);

BigNum add_N2(BigNum a, BigNum b);

BigNum add(BigNum a, BigNum b, BigNum modN);

bool checkBitsBigNum(BigNum n);

BigNum mul(BigNum a, BigNum b, BigNum modN);

BigNum mul1(BigNum a, BigNum b, BigNum modN);

BigNum mul_N(BigNum a, BigNum b);

BigNum expB(BigNum a, BigNum m, BigNum modN);

BigNum findR(BigNum modN);

BigNum modR(BigNum t, int r);

BigNum divR(BigNum t, int r);

BigNum monPro(BigNum a_bar, BigNum b_bar, int r, BigNum modN);

BigNum monPro2(BigNum a_bar, BigNum b_bar, int r, BigNum modN);

BigNum expM(BigNum m, BigNum e, BigNum modN);

bool isPrime(BigNum n);

BigNum genPrime();

BigNum invFermat(BigNum a, BigNum p);

BigNum nprimeExtGCD(BigNum a, BigNum p);

BigNum getBigNum(FILE *fp);

#endif
