// Qin ZHOU's Ecc library
#include "ZMP.h"
#include "uthash.h"
#ifndef ZECC_H_INCLUDED
#define ZECC_H_INCLUDED
typedef struct node{	// for elliptic curve
	bool inf;
	BigNum x;
	BigNum y;
} Node;

typedef struct hashtable{
	Node X;	// key
	BigNum ap;	// a * P
	BigNum bp;	// b * P
	UT_hash_handle hh;         /* makes this structure hashable */
} NodeHT;

bool printNode(Node P);

BigNum eccGetY2(BigNum x, BigNum a, BigNum b, BigNum p);

bool eccHasY(BigNum y2, BigNum p);

// we only consider p == 3 mod 4 ...
BigNum eccGetY(BigNum y2, BigNum p);

Node eccGenNode(BigNum a, BigNum b, BigNum p);

BigNum eccGetNodeOrder(Node nodep, BigNum a, BigNum b, BigNum p);

bool initNode(Node *ret);

Node copyNode(Node n);

bool eqlNode(Node P1, Node P2);

Node int2Node_N(int x, int y);

Node eccAdd(Node P1, Node P2, BigNum a, BigNum b, BigNum p);

Node eccMul(BigNum k, Node P, BigNum a, BigNum b, BigNum p);

BigNum bruteforce(Node P, Node Q, BigNum a, BigNum b, BigNum p);

NodeHT *findNodeHT(Node X);

bool addNodeHT(Node X_0, BigNum ap, BigNum bp);

bool printNodeHT();

bool printNodeStruct(Node P);

// Actually this is not correct. Need to use schoof's to make the group prime first!
BigNum pollard(Node P, Node Q, BigNum a, BigNum b, BigNum p, BigNum P_order);

#endif
