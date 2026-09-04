//
//$Id: cache.cpp,v 1.12 2026/08/29 11:14:02 venkatnarayan.h Exp venkatnarayan.h $//
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

#include "cache.h"

using namespace snucs;

Cache* Cache::highest_level_cache = nullptr;

Cache::Cache(size_t s)
{
	t_cache_entry cache_entry;

	cache_entry.addr = 0;
	cache_entry.data = 0;
	cache_entry.valid = false;
	cache_entry.dirty = false;
	cache_entry.timestamp = 0;
	cache_entry.last_access_time = 0;

	contents.resize(s, cache_entry);
}


int Cache::findIndexOfAddr(size_t addr)
{
	size_t n = contents.size();

	for (size_t i = 0; i < n; ++i)
		if (contents[i].addr == addr)
			return i;

	return -1;
}


int Cache::findFreeSlotIndex()
{
	size_t n = contents.size();
	for (size_t i = 0; i < n; i++) {
		if (!contents[i].valid)
			return i;
	}

	return -1;
}


void Cache::insertAt(int free_slot, const t_cache_entry& cache_entry)
{
	if (free_slot < 0) {
		printf("-E-: Free slot index cannot be negative!\n");   //Failsafe code
		exit(1);
	}
	contents[free_slot] = cache_entry;
}


void Cache::printContents(int index_modified_in_curr_access, bool printNewline)
{
	size_t n = contents.size();
	for (size_t i = 0; i < n; i++) {
		t_cache_entry& cache_entry = contents[i];
		printf("%zu,%d (V:%c, D:%c, MCA:%c, TS:%zu, LAT:%zu)\t",
			cache_entry.addr, cache_entry.data,
			cache_entry.valid ? 'T' : 'F', cache_entry.dirty ? 'T' : 'F',
			i == index_modified_in_curr_access ? 'T' : 'F',
			cache_entry.timestamp, cache_entry.last_access_time);
	}

	if (printNewline)
		printf("\n\n");
}


size_t Cache::findIndexOfFifoEntry()
{
	size_t n = contents.size();
	size_t least_index = 0;
	size_t least_timestamp = contents[0].timestamp;

	bool firsttime = true;
	//Limit focus only to the "valid" entries. If no valid entries, then deem lru_index as 0
	for (size_t i = 0; i < n; i++) {
		if (contents[i].valid) {
			if (firsttime) {
				least_index = i;
				least_timestamp = contents[i].timestamp;
				firsttime = false;
			}
			else if (contents[i].timestamp < least_timestamp) {
				least_index = i;
				least_timestamp = contents[i].timestamp;
			}
		}
	}

	return least_index;
}


size_t Cache::findIndexOfLruEntry()
{
	size_t n = contents.size();
	size_t least_index = 0;
	size_t least_last_access_time = contents[0].last_access_time;

	bool firsttime = true;
	//Limit focus only to the "valid" entries. If no valid entries, then deem lru_index as 0
	for (size_t i = 0; i < n; i++) {
		if (contents[i].valid) {
			if (firsttime) {
				least_index = i;
				least_last_access_time = contents[i].last_access_time;
				firsttime = false;
			}
			else if (contents[i].last_access_time < least_last_access_time) {
				least_index = i;
				least_last_access_time = contents[i].last_access_time;
			}
		}
	}

	return least_index;
}


void Cache::setLowerLevelCache(Cache* c, MainMemory* mm)
{
	lower = c;
	this->mm = mm;
}


void Cache::setUpperLevelCache(Cache* c, MainMemory* mm)
{
	upper = c;
	this->mm = mm;
}


void Cache::setAsHighestLevelCache()
{
	Cache::highest_level_cache = this;
	Cache* c = this;
	for (size_t i = 0; c; c = c->lower)
		c->levelMinus1 = i++;
}


int MainMemory::read(size_t addr)
{
	//If it is not "set" in main memory, return -99999 (just for the sake of this simulator). In reality, main memory will
	//never see a miss. If it is an unset or uninitialized memory, whatever is its contents (junk) will be returned. Thus, here we are
	//deeming -99999 as junk. A better implementation (@@TODO) is to have a "bool junk_data" flag, we'll get to it later.

	auto it = contents.find(addr);
	if (it == contents.end())
		return -99999;

	return it->second;
}


int Cache::getDataAtIndex(size_t index)
{
	return contents[index].data;
}


//Find the entry in this cache to evict.
size_t Cache::findIndexOfReplCandidate(Replacement_Policy rp)
{
	if (rp == Replacement_Policy::FIFO)
		return findIndexOfFifoEntry();
	else if (rp == Replacement_Policy::LRU)
		return findIndexOfLruEntry();
	else
		throw "-E-: Invalid replacement policy!";
}


