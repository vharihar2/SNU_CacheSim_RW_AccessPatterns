//
//$Id$
//
#include "cache.h"

Cache::Cache(size_t s)
{
    t_cache_entry cache_entry;

    cache_entry.addr = 0;
    cache_entry.data = 0;
    cache_entry.valid = false;
    cache_entry.dirty = false;
    cache_entry.modified_in_curr_access = false;
    cache_entry.timestamp = 0;

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
    for (size_t i=0; i < n; i++) {
		if (!contents[i].valid)
			return i;
    }

    return -1;
}


void Cache::insertAt(int free_slot, const t_cache_entry &cache_entry)
{
    if (free_slot < 0) {
        printf("-E-: Free slot index cannot be negative!\n");   //Failsafe code
        exit(1);
    }
    contents[free_slot] = cache_entry;
}


void Cache::printContents(bool printNewline)
{
    for (const auto& cache_entry : contents)
    {
        printf("%zu,%d (V:%c, D:%c, MCA:%c, TS:%zu)\t",
            cache_entry.addr, cache_entry.data,
            cache_entry.valid ? 'T' : 'F', cache_entry.dirty ? 'T' : 'F',
            cache_entry.modified_in_curr_access?'T':'F', cache_entry.timestamp);
    }

    if (printNewline)
        printf("\n\n");
}


