//
//$Id: cache_sim.h,v 1.5 2026/08/29 11:11:06 venkatnarayan.h Exp venkatnarayan.h $
/*
* Copyright (c) 2026, Shiv Nadar University, Delhi NCR, India. All Rights
* Reserved. Permission to use, copy, modify and distribute this software for
* educational, research, and not-for-profit purposes, without fee and without a
* signed license agreement, is hereby granted, provided that this paragraph and
* the following two paragraphs appear in all copies, modifications, and
* distributions.
*
* IN NO EVENT SHALL SHIV NADAR UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST
* PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE.
*
* SHIV NADAR UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS PROVIDED "AS IS". SHIV
* NADAR UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
* ENHANCEMENTS, OR MODIFICATIONS.*/

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
	Replacement_Policy ReplacementPolicy = Replacement_Policy::INVALID;
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
	template <typename T> void sciToNumber(const std::string& str, T& val,
		const char* context);
	void LoadParms(string config_filenm, parameters* parms);
	void PrintConfigFileDataStruct(parameters* parms);
	void PrintAccessPatternFileDataStruct(vector<MemAccess>& m_accesses);
	void LoadAccessPatterns(string access_pat_filenm,
		vector<MemAccess>& m_accesses);
}  //End namespace snucs

#endif