//
//Handle cache misses. Here-in lies the complexity of the cache simulator (to handle eviction, etc). We need to ensure that we follow the
//"inclusive cache" design, handle the write policies correctly, and do slightly different things based on read vs write.
void Cache::processCacheMiss(size_t addr, Write_Policy wp, Replacement_Policy rp, bool write, int& data,
	vector<int>& level_indices_modified_in_curr_access)
{
	//First ensure that it exists in lower level cache (since we are assuming an "inclusive cache").
	int data_from_lower_level;
	if (lower)
		data_from_lower_level = lower->read(addr, wp, rp, level_indices_modified_in_curr_access);
	else
		data_from_lower_level = mm->read(addr);

	//Then insert it in the current level cache (do NOT call write, else we have an infinite loop of write calling read and vice versa).
	int index_to_insert_at = findFreeSlotIndex();

	if (index_to_insert_at < 0)  //No free slot, need to evict and replace.
	{
		//Need to first evict (after waterfalling to lower level cache if WRITE_BACK and if the entry is dirty).
		index_to_insert_at = (int)findIndexOfReplCandidate(rp);	//Find the entry in this cache to evict.

		if (wp == Write_Policy::WRITE_BACK) //In write-back, we waterfall dirty data to lower level cache ONLY UPON EVICTION.
		{
			//Waterfall to lower level cache if FIFO entry is dirty.
			if (contents[index_to_insert_at].dirty)
			{
				//Waterfall to lower level cache (if any) and reset the dirty bit.
				if (lower)
					lower->write(contents[index_to_insert_at].addr, contents[index_to_insert_at].data, wp, rp,
						level_indices_modified_in_curr_access); //Waterfall to lower level cache.
				else
					mm->contents[addr] = contents[index_to_insert_at].data; //Waterfall to main memory.

				if (!write)
					contents[index_to_insert_at].dirty = false;    //Having waterfalled, reset the dirty bit at the current level if doing a 'read'.
			}
		}   //Done waterfalling upon eviction if write-back.
	}

	//Now write it in the curr cache level
	t_cache_entry cache_entry;
	cache_entry.addr = addr;

	if (write)
		cache_entry.data = data;
	else {
		cache_entry.data = data_from_lower_level;
		data = data_from_lower_level;
	}

	cache_entry.valid = true;

	cache_entry.dirty = (write && (wp == Write_Policy::WRITE_BACK));
	cache_entry.timestamp = ++curr_max_timestamp;
	cache_entry.last_access_time = cache_entry.timestamp;

	insertAt(index_to_insert_at, cache_entry);
	level_indices_modified_in_curr_access[levelMinus1] = index_to_insert_at;
}


//
//Read data from cache.
int Cache::read(size_t addr, Write_Policy wp, Replacement_Policy rp, vector<int>& level_indices_modified_in_curr_access)
{
	int index = findIndexOfAddr(addr);

	if (index < 0)  //Cache miss.
	{
		int data;
		processCacheMiss(addr, wp, rp, false, data, level_indices_modified_in_curr_access); //Get it from lower level cache (if any) or main memory, and write it in the curr cache level.
		return data;
	}
	else
	{
		contents[index].last_access_time = ++curr_max_timestamp;	//Need to increment, else LRU doesn't work well.
		return getDataAtIndex(index);
	}
}


//
//Write data into cache.
void Cache::write(size_t addr, int data, Write_Policy wp, Replacement_Policy rp, vector<int>& level_indices_modified_in_curr_access)
{
	t_cache_entry cache_entry;
	cache_entry.addr = addr;
	cache_entry.data = data;
	cache_entry.valid = true;

	int index = findIndexOfAddr(addr);

	if (index < 0)  //Cache miss.
		processCacheMiss(addr, wp, rp, true, data, level_indices_modified_in_curr_access); //Get it from lower level cache (if any) or main memory, and write it in the curr cache level.
	else  //Cache hit. Write it in the curr cache level.
	{
		cache_entry.dirty = (wp == Write_Policy::WRITE_BACK);
		cache_entry.timestamp = ++curr_max_timestamp;
		cache_entry.last_access_time = cache_entry.timestamp;

		insertAt(index, cache_entry);
		level_indices_modified_in_curr_access[levelMinus1] = index;
	}

	//If write-through, waterfall the write to lower level caches/MM.
	if (wp == Write_Policy::WRITE_THRU) {
		if (lower)
			lower->write(addr, data, wp, rp, level_indices_modified_in_curr_access); //Waterfall to lower level cache.
		else
			mm->contents[addr] = data; //Waterfall to main memory.
	}
}

