/*
 * rsa.c
 *
 *  Created on: Apr 4, 2013
 *      Author: jason
 */
// Qin ZHOU's Multiple Precision Library
#include "ZMP.h"
#include <time.h>
#include <sys/types.h>

//#define MAXN 128
//#define MAXL MAXN/32
//#define NUM 10 //54  // number of bits of our desired prime limit
//#define RANGE 256
//#define MAX_U32 0xFFFFFFFF

bool debugflag = false;
bool debugflag2 = false;

int rand_range(int min_n, int max_n)
{
    return rand() % (max_n - min_n + 1) + min_n;
}

bool printBigNum(BigNum n){
	int i;

	printf("0x", n.bits);
	for(i = (n.bits - 1)/ 32; i >= 0; i--){
		printf("%08x", n.num[i]);
	}
	printf("\n");
	return true;
}

bool printBigNum2(BigNum n){
	int i;

	printf("%d\t", n.bits);
	for(i = 0; i < (n.bits - 1)/ 32 + 1; i++){
		printf("%08x ", n.num[i]);
	}
	printf("\n");
	return true;
}

u_int32_t checkBit(BigNum a, int bit){
	u_int32_t num = a.num[bit / 32];
	num = (num >> (bit % 32)) % 2;

	return num;
}

int compareBigNum(BigNum bn1, BigNum bn2){
	int posflag = 0;	// 0: unset or equal; k: bn1[k] > bn2[k]; -k: bn1[k] < bn2[k] highest different bit
	int i;

	if(bn1.bits > bn2.bits){
    	posflag = bn1.bits;
    }
    else if(bn1.bits < bn2.bits){
        posflag = -bn2.bits;
    }
	else{	// bn1.bits == bn2.bits
		for(i = bn1.bits - 1; i >= 0; i--){
			u_int32_t b1 = checkBit(bn1, i);
			u_int32_t b2 = checkBit(bn2, i);
			if(b1 > b2){
				posflag = i+1;
				break;
			}
			else if(b1 < b2){
				posflag = -i-1;
				break;
			}
		}
	}

	return posflag;
}

bool initBigNum(BigNum *ret){
	memset(ret, 0, sizeof(BigNum));
	ret->bits = 1;

	return true;
}

BigNum copyBigNum(BigNum n){
	BigNum ret;
	initBigNum(&ret);

	int i;
	for(i = 0; i < (n.bits - 1)/ 32 + 1; i++){
		ret.num[i] = n.num[i];
	}
	ret.bits = n.bits;

	return ret;
}

BigNum copyBigNum2(BigNum n, int s_index, int range){
	int i;
	BigNum ret;
	initBigNum(&ret);
	for(i = 0; i < range; i++){
		u_int32_t num = checkBit(n, i + s_index);
		if(i % 32 == 0){
			ret.num[i / 32] = ret.num[i / 32] ^ num;
		}
		else
			ret.num[i / 32] = ret.num[i / 32] ^ (num << (i % 32));
	}
	ret.bits = range;
	return ret;
}

BigNum rdm(int bits){
    BigNum ret;
    initBigNum(&ret);
	int i;

    // random a byte every time and combine to get bits value
	for(i = 0; i < bits; i++){
			if((i % 32 == 0) && (i / 32 == (bits - 1) / 32)){ // must guarantee the highest bit is 1
				ret.num[i / 32] = 1;
			}
			else{
                ret.num[i / 32] = ret.num[i / 32] * 2 ^ (u_int32_t)rand_range(0, 1);
			}
	}

	ret.bits = bits;

    return ret;
}

BigNum rdm2(BigNum minN, BigNum maxN){ // minN <= ret < maxN
	BigNum ret;
	int i;
	while(true){	// generate that bits of random number, then check whether within this range, if not, regenerate
		initBigNum(&ret);
		int maxBits = 0;
		for(i = 0; i < maxN.bits; i++){
			int ran = rand_range(0, 1);
			if((i / 32 == (maxN.bits - 1) / 32) && ran > 0 && maxBits == 0){
				maxBits = maxN.bits - i + (i / 32) * 32;
			}
			ret.num[i / 32] = (ret.num[i / 32] << 1) ^ (u_int32_t)ran;
		}
		ret.bits = maxBits;

		if((compareBigNum(ret, minN) >= 0) && (compareBigNum(ret, maxN) < 0)){
			return ret;
		}
	}
	return ret;
}

