#include "ZEcc.h"

NodeHT *ht = NULL;	/* important! initialize to NULL */

bool printNode(Node P){
	if(P.inf){
		printf("Node infinity\n");
	}
	else{
		printf("x:\n");
		printBigNum(P.x);
		printf("y:\n");
		printBigNum(P.y);
	}
	return true;
}


BigNum eccGetY2(BigNum x, BigNum a, BigNum b, BigNum p){	// get y2 = x^3+ax+b (mod p)
	BigNum ret;
	initBigNum(&ret);
	BigNum _3 = int2BigNum(3, p);
	BigNum x3 = expB(x, _3, p);
	BigNum ax = mul(a, x, p);
	BigNum x3_ax = add(x3, ax, p);

	ret = add(x3_ax, b, p);

	return ret;
}


bool eccHasY(BigNum y2, BigNum p){
	BigNum e = copyBigNum(p);
	BigNum _1 = int2BigNum(1, p);
	BigNum ret;

	e = sub(e, _1, p);
	e = shiftR(e);	//e = (p - 1) / 2

	ret = expB(y2, e, p);

	if(compareBigNum(_1, ret) == 0){
		return true;
	}

	return false;
}

// we only consider p == 3 mod 4 ...
BigNum eccGetY(BigNum y2, BigNum p){
	BigNum ret;
	initBigNum(&ret);

	BigNum e = copyBigNum(p);
	BigNum _1 = int2BigNum(1, p);
	e = add_N(p, _1);
	e = shiftR(e);
	e = shiftR(e);	// e = (p+1)/4 mod p

	ret = expB(y2, e, p);

	return ret;
}


Node eccGenNode(BigNum a, BigNum b, BigNum p){
	BigNum _0 = int2BigNum(0, p);
	BigNum x = rdm2(_0, p);
	BigNum y;
	while(true){
		BigNum y2 = eccGetY2(x, a, b, p);

		if(eccHasY(y2, p)){
			y = eccGetY(y2, p);
			break;
		}
		x = rdm2(_0, p);
	}

	Node ret;
	ret.inf = false;
	ret.x = x;
	ret.y = y;

	return ret;
}

// just ignore it ??	Actually we need schoof's and enumeration to settle down the threshold of k! \
// Now it is implemented in main()
// Now we just use [0, p) as the chosen set ...
BigNum eccGetNodeOrder(Node nodep, BigNum a, BigNum b, BigNum p){
	//for(i = 0; i < )
	BigNum ret;

	return ret;
}

bool initNode(Node *ret){
	memset(ret, 0, sizeof(Node));

	return true;
}

Node copyNode(Node n){
	Node ret;
	initNode(&ret);

	ret.inf = n.inf;
	ret.x = copyBigNum(n.x);
	ret.y = copyBigNum(n.y);

	return ret;
}

bool eqlNode(Node P1, Node P2){
	if(P1.inf && P2.inf){
		return true;
	}
	else if(!P1.inf && !P2.inf){
		if(compareBigNum(P1.x, P2.x) == 0 && compareBigNum(P1.y, P2.y) == 0){
			return true;
		}
	}
	return false;
}

Node int2Node_N(int x, int y){ 	// x, y >= 0
	BigNum X = int2BigNum_N(x);
	BigNum Y = int2BigNum_N(y);
	Node ret;
	initNode(&ret);
	ret.x = copyBigNum(X);
	ret.y = copyBigNum(Y);

	return ret;
}

Node eccAdd(Node P1, Node P2, BigNum a, BigNum b, BigNum p){
	Node ret;
	initNode(&ret);
	BigNum x1 = copyBigNum(P1.x);
	BigNum y1 = copyBigNum(P1.y);
	BigNum x2 = copyBigNum(P2.x);
	BigNum y2 = copyBigNum(P2.y);

	// for P1 = inf or P2 = inf
	if(P1.inf){
		ret = copyNode(P2);
	}
	else if(P2.inf){
		ret = copyNode(P1);
	}
	else if(eqlNode(P1, P2)){	// P1 == P2
		BigNum _3 = int2BigNum(3, p);
		BigNum _3x1_2 = mul(mul(_3, x1, p), x1, p);
		BigNum _3x2_a = add(_3x1_2, a, p);
		BigNum _2y1 = add(y1, y1, p);
		BigNum _0 = int2BigNum(0, p);
		if(compareBigNum(_0, _2y1) == 0){
			ret.inf = true;
			return ret;
		}

		BigNum m = mul(_3x2_a, invFermat(_2y1, p), p);
		BigNum x3 = mul(m, m, p);
		x3 = sub(x3, x1, p);
		x3 = sub(x3, x2, p);
		BigNum y3 = mul(m, sub(x1, x3, p), p);
		y3 = sub(y3, y1, p);

		ret.inf = false;
		ret.x = copyBigNum(x3);
		ret.y = copyBigNum(y3);
	}
	else{	// P1 != P2
		BigNum y2_y1 = sub(y2, y1, p);
		BigNum x2_x1 = sub(x2, x1, p);
		BigNum _0 = int2BigNum(0, p);
		if(compareBigNum(_0, x2_x1) == 0){
			ret.inf = true;
			return ret;
		}
		BigNum invx2_x1 = invFermat(x2_x1, p);
		BigNum m = mul(y2_y1, invx2_x1, p);
		BigNum x3 = mul(m, m, p);
		x3 = sub(x3, x1, p);
		x3 = sub(x3, x2, p);
		BigNum y3 = mul(m, sub(x1, x3, p), p);
		y3 = sub(y3, y1, p);

		ret.inf = false;
		ret.x = copyBigNum(x3);
		ret.y = copyBigNum(y3);
	}

	return ret;
}

