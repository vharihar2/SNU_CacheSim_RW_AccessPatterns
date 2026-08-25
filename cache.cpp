//
//$Id$
//
#include "cache.h"

Cache::Cache(size_t s) //@@: size(s)
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
/*@@    if (n > size) {
        printf("-E-: More entries than cache size set up!\n");   //Failsafe code
        exit(1);
    }*/

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
/*@@    else if (free_slot < size) {
        contents[free_slot] = cache_entry;
    }
    else
    {
        printf("-E-: Free slot index MUST be less than the cache size set up!\n");   //Failsafe code
        exit(1);
    }
*/
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
/*@@    if (n > size) {
        printf("-E-: More entries than cache size set up!\n");   //Failsafe code
        exit(1);
    }

    if (n == 0) return 0;

    size_t lru_index = 0;
    size_t lru_timestamp = contents[0].timestamp;

    for (size_t i = 1; i < n; ++i)
    {
        if (contents[i].timestamp < lru_timestamp)
        {
            lru_index = i;
            lru_timestamp = contents[i].timestamp;
        }
    }
    */

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

void Cache::write(size_t addr, int data, Write_Policy wp)
{
    t_cache_entry cache_entry;
    cache_entry.addr = addr;
    cache_entry.data = data;
    cache_entry.valid = true;

    if (findIndexOfAddr(addr) < 0)  //Addr not in cache.
    {
        int index_to_insert_at = findFreeSlotIndex();

        if (index_to_insert_at < 0)  //No free slot.
        {
            //Need to first evict (after waterfalling to lower level cache if WRITE_BACK and if LRU entry is dirty).
            index_to_insert_at = (int)findIndexOfLruEntry();    //Find the LRU entry in this cache (to evict).

            if (wp == Write_Policy::WRITE_BACK)
            {
                //Waterfall to lower level cache if LRU entry is dirty.
                if (contents[index_to_insert_at].dirty)
                {
                    //Waterfall to lower level cache (if any) and reset the dirty bit.
                    if (lower)
                        lower->write(contents[index_to_insert_at].addr, contents[index_to_insert_at].data, wp);

                    contents[index_to_insert_at].dirty = false;    //Reset the dirty bit at the current level.
                }
            }
        }

        //Now write it in the curr cache level
        cache_entry.dirty = (wp == Write_Policy::WRITE_BACK);
        cache_entry.modified_in_curr_access = true;
        cache_entry.timestamp = ++curr_max_timestamp;

        insertAt(index_to_insert_at, cache_entry);
    }
    else  //Addr found in cache.
    {
        printf("Not yet implemented!\n");   //@@
        exit(1); //@@
    }
}


