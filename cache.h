//
//$Id: cache.h,v 1.8 2026/08/29 11:11:23 venkatnarayan.h Exp venkatnarayan.h $
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
	size_t timestamp;	//For LRU replacement policy
} t_cache_entry;

class Cache {
public:
	Cache(size_t s, size_t levelMinus1);
	int findIndexOfAddr(size_t addr);
	int findFreeSlotIndex();
	void insertAt(int free_slot, const t_cache_entry& cache_entry);
	size_t findIndexOfLruEntry();
	int read(size_t addr, Write_Policy wp, vector<int>& level_indices_modified_in_curr_access);
	void write(size_t addr, int data, Write_Policy wp, vector<int>& level_indices_modified_in_curr_access);
	void setLowerLevelCache(Cache* c, MainMemory* mm);
	void setUpperLevelCache(Cache* c, MainMemory* mm);
	void printContents(int index_modified_in_curr_access, bool printNewline = true);

private:
	vector<t_cache_entry> contents;	//Contents of the cache after the current access pattern.
	size_t levelMinus1;	//Eg.: Hold 0 for L1 cache, 1 for L2 cache, etc.
	Cache* upper = NULL;
	Cache* lower = NULL;
	MainMemory* mm = NULL;
	int curr_max_timestamp = -1;

	int getDataAtIndex(size_t index);
	void processCacheMiss(size_t addr, Write_Policy wp, bool write, int& data,
		vector<int>& level_indices_modified_in_curr_access);
};


namespace snucs {

}  //End namespace snucs

#endif