Node eccMul(BigNum k, Node P, BigNum a, BigNum b, BigNum p){
	int i;
	Node ret;
	initNode(&ret);

	u_int32_t num = checkBit(k, k.bits - 1);

	if(num == 1){
		ret = copyNode(P);
	}	// else ret = (0, 0)

	for(i = k.bits - 2; i >= 0; i--){
		ret = eccAdd(ret, ret, a, b, p);

		// get e[i]
		num = checkBit(k, i);
		if(num == 1){
			ret = eccAdd(ret, P, a, b, p);
		}
	}

	return ret;
}

// single thread bruteforce
BigNum bruteforce(Node P, Node Q, BigNum a, BigNum b, BigNum p){
	BigNum _1 = int2BigNum(1, p);
	BigNum k = copyBigNum(_1);
	Node temp = copyNode(P);
	if(eqlNode(temp, Q)){
		return k;
	}
	while(compareBigNum(k, p) < 0){
		k = add(k, _1, p);
		temp = eccAdd(temp, P, a, b, p);
		if(eqlNode(temp, Q)){
			printf("Bruteforce Rounds:\n");
			printBigNum(k);
			return k;
		}
	}

	printf("Bruteforce Rounds:\n");
	printBigNum(k);
	return k;
}

NodeHT *findNodeHT(Node X){
	NodeHT l;
	memset(&l, 0, sizeof(NodeHT));
	l.X = copyNode(X);
	NodeHT *p;
	HASH_FIND(hh, ht, &l.X, sizeof(Node), p);
	return p;
}


bool addNodeHT(Node X_0, BigNum ap, BigNum bp){
	NodeHT *newr = (NodeHT*)malloc(sizeof(NodeHT));
	memset(newr, 0, sizeof(NodeHT));
	newr->X = copyNode(X_0);
	newr->ap = ap;
	newr->bp = bp;
	HASH_ADD(hh, ht, X, sizeof(Node), newr);

	return true;
}

bool printNodeHT(){
	NodeHT *s;
	int i = 0;
	printf("printNodeHT:\n");
	for(s = ht; s != NULL; s = s->hh.next){
		printf("i\t%d\n", i);
		printNode(s->X);
		printf("ap, bp:\n");
		printBigNum(s->ap);
		printBigNum(s->bp);
	}

	return true;
}

bool printNodeStruct(Node P){
	unsigned char *p = (unsigned char *)(&P);
	int n = sizeof(Node);
	int i;
	for(i = 0; i < n; i++){
		printf("%02x", p[i]);
	}
	printf("\n");

	return true;
}

// Actually this is not correct. Need to use schoof's to make the group prime first!
BigNum pollard(Node P, Node Q, BigNum a, BigNum b, BigNum p, BigNum P_order){	//P's order <= p
	BigNum ret;
	initBigNum(&ret);

	// generate random X_0 = ap*P + bp*Q
	BigNum _0 = int2BigNum(0, P_order);
	BigNum ap = rdm2(_0, P_order);
	BigNum bp = rdm2(_0, P_order);
	BigNum _1 = int2BigNum(1, p);
	BigNum _2 = int2BigNum(2, p);
	Node a_P = eccMul(ap, P, a, b, p);
	Node b_P = eccMul(bp, Q, a, b, p);
	Node X_0 = eccAdd(a_P, b_P, a, b, p);
	BigNum k = copyBigNum(_0);

	// insert X_0 and start the random walk
	addNodeHT(X_0, ap, bp);	// add the first Node
	unsigned int num_X;
	Node X_i = copyNode(X_0);
	num_X = HASH_COUNT(ht);
	while(true){
		k = add(k, _1, p);
		// split into 3 sets by x mod 3
		int t = rand_range(0, 2);	// this is not the same as the paper...
		if(t == 0){	// add P
			X_i = eccAdd(X_i, P, a, b, p);
			ap = add(ap, _1, P_order);
		}
		else if(t == 1){	// 2X_i
			X_i = eccMul( _2, X_i, a, b, p);
			ap = mul(ap, _2, P_order);
			bp = mul(bp, _2, P_order);
		}
		else if(t == 2){	// add Q
			X_i = eccAdd(X_i, Q, a, b, p);
			bp = add(bp, _1, P_order);
		}
		if(X_i.inf){
			printf("X_i hits infinity!\n");
			BigNum b2_b1 = sub(P_order, bp, P_order);
			BigNum inv_b2_b1 = invFermat(b2_b1, P_order);
			BigNum a1_a2 = copyBigNum(ap);
			ret = mul(a1_a2, inv_b2_b1, P_order);

			break;
		}

		NodeHT *r = findNodeHT(X_i);
		if(r){	// found a collision!
			if(compareBigNum(r->bp, bp) != 0){	// b1 != b2
				// d = (a1 - a2) / (b2 - b1);	a1 is r->ap
				BigNum b2_b1 = sub(bp, r->bp, P_order);
				BigNum inv_b2_b1 = invFermat(b2_b1, P_order);
				BigNum a1_a2 = sub(r->ap, ap, P_order);
				ret = mul(a1_a2, inv_b2_b1, P_order);
				break;
			}
		}
		else
			addNodeHT(X_i, ap, bp);

		num_X = HASH_COUNT(ht);
	}

	printf("Pollard Rounds:\n");
	printBigNum(k);

	return ret;
}