int countIntBits(int num){
	int count = 0;
	if(num == 0){
		return 1;
	}
	while(num > 0){
		num = num / 2;
		count++;
	}
	return count;
}

BigNum sub(BigNum bn1, BigNum bn2, BigNum modN){ // substraction
	int i, j;
	int posflag = compareBigNum(bn1, bn2);
	BigNum ret;
	initBigNum(&ret);

	if(posflag >= 0){ // ret is positive
		for(i = bn1.bits / 32; i >= 0; i--){
			if(bn1.num[i] >= bn2.num[i]){
				ret.num[i] = bn1.num[i] - bn2.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - bn2.num[i] + 1 + bn1.num[i]; // +1 is awesome!
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}
		}
		// set the ret.bits
		for(i = bn1.bits - 1; i >= 0; i--){
			u_int32_t num = checkBit(ret, i);
			if(num > 0){
				ret.bits = i + 1;
				break;
			}
		}
	}
	else{	// ret is negative, add modN
		for(i = bn2.bits / 32; i >= 0; i--){
			if(bn2.num[i] >= bn1.num[i]){
				ret.num[i] = bn2.num[i] - bn1.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - bn1.num[i] + 1 + bn2.num[i];
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}
		}
		for(i = modN.bits / 32; i >= 0; i--){
			if(modN.num[i] >= ret.num[i]){
				ret.num[i] = modN.num[i] - ret.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - ret.num[i] + 1 + modN.num[i];
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}
		}
		// set the ret bits
		for(i = modN.bits - 1; i >= 0; i--){
			u_int32_t num = checkBit(ret, i);
			if(num > 0){
				ret.bits = i + 1;
				break;
			}
		}
	}
	return ret;
}

BigNum sub_N(BigNum bn1, BigNum bn2){ // substraction without modN: we guarantee that bn1 >= bn2, otherwise wrong!
	int i, j;
	int posflag = compareBigNum(bn1, bn2);
	BigNum ret;
	initBigNum(&ret);

	if(posflag >= 0){ // ret is positive
		for(i = bn1.bits / 32; i >= 0; i--){
			if(bn1.num[i] >= bn2.num[i]){
				ret.num[i] = bn1.num[i] - bn2.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - bn2.num[i] + 1 + bn1.num[i]; // +1 is awesome!
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}
		}
		// set the ret.bits
		for(i = bn1.bits - 1; i >= 0; i--){
			u_int32_t num = checkBit(ret, i);
			if(num > 0){
				ret.bits = i + 1;
				break;
			}
		}
	}
	else{
		printf("ERROR in sub_N: bn1 < bn2!\n");
		exit(0);
	}

	return ret;
}

BigNum int2BigNum(int num, BigNum modN){ // integer to BigNum type
	BigNum ret;
	initBigNum(&ret);

	if(num >= 0){
		ret.num[0] = num;
		ret.bits = countIntBits(num);
	}
	else if(num < 0){
		ret.num[0] = -num;
		ret.bits = countIntBits(-num);
		ret = sub(modN, ret, modN);
	}

	return ret;
}

BigNum int2BigNum_N(int num){	// num must >= 0
	BigNum ret;
	initBigNum(&ret);

	ret.num[0] = num;
	ret.bits = countIntBits(num);

	return ret;
}

bool even(BigNum m){
	if(m.num[0] % 2 == 0)
		return true;
	else
		return false;
}

BigNum shiftR(BigNum m){	// divide 2
	int i;
	BigNum ret = copyBigNum(m);

	for(i = 0; i < (ret.bits - 1) / 32 + 1; i++){
		ret.num[i] = ret.num[i] >> 1;
		if((i + 1) < (ret.bits - 1)/ 32 + 1){
			ret.num[i] = ret.num[i] ^ ((ret.num[i + 1] % 2) << 31);
		}
	}
	ret.bits--;

	return ret;
}

