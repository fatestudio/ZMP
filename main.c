#include "ZMP.h"
#include "ZEcc.h"
#include <time.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
	srand(0);// for testing
//	srand((unsigned)time(NULL));
	BigNum prime;
	BigNum a;
	BigNum e;
	prime = genPrime();
	BigNum _0 = int2BigNum(0, prime);
	a = rdm2(_0, prime);	// generate r that 0 <= r < p
	e = rdm2(_0, prime);

	printf("m:\n");
	printBigNum(a);
	printf("e:\n");
	printBigNum(e);
	printf("n:\n");
	printBigNum(prime);

	Timeval start, end;
	long mtime, seconds, useconds;
	gettimeofday(&start, NULL);
	BigNum ans1 = expB(a, e, prime);
	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	printBigNum(ans1);
	printf("Elapsed time: %ld milliseconds\n", mtime);

	gettimeofday(&start, NULL);
	BigNum ans2 = expM(a, e, prime);
	gettimeofday(&end, NULL);

	printBigNum(ans2);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
	printf("Elapsed time: %ld milliseconds\n", mtime);

	return 0;
}
