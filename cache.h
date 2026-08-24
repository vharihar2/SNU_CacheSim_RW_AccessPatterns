//
//$Id$
//
#ifndef SNUCS_CACHE
#define SNUCS_CACHE

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string>

using namespace std;

#define SNUCS_MAX_CACHE_LEVELS 3  //L1, L2, L3 (excluding main memory)

enum class Write_Policy {
  WRITE_BACK,
  WRITE_THRU,
  INVALID
};

typedef struct cache_entry {
  size_t addr;
  int data;
  bool dirty;		//Relevant for write-thru policy
  bool modified_in_curr_access;
  size_t timestamp;	//For LRU replacement policy
} t_cache_entry;

class Cache {
public:
  Cache(size_t s) : size(s) {}
  inline int findIndexOfAddr(size_t addr);
  inline int findFreeSlotIndex();
  inline void insertAt(int free_slot, const t_cache_entry& cache_entry);
  inline size_t findIndexOfLruEntry();
  int read(size_t addr) {/*@@*/ printf("Not implemented yet!\n"); exit(1); return -99999; };
  void write(size_t addr, int data, Write_Policy wp);
  void setLowerLevelCache(Cache* c) {/*@@*/};
  void setUpperLevelCache(Cache *c) {/*@@*/ };
  
private:
  vector<t_cache_entry> contents;	//Contents of the cache after the current access pattern.
  size_t size;  //Max entries allowed in the vector above
  Cache *upper = NULL;
  Cache *lower = NULL;
  int curr_max_timestamp = -1;
};


namespace snucs {

}  //End namespace snucs

#endif

