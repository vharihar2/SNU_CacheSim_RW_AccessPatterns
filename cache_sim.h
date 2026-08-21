//
//$Id: cache_sim.h,v 1.3 2026/08/19 13:03:55 venkatnarayan.h Exp venkatnarayan.h $
//
#ifndef SNUCS_CACHE_SIM
#define SNUCS_CACHE_SIM

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <lexical_cast.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "vhutil_config_unique_keys.h"
#include "cache_sim.h"

using namespace std;

#define SNUCS_MAX_CACHE_LEVELS 3  //L1, L2, L3 (excluding main memory)

enum class Write_Policy {
  WRITE_BACK,
  WRITE_THRU,
  INVALID
};

typedef struct t_parameters {
  //Input parameters:
  size_t NumCacheLevelsExclMM = 0;
  Write_Policy WritePolicy = Write_Policy::INVALID;
  size_t CapacityOfL1Cache = 0;
  size_t CapacityOfL2Cache = 0;
  size_t CapacityOfL3Cache = 0;
  size_t CapacityOfL4Cache = 0;
} parameters;

typedef struct cache_entry {
  size_t addr;
  int data;
  bool dirty;  //Relevant for write-thru policy
  bool modified_in_curr_access;
} t_cache_entry;

class Cache {
public:
  Cache(size_t s) : size(s) {}
  int findAddr(size_t addr);
  void write(size_t addr, int data, Write_Policy wp);
  void setLowerLevelCache(Cache *c);
  void setUpperLevelCache(Cache *c);

private:
  vector<vector<t_cache_entry>> contents;
  size_t size;  //Max entries in the outer vector above
  Cache *upper;
  Cache *lower;
};


class L1Cache : public Cache {
public:
  L1Cache() : Cache(2) {}
};


class L2Cache : public Cache {
public:
  L2Cache() : Cache(4) {}
};


class MemAccess {
public:
  bool read;
  size_t addr;
  int write_data;
};

class MemAccesses {
private:
  vector<MemAccess> mem_accesses;
};


namespace snucs {
void usage(string progname);
template <typename T> void sciToNumber(const std::string &str, T &val, 
                                       const char *context);
void LoadParms(string config_filenm, parameters *parms);
void PrintConfigFileDataStruct(parameters *parms);
void PrintAccessPatternFileDataStruct(vector<MemAccess> &m_accesses);
void LoadAccessPatterns(string access_pat_filenm,
                        vector<MemAccess> &m_accesses);
}  //End namespace snucs

#endif

