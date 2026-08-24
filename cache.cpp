//
//$Id$
//
#include "cache.h"

int Cache::findIndexOfAddr(size_t addr)
{
    for (size_t i = 0; i < size; ++i)
        if (contents[i].addr == addr)
            return i;

    return -1;
}


int Cache::findFreeSlotIndex()
{
    size_t n = contents.size();
    return (size > 1 ? -1 : size);
}


void Cache::insertAt(int free_slot, const t_cache_entry &cache_entry)
{
	contents[free_slot] = cache_entry;
}


size_t Cache::findIndexOfLruEntry()
{
    size_t lru_index = 0;
    size_t lru_timestamp = contents[0].timestamp;

    for (size_t i = 1; i < size; ++i)
    {
        if (contents[i].timestamp < lru_timestamp)
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
        cache_entry.dirty = (wp == Write_Policy::WRITE_THRU);
        cache_entry.modified_in_curr_access = true;
        cache_entry.timestamp = curr_max_timestamp++;

        insertAt(index_to_insert_at, cache_entry);
    }
    else  //Addr found in cache.
    {
        printf("Not implemented yet!\n");   //@@
        exit(1); //@@
    }
}


