//
//$Id: cache_sim.cpp,v 1.5 2026/08/19 13:03:55 venkatnarayan.h Exp venkatnarayan.h $
//
#include "cache_sim.h"

int main(int argc, char **argv);

namespace snucs {

void usage(string progname)
{
  cout << 
"Usage: \n" <<
"  " << progname << " <Access pattern dat file> <Config file>\n";
}


void LoadParms(string config_filenm, parameters *parms)
{
  map<string, string> config_kvs;
  if (!vhutil_config::readConfigFile(config_filenm, config_kvs))
    exit(1);

  for (auto const &kv : config_kvs) {  //Loop thru KV pairs map
    try {
#define MCFD_IF_KEY_COND(k,T) \
  (kv.first == #k) { \
    sciToNumber<T>(kv.second, parms->k, kv.first.c_str()); \
  }

      if MCFD_IF_KEY_COND(NumCacheLevelsExclMM, size_t)
      else if MCFD_IF_KEY_COND(CapacityOfL1Cache, size_t)
      else if MCFD_IF_KEY_COND(CapacityOfL2Cache, size_t)
      else if MCFD_IF_KEY_COND(CapacityOfL3Cache, size_t)
      else if MCFD_IF_KEY_COND(CapacityOfL4Cache, size_t)
      else if (kv.first == "WritePolicy") {
        if (kv.second == "WB") parms->WritePolicy = Write_Policy::WRITE_BACK;
        if (kv.second == "WT") parms->WritePolicy = Write_Policy::WRITE_THRU;
      }
      else {
        std::cerr << "-E-: " << "Unrecognized key '" << kv.first << "'" << std::endl;
        exit(1);
      }
    }
    catch (std::string &e) {
      std::cerr << "-E-: " << e << std::endl;
      exit(1);
    }
  }  //End loop thru KV pairs map

  //Check that all necessary members are set
  if (parms->WritePolicy == Write_Policy::INVALID) {
    printf("-E-: WritePolicy as not been specified in the config file\n");
    exit(1);
  }

  switch (parms->NumCacheLevelsExclMM)
  {
    case 0:
      printf("-E-: NumCacheLevelsExclMM as not been specified in the config file\n");
      exit(1);

    case 1:
      if (parms->CapacityOfL1Cache && !parms->CapacityOfL2Cache &&
         !parms->CapacityOfL3Cache && !parms->CapacityOfL4Cache) {}
      else {
        printf("-E-: CapacityOfL1Cache (only) must be specified in the config file\n");
        exit(1);
      }
      break;

    case 2:
      if (parms->CapacityOfL1Cache && parms->CapacityOfL2Cache &&
         !parms->CapacityOfL3Cache && !parms->CapacityOfL4Cache) {}
      else {
        printf("-E-: Capacities of L1 and L2 caches (only) must be specified in the config file\n");
        exit(1);
      }
      break;

    case 3:
      if (parms->CapacityOfL1Cache && parms->CapacityOfL2Cache &&
          parms->CapacityOfL3Cache && !parms->CapacityOfL4Cache) {}
      else {
        printf("-E-: Capacities of L1, L2 and L3 caches (only) must be specified in the config file\n");
        exit(1);
      }
      break;

    case 4:
      if (parms->CapacityOfL1Cache && parms->CapacityOfL2Cache &&
          parms->CapacityOfL3Cache && parms->CapacityOfL4Cache) {}
      else {
        printf("-E-: Capacities of L1, L2, L3 and L4 caches (only) must be specified in the config file\n");
        exit(1);
      }
      break;

    default:
      printf("-E-: Invalid value for NumCacheLevelsExclMM in the config file\n");
      exit(1);
  }

  printf("-I-: Done reading config file, printing parameters read ...\n");
  PrintConfigFileDataStruct(parms);  //@@Debug
  printf("-I-: Done printing parameters read.\n\n");
}


template <typename T> void sciToNumber(const string &str, T &val, 
                                             const char *context)
{
  try {
    val = boost::lexical_cast<T>(str);
  }
  catch (boost::bad_lexical_cast &) {
    std::string s = "-E-: Unable to format \"";
    s += str;
    s += "\" as a number (";
    s += std::string(context);
    s += ")\n";
    cout << s;
    exit(1);
  }
}


void PrintConfigFileDataStruct(parameters *parms)
{
  printf("parms->NumCacheLevelsExclMM = %zu\n", parms->NumCacheLevelsExclMM);
  printf("parms->WritePolicy = %s\n",
    parms->WritePolicy == Write_Policy::WRITE_BACK ? "WB" :
    parms->WritePolicy == Write_Policy::WRITE_THRU ? "WT" :
    "INVALID");
  printf("parms->CapacityOfL1Cache = %zu\n", parms->CapacityOfL1Cache);
  printf("parms->CapacityOfL2Cache = %zu\n", parms->CapacityOfL2Cache);
  printf("parms->CapacityOfL3Cache = %zu\n", parms->CapacityOfL3Cache);
  printf("parms->CapacityOfL4Cache = %zu\n", parms->CapacityOfL4Cache);
}


void LoadAccessPatterns(string access_pat_filenm, vector<MemAccess> &m_accesses)
{
  std::ifstream file(access_pat_filenm);

  if (!file)
  {
    std::cerr << "Cannot open '" << access_pat_filenm << "'\n";
    exit(1);
  }

  std::string line;

  while (std::getline(file, line))
  {
    if (line.empty()) continue;

    std::istringstream iss(line);

    char read_write;
    MemAccess m_access;

    if (iss >> read_write >> m_access.addr)
    {
      if (read_write == 'R') {
        m_access.read = true;
        m_access.write_data = -99999;
      }
      else if (read_write == 'W') {
        m_access.read = false;
        iss >> m_access.write_data;
      }
      else {
        cerr << "-W-: Invalid read/write flag, skipping ...\n";
        continue;
      }

      /* @@Debug
      cout << "read_flag = " << m_access.read
           << ", addr = " << m_access.addr
           << ", write_data = " << m_access.write_data
           << '\n';
      */

      m_accesses.push_back(m_access);
    }
  }

  printf("-I-: Done reading access patterns, printing contents ...\n");
  PrintAccessPatternFileDataStruct(m_accesses);   //@@Debug
  printf("-I-: Done printing access patterns read.\n\n");
}


void PrintAccessPatternFileDataStruct(vector<MemAccess> &m_accesses)
{
  for (const auto &access : m_accesses) {
    printf("%c %zu", access.read ? 'R' : 'W', access.addr);
    if (!access.read)
      printf(" %d", access.write_data);
    printf("\n");
  }
}


void SimulateCache(vector<MemAccess> &m_accesses, parameters *parms)
{
  Cache l1(2);
  Cache l2(4);
  MainMemory mm;

  l1.setLowerLevelCache(&l2, &mm);
  l2.setUpperLevelCache(&l1, &mm);

  for (const auto &access : m_accesses)
  {
    if (access.read)
    {
      //Read
      int data = l1.read(access.addr, parms->WritePolicy);
    } else
    {
      //Write
      l1.write(access.addr, access.write_data, parms->WritePolicy);
    }

    l1.printContents(false);
    printf("\t");
    l2.printContents();
  }
}

} //End namespace snucs


int main(int argc, char **argv)
{
  string progname(argv[0]);
  string access_pat_filenm;
  string config_filenm;
  parameters parms_struct = {};
  parameters *parms = &parms_struct;

  if (argc == 3) {
    access_pat_filenm = argv[1];
    config_filenm = argv[2];
    snucs::LoadParms(config_filenm, parms);
  }
  else {
    snucs::usage(progname);
    return 1;
  }

  cout << "access_pat_filenm: " << access_pat_filenm << "\n";
  cout << "config_filenm: " << config_filenm << "\n";

  vector<MemAccess> m_accesses;
  snucs::LoadAccessPatterns(access_pat_filenm, m_accesses);

  snucs::SimulateCache(m_accesses, parms);

  return 0;
}