BigNum shiftL_N(BigNum a, int d){	// shift left without modN
	int i;
	BigNum ret = copyBigNum(a);
	BigNum _0;
	initBigNum(&_0);
	int gap1 = d / 32;
	if(compareBigNum(a, _0) != 0){
		if(gap1 > 0){
			for(i = (a.bits - 1) / 32; i >= 0; i--){
				ret.num[i + gap1] = ret.num[i];
			}
			for(i = 0; i < gap1; i++){
				ret.num[i] = 0;
			}
			ret.bits = ret.bits + gap1 * 32;
		}

		int gap2 = d % 32;
		for(i = (ret.bits - 1) / 32; i >= 0; i--){
			if(gap2 > 0){	// this is a bug and I don't know why ...  >> 32 will be the same
				ret.num[i + 1] = ret.num[i + 1] ^ (ret.num[i] >> (32 - gap2));
				ret.num[i] = ret.num[i] << gap2;
			}
		}

		ret.bits = ret.bits + gap2;
	}

	return ret;
}

BigNum shiftL(BigNum a, int d, BigNum modN){	// shift left with modN, with errors!
	int i;
	BigNum ret = copyBigNum(a);

	int gap1 = d / 32;
	if(gap1 > 0){
		for(i = (a.bits - 1) / 32; i >= 0; i--){
			ret.num[i + gap1] = ret.num[i];
		}
		for(i = 0; i < gap1; i++){
			ret.num[i] = 0;
		}
		ret.bits = ret.bits + gap1 * 32;
	}

	int gap2 = d % 32;
	for(i = (ret.bits - 1) / 32; i >= 0; i--){
		ret.num[i + 1] = ret.num[i + 1] ^ (ret.num[i] >> (32 - gap2));
		ret.num[i] = ret.num[i] << gap2;
	}
	ret.bits = ret.bits + gap2;

	// clear the ret.num[(modN.bits - 1) / 32]
	ret.num[(modN.bits - 1) / 32] = ret.num[(modN.bits - 1) / 32] << ((32 - modN.bits) % 32) >> ((32 - modN.bits) % 32);
	if(a.bits + d > modN.bits){
		ret.bits = modN.bits;
	}
	else
		ret.bits = a.bits + d;

	// may be ret > modN
	int posflag = compareBigNum(ret, modN);
	if(posflag >= 0){
		ret = sub(ret, modN, modN);
	}
	return ret;
}

BigNum add_N(BigNum a, BigNum b){	// add without modN		!!!!!! bitwise....
	int i;
	u_int32_t ans;
	BigNum ret;
	initBigNum(&ret);
	int max;
	if(a.bits >= b.bits){
		max = a.bits;
		ret.bits = max;
	}
	else{
		max = b.bits;
		ret.bits = max;
	}
	u_int32_t prev = 0;

	for(i = 0; i < max; i++){
		ans = prev;
		if(i < a.bits){
			ans = ans + checkBit(a, i);
		}
		if(i < b.bits){
			ans = ans + checkBit(b, i);
		}
		if(ans > 1){
			ans = ans - (u_int64_t)2;
			if((i % 32) != 0)
				ret.num[i / 32] = ret.num[i / 32] ^ (ans << (i % 32));
			else
				ret.num[i / 32] = ans;

			prev = 1;
		}
		else{
			if((i % 32) != 0)
				ret.num[i / 32] = ret.num[i / 32] ^ (ans << (i % 32));
			else
				ret.num[i / 32] = ans;

			prev = 0;
		}
	}

	if(prev == 1){
		if(max % 32 == 0)
			ret.num[max / 32] = (u_int32_t)1;
		else
			ret.num[max / 32] = ((u_int32_t)1 << (max % 32)) ^ ret.num[max / 32];
		ret.bits++;
	}

	return ret;
}

// this is the correct one!!	not correct still...
BigNum add_N2(BigNum a, BigNum b){	// add without modN		!!!!!! word wise!!!!!!
	int i;
	BigNum ret;
	initBigNum(&ret);
	int max;
	if(a.bits >= b.bits){
		max = a.bits;
		ret.bits = max;
	}
	else{
		max = b.bits;
		ret.bits = max;
	}
	u_int64_t prev = 0;
	u_int64_t max_u32 = 0x100000000;
	for(i = 0; i < (max - 1) / 32 + 1; i++){	//
		u_int64_t tnow = (u_int64_t)a.num[i] + (u_int64_t)b.num[i] + (u_int64_t)prev;
		if(tnow >= max_u32){
			prev = 1;
			tnow = tnow - max_u32;
		}
		else{
			prev = 0;
		}
		ret.num[i] = tnow;
	}

	if(prev == 1){
		ret.num[i] = 1;
	}

	for(i = max + 2; i >= 0; i--){
		if(checkBit(ret, i) == 1){
			ret.bits = i + 1;
			break;
		}
	}

	return ret;
}

