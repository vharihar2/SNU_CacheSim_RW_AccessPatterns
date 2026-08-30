# CacheSim_RW_AccessPatterns
This is a simple Cache Simulator developed by Dr Venkatnarayan Hariharan of the Dept of Electrical Engg at Shiv Nadar Instituion of Eminence, Delhi NCR. It is primarily motivated by an exercise problem in Prof Smruti Ranjan Sarangi's text titled "Basic Computer Architecture" (whose text I follow to teach COD to UG students). My motive was to proliferate more similar problems and find automated solutions to them.

*As of Aug 30th 2026, it has the following features*:
- Supports a data file containing any number of user-specified access patterns.
- Supports a configuration file that specifies #levels in the cache hierarchy, and whether it follows a write-through or write-back policy. Any number of cache levels are supported, though presently in `cache_sim.cpp` containing `main()`, it is hardcoded to 2 levels of cache. It is written so that it is easily expandable to any number of caches.
- The cache replacement policy is fixed at LRU for now. Will be enhanced in future to other policies.
- The cache line length is just 1 word (bcoz, as I said, this task was solely motivated by the exercise problem).


*Caveats*:
- In the output listing, amongst other things, we display a flag indicating whether a cache entry "was modified in the current access". This feature could misbehave sometimes, eg. when multiple addresses in a cache level are written to in the same access (eg. L1 miss and L2 miss, fetch from main memory (MM) into L2 at index A2, then attempt to load it into L1 but no free slot in L1, so need to evict say index A1 in L1 by waterfalling it to L2 (if WRITE_BACK policy), but no free slot in L2 either, so need to evict say index A3 in L2 (by waterfalling it to MM) and then writing at index A3 in L2. Thus, in L2, two indices would have been "modified in the current access", viz. A2 and A3.

<br>

***Future TODOs (@@TODO)***:
- P1 (top priority):
    - Validate the o/p related to dirty bit, and fix any bugs related to it
    - The `map` member variable in `MainMemory` class is presently `public`, as a quickfix. Need to make it `private` to be cleaner.
    - Uninitialized memory (ie. junk) loaded from MM is presently deemed as -99999. A better implementation is to have a `bool junk_data` flag.
    - Test out write-through cases too (there is an exercise problem regarding that too).
- P2:
    - Add support for FIFO replacement policy (there is an exercise problem regarding that too).
- P3:
    - Make the cache instantiation dynamic in cache_sim.cpp in line with the intent of the config file, instead of hardcoding it to 2 levels.
    - Fix the caveat related to "multiple words in a cache being modified in an access".
