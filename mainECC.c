/*//#include "ZEcc.h"
#include "ZMP.h"
#include "ZEcc.h"

int total = 10;	//	total sets needed

int main(int argc, char *argv[]){
	//srand(0);// for testing
	srand((unsigned)time(NULL));
	BigNum prime;
	BigNum a;
	BigNum b;
	Node P;
	BigNum k;
	Node Q;
	BigNum P_order;
	int i = 0;
	FILE *fp;
	char *line = NULL;
	size_t len = 0;

//	for(i = 0; i < total; i++){
//		while(true){
//			prime = genPrime();
//
//			// generate a stupid elliptic curve ...
//			BigNum _0 = int2BigNum(0, prime);
//			BigNum r = rdm2(_0, prime);	// generate r that 0 <= r < p
//			a = copyBigNum(r);	// stupid a = r, because we need r * b^2 = a^3 (mod p), just make a = r, b = r
//			b = copyBigNum(r);	// so now the curve is: y^2 = x^3 + ax + b (mod p) ...
//
//			// generate a stupid node
//			P = eccGenNode(a, b, prime);
//			k = rdm2(_0, prime);	// k as the private key
//			Q = eccMul(k, P, a, b, prime);	// Q = kP as the public key
//			while(Q.inf){
//				k = rdm2(_0, prime);
//				Q = eccMul(k, P, a, b, prime);
//			}
//
//			// find the P's order
//			BigNum _1 = int2BigNum(1, prime);
//			BigNum _51 = int2BigNum(51, prime);
//			P_order = copyBigNum(_1);
//			Node Pnow = copyNode(P);
//			while(!Pnow.inf){
//				if(compareBigNum(P_order, _51) == 0){
//					debugflag = false;
//				}
//				Pnow = eccAdd(Pnow, P, a, b, prime);
//				P_order = add_N(P_order, _1);
//			}
//
//			if(isPrime(P_order)){
//				break;
//			}
//		}
//
//		printf("i:\t%d\n", i);
//		printf("p:\n");
//		printBigNum(prime);
//		printf("a:\n");
//		printBigNum(a);
//		printf("b:\n");
//		printBigNum(b);
//		printf("P:\n");
//		printNode(P);
//		printf("k:\n");
//		printBigNum(k);
//		printf("Q:\n");
//		printNode(Q);
//		printf("\n");
//	}

	if(argc != 2){
		printf("Please run like this: ./cracker inputfile\n");
		exit(-1);
	}

	fp = fopen(argv[1], "r");
	// exhausting Method
	for(i = 0; i < total; i++){
		getline(&line, &len, fp);	// i:
		getline(&line, &len, fp);	// p:
		prime = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// a:
		a = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// b:
		b = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// P:
		getline(&line, &len, fp);	// x:
		P.inf = false;
		P.x = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// y:
		P.y = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// k:
		k = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// Q:
		getline(&line, &len, fp);	// x:
		Q.inf = false;
		Q.x = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// y:
		Q.y = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// P_order
		P_order = getBigNum(fp);
		getline(&line, &len, fp);	// \n
		getline(&line, &len, fp);	// \n

//		printf("i:\t%d\n", i);
//		printf("p:\n");
//		printBigNum(prime);
//		printf("a:\n");
//		printBigNum(a);
//		printf("b:\n");
//		printBigNum(b);
//		printf("P:\n");
//		printNode(P);
//		printf("k:\n");
//		printBigNum(k);
//		printf("Q:\n");
//		printNode(Q);
//		printf("P_order:\n");
//		printBigNum(P_order);
//		printf("\n");

		printf("\ni:\t%d\n", i);
		// bruteforce method
		BigNum k2 = bruteforce(P, Q, a, b, prime);
		printf("k2:\n");
		printBigNum(k2);
		printf("\n");

		// Pollard's Rho Method
		BigNum k3 = pollard(P, Q, a, b, prime, P_order);
		printf("k3:\n");
		printBigNum(k3);
	}

	return 0;
}
*/