BigNum add(BigNum a, BigNum b, BigNum modN){
	BigNum ret = add_N2(a, b);

	int posflag = compareBigNum(ret, modN);
	if(posflag >= 0){
		ret = sub(ret, modN, modN);
	}

	return ret;
}

bool checkBitsBigNum(BigNum n){
	int i;
	for(i = n.bits + 32; i >= n.bits - 1; i--){
		if(checkBit(n, i) > 0 && i != n.bits - 1){
			printf("checkBitsBigNum i: %d\n", i);
			return false;
		}
	}
	return true;
}

BigNum mul(BigNum a, BigNum b, BigNum modN){
	// let me count how many additions and subtractions
	int additions = 0;
	int subtractions = 0;
	int i;
	u_int32_t num;
	BigNum temp;
	BigNum ret;
	initBigNum(&ret);

	for(i = 0; i < a.bits; i++){ // enumerate all bits of number a
		num = checkBit(a, i);
		if(num == 1){
			initBigNum(&temp);
			temp = shiftL_N(b, i);
			ret = add_N2(ret, temp);
		}
	}

	if(compareBigNum(ret, modN) >= 0){
		// get the remainder of modN
		int s_index = ret.bits - modN.bits;	// index now
		BigNum dividend = copyBigNum2(ret, s_index, modN.bits);
		while(s_index >= 0){
			while(compareBigNum(dividend, modN) < 0 && s_index > 0){
				// dividend < modN, means we need 1 more bit of ret
				dividend = shiftL_N(dividend, 1);
				s_index--;
				dividend.num[0] = dividend.num[0] ^ checkBit(ret, s_index);
			}

			if(compareBigNum(dividend, modN) >= 0){
				dividend = sub(dividend, modN, modN); // use sub is fine, don't need sub_N
			}
			else
				break;	// don't need operation
		}
		ret = copyBigNum(dividend);
	}

	return ret;
}

BigNum mul_N(BigNum a, BigNum b){
	int i;
	u_int32_t num;
	BigNum temp;
	BigNum ret;
	initBigNum(&ret);

	for(i = 0; i < a.bits; i++){ // enumerate all bits of number a
		num = checkBit(a, i);
		if(num == 1){
			initBigNum(&temp);
			temp = shiftL_N(b, i);
			ret = add_N2(ret, temp);
		}
	}

	return ret;
}

BigNum2 divAndMod(BigNum a, BigNum b){
	BigNum2 ret2;
	initBigNum(&ret2.quotient);
	initBigNum(&ret2.remainder);

	if(compareBigNum(a, b) < 0){
		ret2.quotient = int2BigNum_N(0);
		ret2.remainder = copyBigNum(a);
	}
	else{
		// get the remainder of modN
		int s_index = a.bits - b.bits;	// index now
		BigNum dividend = copyBigNum2(a, s_index, b.bits);
		while(s_index >= 0){
			while(compareBigNum(dividend, b) < 0 && s_index > 0){
				// dividend < modN, means we need 1 more bit of ret
				dividend = shiftL_N(dividend, 1);
				s_index--;
				dividend.num[0] = dividend.num[0] ^ checkBit(a, s_index);
				ret2.quotient = shiftL_N(ret2.quotient, 1);
			}

			if(compareBigNum(dividend, b) >= 0){
				dividend = sub(dividend, b, b); // use sub is fine, don't need sub_N
				ret2.quotient.num[0] = ret2.quotient.num[0] ^ 1;
			}
			else
				break;	// don't need operation
		}
		ret2.remainder = copyBigNum(dividend);
	}

	return ret2;
}

