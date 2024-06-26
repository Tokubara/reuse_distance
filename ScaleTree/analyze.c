//#define CACHELINE 5 /* 32 byte */
//#define NULL 0
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <algorithm>
#include <queue>
#include <sstream>
#include <fstream>
#include <time.h>

#include<iostream>
#include<fstream>

#include "hash.cxx"
#define BUF_NUM 1
using namespace std;
typedef struct tree_node Tree;
struct tree_node {
    Tree * left, * right;
    unsigned long long item;    /* last access time of this node */
    unsigned nodeWt;  /* weight of the node */
    unsigned weight;  /* weight of the entire subtree, including this node */
    unsigned maxSize;    /* maximal size of the node */
    Tree *prev, *next; /* nodes that are next to this node in the sorted order */
};

Tree * trace = NULL;              /* the scale-tree trace  */
float errorRate = 0.001;              /* 0.1 means 90% accurate */

unsigned numData;
unsigned long long curCycle = 0;

#include "scaleTree.c"
#include "counter.cxx"

unsigned power=1;

int _DataAccess(unsigned long addr) {
  //cout<<addr<<endl;
  unsigned long long lastAccCyc;
  unsigned dis = 0;
  //int i;

/*  addr = addr >> CACHELINE;*/

  lastAccCyc = HashSearchUpdate(addr, curCycle);
  
  trace = QueryScaleTree(lastAccCyc, curCycle, trace, &dis);
  if (lastAccCyc == curCycle) { /* a new element */
    numData ++;
    if (numData - ((numData >> 10)<<10)==0) {
      power=1;
      unsigned tmp = numData;
      while (tmp>0) {
	tmp = tmp >> 6;
	power ++;
      }
    }
  }
  else
    RecordDistance(dis,addr);

  if (sizeTrace > 10*10 * 100 * power) {
    unsigned oldSize = sizeTrace;
    trace = CompactScaleTree(trace);
    //printf("compact the scale tree from size %u to %u \n",oldSize, sizeTrace);
    assert(sizeTrace <= oldSize/2);
  }

  curCycle ++;
  return dis;
}

void PrintSize(){
  printf("hash allocated %u,  hash table size %u; scaletree size %u.\n",Hashallocated, numData,sizeTrace);
}
int main(int argc, char *argv[]) {
	HashInitialize();
	CounterInitialize();
  if (argc < 2) {
    perror("wrong argument number");
    return 1;
  }

  FILE *fp = fopen(argv[1], "rb");
  if (!fp) {
    perror("Can't open");
    return 1;
  }
  unsigned long buf[BUF_NUM];
  int n;
  long long counter = 0;
  while((n=fread(buf, sizeof(long long), BUF_NUM, fp))>0) {
    for (int i = 0; i < n; i++) {
          _DataAccess(buf[i]);
    }
    counter++;
    if (counter % 100000000 == 0) {
      printf("count=%lld\n", counter);
    }
  }
  fclose(fp);

  _PrintResults((argc > 2)? argv[2] : nullptr);
  return 0;
}
