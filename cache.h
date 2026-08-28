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

class MainMemory {
public:
	int read(size_t addr);

public:	//@@Just being lazy for now. Make it private later
	map<size_t, int> contents;	//Map of (addr, data)
};


enum class Write_Policy {
  WRITE_BACK,
  WRITE_THRU,
  INVALID
};

typedef struct cache_entry {
	size_t addr;
	int data;
	bool valid;		//False means there is no data here
	bool dirty;		//Relevant for write-thru policy
	bool modified_in_curr_access;
	size_t timestamp;	//For LRU replacement policy
} t_cache_entry;

class Cache {
public:
  Cache(size_t s);
  int findIndexOfAddr(size_t addr);
  int findFreeSlotIndex();
  void insertAt(int free_slot, const t_cache_entry& cache_entry);
  size_t findIndexOfLruEntry();
  int read(size_t addr, Write_Policy wp);
  void write(size_t addr, int data, Write_Policy wp);
  void setLowerLevelCache(Cache *c, MainMemory *mm);
  void setUpperLevelCache(Cache *c, MainMemory *mm);
  void printContents(bool printNewline = true);
  
private:
  vector<t_cache_entry> contents;	//Contents of the cache after the current access pattern.
  Cache *upper = NULL;
  Cache *lower = NULL;
  MainMemory *mm = NULL;
  int curr_max_timestamp = -1;

  int getDataAtIndex(size_t index);
};


namespace snucs {

}  //End namespace snucs

#endif