BigNum expB(BigNum a, BigNum m, BigNum modN){ // follow the algrithm on page 14/17 of ~koc/~cs178/docx/w00a.pdf
	int i;
	BigNum ret;
	initBigNum(&ret);

	u_int32_t num = checkBit(m, m.bits - 1);

	if(num == 1){
		ret = copyBigNum(a);
		if(debugflag){
			printBigNum(ret);
		}
	}
	else{
		ret.num[0] = 1;
		ret.bits = 1;
	}

	for(i = m.bits - 2; i >= 0; i--){
		ret = mul(ret, ret, modN);
		if(debugflag){
			printBigNum(ret);
		}
		// get e[i]
		num = checkBit(m, i);
		if(num == 1){
			ret = mul(ret, a, modN);
			if(debugflag){
				printBigNum(ret);
			}
		}
	}

	return ret;
}

BigNum n_prime;

BigNum findR(BigNum modN){
	BigNum ret = int2BigNum_N(1);

	while(compareBigNum(ret, modN) < 0){ // we need ret > modN
		ret = shiftL_N(ret, 1);
	}

	return ret;
}

BigNum modR(BigNum t, int r){ // r is R=2^r
	BigNum ret;
	int blocks = r / 32;
	int remains = r - 32 * blocks;
	int i;
	initBigNum(&ret);
	for(i = 0; i < blocks; i++){	// copy the whole block
		ret.num[i] = t.num[i];
	}
	ret.num[blocks] = ((t.num[blocks] >> remains) << remains) ^ t.num[blocks];

	for(i = r - 1; i >= 0; i--){	// start top bit
		int bit = checkBit(ret, i);
		if(bit != 0){
			ret.bits = i + 1;	// because this is the total number of bits
			break;
		}
	}

	return ret;
}

BigNum divR(BigNum t, int r){
	BigNum ret;
	initBigNum(&ret);
	int blocks = r / 32;
	int remains = r - 32 * blocks;
	int _32_remains = 32 - remains;
	int i;
	for(i = 0; i < (t.bits - r) / 32 + 1; i++){
		ret.num[i] = t.num[i + blocks] >> remains;
		if((i + blocks + 1) <= (t.bits - 1) / 32 + 1){
			if(_32_remains != 32){	// shift 32 = 0, ret.num[i] ^ 0 = ret.num[i]
				u_int32_t tshift = (t.num[i + blocks + 1] >> _32_remains);

				ret.num[i] = ret.num[i] ^ tshift;
			}
		}
	}
	ret.bits = t.bits - r;
	if(ret.bits < 0)
		ret.bits = 0;

	return ret;
}

BigNum monPro(BigNum a_bar, BigNum b_bar, int r, BigNum modN){
	BigNum ret;
	initBigNum(&ret);

	BigNum t = mul_N(a_bar, b_bar);
	BigNum m = mul_N(t, n_prime);
	BigNum m1 = modR(m, r);
	BigNum m2 = mul_N(m1, modN);
	BigNum m3 = add_N2(t, m2);
	BigNum u = divR(m3, r);

	if(compareBigNum(u, modN) >= 0){
		ret = sub_N(u, modN);
	}
	else{
		ret = copyBigNum(u);
	}

	return ret;
}

BigNum monPro2(BigNum a_bar, BigNum b_bar, int r, BigNum modN){
	BigNum ret;
	int i;
	initBigNum(&ret);

	for(i = 0; i < a_bar.bits; i++){
		if(checkBit(a_bar, i) == 1){
			ret = add_N2(ret, b_bar);
		}
		if(ret.num[0] & 1 == 1){
			ret = add_N2(ret, modN);
		}
		ret = shiftR(ret);
	}
	for(i = 0; i < r - a_bar.bits; i++){
		if(ret.num[0] & 1 == 1){
			ret = add_N2(ret, modN);
		}
		ret = shiftR(ret);
	}

	while(compareBigNum(ret, modN) >= 0){
		ret = sub_N(ret, modN);
	}

	return ret;
}