size_t Cache::findIndexOfLruEntry()
{
	size_t n = contents.size();
    size_t lru_index = 0;
    size_t lru_timestamp = contents[0].timestamp;

    //Limit focus only to the "valid" entries. If no valid entries, then deem lru_index as 0
    for (size_t i = 0; i < n; i++)
    {
        if (contents[i].valid && contents[i].timestamp < lru_timestamp)
        {
            lru_index = i;
            lru_timestamp = contents[i].timestamp;
        }
    }

    return lru_index;
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



void Cache::processCacheMiss(size_t addr, Write_Policy wp, bool write, int& data)
{
    //First ensure that it exists in lower level cache (since we are assuming an "inclusive cache").
    int data_from_lower_level;
    if (lower)
        data_from_lower_level = lower->read(addr, wp);
    else
        data_from_lower_level = mm->read(addr);

    //Then insert it in the current level cache (do NOT call write, else we have an infinite loop of write calling read and vice versa).
    //We follow the philosophy that read() at any level should NOT call write(). But it is ok for write() to call read().
    int index_to_insert_at = findFreeSlotIndex();

    if (index_to_insert_at < 0)  //No free slot.
    {
        //Need to first evict (after waterfalling to lower level cache if WRITE_BACK and if LRU entry is dirty).
        index_to_insert_at = (int)findIndexOfLruEntry();    //Find the LRU entry in this cache (to evict).

        if (wp == Write_Policy::WRITE_BACK) //In write-back, we waterfall dirty data to lower level cache ONLY UPON EVICTION.
        {
            //Waterfall to lower level cache if LRU entry is dirty.
            if (contents[index_to_insert_at].dirty)
            {
                //Waterfall to lower level cache (if any) and reset the dirty bit.
                if (lower)
                    lower->write(contents[index_to_insert_at].addr, contents[index_to_insert_at].data, wp);
                else
                    mm->contents[addr] = contents[index_to_insert_at].data;     //@@data;    //Save in main memory.

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
    cache_entry.modified_in_curr_access = write;
    cache_entry.timestamp = ++curr_max_timestamp;

    insertAt(index_to_insert_at, cache_entry);
}



//
//Read data from cache (in the process, load up the data into curr and lower level caches if not already present)
int Cache::read(size_t addr, Write_Policy wp)
{
    int index = findIndexOfAddr(addr);

    if (index < 0)  //Addr not in cache (cache miss), get it from lower level cache (if any) or main memory.
    {
        int data;
        processCacheMiss(addr, wp, false, data);
        return data;

        /*@@
        //Get it from lower level cache (if any) or main memory.
        //First ensure that it exists in lower level cache (since we are assuming an "inclusive cache").
        int data;
        if (lower)
            data = lower->read(addr, wp);
        else
            data = mm->read(addr);

        //Then insert it in the current level cache (do NOT call write, else we have an infinite loop of write calling read and vice versa).
        //We follow the philosophy that read() at any level should NOT call write(). But it is ok for write() to call read().
        int index_to_insert_at = findFreeSlotIndex();

        if (index_to_insert_at < 0)  //No free slot.
        {
            //Need to first evict (after waterfalling to lower level cache if WRITE_BACK and if LRU entry is dirty).
            index_to_insert_at = (int)findIndexOfLruEntry();    //Find the LRU entry in this cache (to evict).

            if (wp == Write_Policy::WRITE_BACK) //In write-back, we waterfall dirty data to lower level cache ONLY UPON EVICTION.
            {
                //Waterfall to lower level cache if LRU entry is dirty.
                if (contents[index_to_insert_at].dirty)
                {
                    //Waterfall to lower level cache (if any) and reset the dirty bit.
                    if (lower)
                        lower->write(contents[index_to_insert_at].addr, contents[index_to_insert_at].data, wp);
                    else
                        mm->contents[addr] = data;    //Save in main memory.

                    contents[index_to_insert_at].dirty = false;    //Reset the dirty bit at the current level.
                }
            }
        }
        else
        {
            //Cache miss. Have got the data from lower level cache. Need to insert in curr level where we DO have a free slot.
            //cache_entry.dirty = false;  //Data cannot be dirty in a read.
        }

        //@@

        //Now write it in the curr cache level
        t_cache_entry cache_entry;
        cache_entry.addr = addr;
        cache_entry.data = data;
        cache_entry.valid = true;
        cache_entry.dirty = false;  //Data cannot be dirty in a read.
        cache_entry.modified_in_curr_access = false;    //Data is not modified in a read.
        cache_entry.timestamp = ++curr_max_timestamp;

        insertAt(index_to_insert_at, cache_entry);
        */

        return data;
    }
    else
        return getDataAtIndex(index);
}


//
//Write data into cache (before that, load up the data into curr and lower level caches if not already present)
void Cache::write(size_t addr, int data, Write_Policy wp)
{
    t_cache_entry cache_entry;
    cache_entry.addr = addr;
    cache_entry.data = data;
    cache_entry.valid = true;

    int index = findIndexOfAddr(addr);

    if (index < 0)  //Addr not in cache (cache miss), get it from lower level cache (if any) or main memory.
    {
        processCacheMiss(addr, wp, true, data);

        /*
		//@@@@
        //Get it from lower level cache (if any) or main memory.
        //First ensure that it exists in lower level cache (since we are assuming an "inclusive cache").
        int dummy;    //We use "dummy" as var name, since we dont care about it, as we'll overwrite it with the new data anyway.
		if (lower)
			dummy = lower->read(addr, wp);
        else
            dummy = mm->read(addr);
        //@@

        @@@@
        //Get it into the current cache by doing a read, which'll recursively read from lower level caches if needed)
        int dummy = read(addr);
        @@

        //Then insert at curr level cache.
        int index_to_insert_at = findFreeSlotIndex();

        if (index_to_insert_at < 0)  //No free slot.
        {
            //Need to first evict (after waterfalling to lower level cache if WRITE_BACK and if LRU entry is dirty).
            index_to_insert_at = (int)findIndexOfLruEntry();    //Find the LRU entry in this cache (to evict).

            if (wp == Write_Policy::WRITE_BACK) //In write-back, we waterfall dirty data to lower level cache ONLY UPON EVICTION.
            {
                //Waterfall to lower level cache if LRU entry is dirty.
                if (contents[index_to_insert_at].dirty)
                {
                    //Waterfall to lower level cache (if any) and reset the dirty bit.
                    if (lower)
                        lower->write(contents[index_to_insert_at].addr, contents[index_to_insert_at].data, wp);
                    else
                        mm->contents[addr] = data;    //Save in main memory.

//@@                    contents[index_to_insert_at].dirty = false;    //Reset the dirty bit at the current level.
                }
            }
        }
        else {
            //There is a free slot in curr level cache, where we'll be inserting soon. Mark it as dirty.
//@@            cache_entry.dirty = (wp == Write_Policy::WRITE_BACK);
        }
        
        cache_entry.dirty = (wp == Write_Policy::WRITE_BACK);//@@

        //Now write it in the curr cache level
        cache_entry.modified_in_curr_access = true;
        cache_entry.timestamp = ++curr_max_timestamp;

        insertAt(index_to_insert_at, cache_entry);
        */
    }
    else  //Addr found in cache.
    {
        //Write it in the curr cache level.
        cache_entry.dirty = (wp == Write_Policy::WRITE_BACK);
        cache_entry.modified_in_curr_access = true;
        cache_entry.timestamp = ++curr_max_timestamp;

        insertAt(index, cache_entry);
    }
}


