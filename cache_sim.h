//
//$Id: cache_sim.h,v 1.3 2026/08/19 13:03:55 venkatnarayan.h Exp venkatnarayan.h $
//
#ifndef SNUCS_CACHE_SIM
#define SNUCS_CACHE_SIM

#include "cache.h"
#include "vhutil_config_unique_keys.h"

using namespace std;

#define SNUCS_MAX_CACHE_LEVELS 3  //L1, L2, L3 (excluding main memory)

typedef struct t_parameters {
  //Input parameters:
  size_t NumCacheLevelsExclMM = 0;
  Write_Policy WritePolicy = Write_Policy::INVALID;
  size_t CapacityOfL1Cache = 0;
  size_t CapacityOfL2Cache = 0;
  size_t CapacityOfL3Cache = 0;
  size_t CapacityOfL4Cache = 0;
} parameters;

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