BigNum expM(BigNum m, BigNum e, BigNum modN){	// Montgomery method
	int i;
	BigNum ret;
	initBigNum(&ret);
	Timeval start, end;
	long mtime, seconds, useconds;

	initBigNum(&n_prime);
	BigNum R = findR(modN);
	n_prime = nprimeExtGCD(modN, R);

	BigNum m_bar = mul(m, R, modN);
	BigNum _1 = int2BigNum(1, modN);
	BigNum x_bar = mul(_1, R, modN);

	u_int32_t num;
	for(i = e.bits - 1; i >= 0; i--){
		x_bar = monPro2(x_bar, x_bar, R.bits - 1, modN);

		// get e[i]
		num = checkBit(e, i);
		if(num == 1){
			x_bar = monPro2(m_bar, x_bar, R.bits - 1, modN);
		}
	}
	ret = monPro2(x_bar, _1, R.bits - 1, modN);

	return ret;
}

// Miller-Rabin Primality Test
bool isPrime(BigNum n){
	printBigNum(n);
	int i;
	BigNum n_minus1 = copyBigNum(n);       // n - 1

	// generate n - 1
	i = 0;
	while(n_minus1.num[i] == 0){
		n_minus1.num[i] = MAX_U32;
		i++;
	}
	n_minus1.num[i]--;

	BigNum m = copyBigNum(n_minus1);
	int k = 0;
	while(even(m)){ // get odd m
		m = shiftR(m);
		k++;
	}

	BigNum _2 = int2BigNum(2, n);
	BigNum a = rdm2(_2, n_minus1);    // random a, for a^m
	BigNum b;
//	BigNum bb;
	b = expM(a, m, n);  // b_0 = a^m
//	b = expB(a, m, n);
//	if(compareBigNum(bb, b) != 0){
//		printf("a:\n");
//		printBigNum(a);
//		printf("m:\n");
//		printBigNum(m);
//		printf("n:\n");
//		printBigNum(n);
//		printf("expM:\n");
//		printBigNum(bb);
//		printf("expB:\n");
//		printBigNum(b);
//		exit(0);
//	}
	BigNum _1 = int2BigNum(1, n);
	BigNum _minus1 = int2BigNum(-1, n);

	if(compareBigNum(b, _1) == 0 || compareBigNum(b, _minus1) == 0){
			return true;
	}
	for(i = 1; i < k; i++){
			BigNum tempb = copyBigNum(b);
			b = expM(b, _2, n);      // b_i = b_{i-1}^2
//			b = expB(b, _2, n);
//			if(compareBigNum(bb, b) != 0){
//				printf("b:\n");
//				printBigNum(tempb);
//				printf("2:\n");
//				printf("n:\n");
//				printBigNum(n);
//				printf("expM:\n");
//				printBigNum(bb);
//				printf("expB:\n");
//				printBigNum(b);
//				exit(0);
//			}

			if(compareBigNum(b, _1) == 0){
					return false;
			}
			else if(compareBigNum(b, _minus1) == 0){
					return true;
			}
	}

	return false;
}


BigNum genPrime(){      // generate Prime p
	int times = 0;
	BigNum prime = rdm(NUM);    // generate a random number as prime (?Improvement: odd it...)
	times++;
	while(true){
		while(even(prime)){
			 prime = rdm(NUM);
			 times++;
		}
//		if(isPrime(prime) && prime.num[0] % 4 == 3){	// need the prime == 3 (mod 4), otherwise it is hard to compute...
//			printf("genPrime's trials: %d\n", times);
//			return prime;
//		}
//		else{
//			prime = rdm(NUM);
//			times++;
//		}
		break;
	}
	printf("genPrime's trials: %d\n", times);
	return prime;
}

BigNum invFermat(BigNum a, BigNum p){	// using fermat's theorem to get the inverse of a!
	BigNum ret;
	initBigNum(&ret);
	BigNum _2 = int2BigNum(2, p);
	BigNum p_2 = sub(p, _2, p);
	ret = expB(a, p_2, p);	// a^(p-2) = inv(a)

	return ret;
}

typedef struct extret{
	BigNum inva;
	int inva_sign;
	BigNum invb;
	int invb_sign;
} ExtRet;

