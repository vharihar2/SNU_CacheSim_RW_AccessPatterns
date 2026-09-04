# CacheSim_RW_AccessPatterns
This is a simple Cache Simulator developed by Dr Venkatnarayan Hariharan of the Dept of Electrical Engg at Shiv Nadar Institution of Eminence, Delhi NCR. It is primarily motivated by an exercise problem in Prof Smruti Ranjan Sarangi's text titled *[Basic Computer Architecture](https://www.cse.iitd.ac.in/~srsarangi/archbooksoft.html)* (whose text I follow to teach COD to UG students). My motive was to proliferate more similar problems and find automated solutions to them.

*As of Aug 31st 2026, it has the following features*:
- Supports a data file containing any number of user-specified access patterns.
- Supports a configuration file that specifies #levels in the cache hierarchy, and whether it follows a write-through or write-back policy. Any number of cache levels are supported, though presently in `cache_sim.cpp` containing `main()`, it is hardcoded to 2 levels of cache. It is written so that it is easily expandable to any number of caches.
- Only LRU and FIFO cache replacement policies are supported for now.
- The cache line length is just 1 word (bcoz, as I said, this task was solely motivated by the exercise problem).


*Caveats*:
- In the output listing, amongst other things, we display a flag indicating whether a cache entry "was modified in the current access". This feature could misbehave sometimes, eg. when multiple addresses in a cache level are written to in the same access (eg. L1 miss and L2 miss, fetch from main memory (MM) into L2 at index A2, then attempt to load it into L1 but no free slot in L1, so need to evict say index A1 in L1 by waterfalling it to L2 (if WRITE_BACK policy), but no free slot in L2 either, so need to evict say index A3 in L2 (by waterfalling it to MM) and then writing at index A3 in L2. Thus, in L2, two indices would have been "modified in the current access", viz. A2 and A3.

<br>

**Prerequisites for building:**
- Ensure that the C++ Boost library is installed and its location is made known to your build system (MSVS 2026 Community Edition in my case, though you can use whatever you want).
- Ensure that the IniReader library (a simple key-value parser that I built) is built and staged in a suitable directory (inferable from the project settings). The IniReader library is available at [IniReader GitHub Repository](https://github.com/vharihar2/IniReader).

<br>

***Future TODOs (@@TODO)***:
- P1 (top priority):
    - **IMP**: Test out write-through cases too (there is an exercise problem regarding that too).
    - **IMP**: Fix the caveat related to "multiple words in a cache being modified in an access".
    - The `map` member variable in `MainMemory` class is presently `public`, as a quickfix. Need to make it `private` to be cleaner.
    - Uninitialized memory (ie. junk) loaded from MM is presently deemed as -99999. A better implementation is to have a `bool junk_data` flag.
- P2:
    - None.
- P3:
    - Make the cache instantiation dynamic in `cache_sim.cpp` in line with the intent of the config file, instead of hardcoding it to 2 levels.
- P4:
    - Add performance modeling capability.