ExtRet extGCD(BigNum a, BigNum b){
	ExtRet ret;

	BigNum x = int2BigNum_N(0);	// x, lastx, y, lasty, all need signs!
	int x_sign = 1;
	BigNum lastx = int2BigNum_N(1);
	int lastx_sign = 1;
	BigNum y = int2BigNum_N(1);
	int y_sign = 1;
	BigNum lasty = int2BigNum_N(0);
	int lasty_sign = 1;

	BigNum _0 = int2BigNum_N(0);
	BigNum2 qr;
	BigNum temp1;
	int temp1_sign = 0;
	BigNum temp2;
	int temp2_sign = 0;

	while(compareBigNum(_0, b) != 0){
		qr = divAndMod(a, b);
		temp1 = copyBigNum(b);
		temp2 = copyBigNum(qr.remainder);
		a = copyBigNum(temp1);
		b = copyBigNum(temp2);

		temp1 = mul_N(qr.quotient, x);
		if(x_sign == 1 && lastx_sign == -1){
			temp1 = add_N2(lastx, temp1);
			temp1_sign = -1;
		}
		else if(x_sign == -1 && lastx_sign == 1){
			temp1 = add_N2(lastx, temp1);
			temp1_sign = 1;
		}
		else if(x_sign == 1 && lastx_sign == 1){
			if(compareBigNum(lastx, temp1) >= 0){
				temp1 = sub_N(lastx, temp1);
				temp1_sign = 1;
			}
			else{
				temp1 = sub_N(temp1, lastx);
				temp1_sign = -1;
			}
		}
		else if(x_sign == -1 && lastx_sign == -1){
			if(compareBigNum(lastx, temp1) >= 0){
				temp1 = sub_N(lastx, temp1);
				temp1_sign = -1;
			}
			else{
				temp1 = sub_N(temp1, lastx);
				temp1_sign = 1;
			}
		}

		temp2 = mul_N(qr.quotient, y);
		if(y_sign == 1 && lasty_sign == -1){
			temp2 = add_N2(lasty, temp2);
			temp2_sign = -1;
		}
		else if(y_sign == -1 && lasty_sign == 1){
			temp2 = add_N2(lasty, temp2);
			temp2_sign = 1;
		}
		else if(y_sign == 1 && lasty_sign == 1){
			if(compareBigNum(lasty, temp2) >= 0){
				temp2 = sub_N(lasty, temp2);
				temp2_sign = 1;
			}
			else{
				temp2 = sub_N(temp2, lasty);
				temp2_sign = -1;
			}
		}
		else if(y_sign == -1 && lasty_sign == -1){
			if(compareBigNum(lasty, temp2) >= 0){
				temp2 = sub_N(lasty, temp2);
				temp2_sign = -1;
			}
			else{
				temp2 = sub_N(temp2, lasty);
				temp2_sign = 1;
			}
		}

		lastx = copyBigNum(x);
		lastx_sign = x_sign;
		lasty = copyBigNum(y);
		lasty_sign = y_sign;
		x = copyBigNum(temp1);
		x_sign = temp1_sign;
		y = copyBigNum(temp2);
		y_sign = temp2_sign;
	}

	ret.inva = copyBigNum(lastx);
	ret.inva_sign = lastx_sign;
	ret.invb = copyBigNum(lasty);
	ret.invb_sign = lasty_sign;

	return ret;
}

BigNum nprimeExtGCD(BigNum a, BigNum b){	// using Extend GCD, remember it is nprime!
	ExtRet extret = extGCD(a, b);
	if(extret.inva_sign == 1){
		return sub_N(b, extret.inva);
	}
	else{
		return extret.inva;
	}
}

BigNum getBigNum(FILE *fp){
	BigNum ret;
	initBigNum(&ret);
	int i;
	char c = fgetc(fp);
	int bits = 0;	// how many bits ...
	int j = 1;	// j = 10 ^ ..
	while(c != '\t'){	// get bits
		bits = bits * 10 + (c - '0');
		c = fgetc(fp);
	}
	ret.bits = bits;

	int seg = (bits - 1)/ 32 + 1;
	for(i = 0; i < seg; i++){
		for(j = 0; j < 8; j++){	// 8 bytes
			c = fgetc(fp);
			int k = 0;
			if(c >= '0' && c <= '9')
				k = c - '0';
			else if(c >= 'a' && c <= 'f')
				k = c - 'a' + 10;

			ret.num[i] = ret.num[i] * 16 + k;
		}
	}

	return ret;
}



