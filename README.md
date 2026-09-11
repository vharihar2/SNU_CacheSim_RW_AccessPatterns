# CacheSim_RW_AccessPatterns
This is a simple Cache Simulator developed by Dr Venkatnarayan Hariharan of the Dept of Electrical Engg at Shiv Nadar Institution of Eminence, Delhi NCR. It is primarily motivated by an exercise problem in Prof Smruti Ranjan Sarangi's text titled *[Basic Computer Architecture](https://www.cse.iitd.ac.in/~srsarangi/archbooksoft.html)* (whose text I follow to teach COD to UG students). My motive was to proliferate more similar problems and find automated solutions to them.

*It has the following features*:
- To invoke, run it as follows:
    - `x64\Debug\CacheSim_RW_AccessPatterns.exe -ap access_pattern.dat -c config_file.dat -o output.txt`
- Takes a data file as an input that contains any number of user-specified access patterns.
- Takes a configuration file as an input that specifies #levels in the cache hierarchy, and whether it follows a write-through or write-back policy. Any number of cache levels are supported, though presently in `cache_sim.cpp` containing `main()`, it is hardcoded to 2 levels of cache. It is written so that it is easily expandable to any number of caches.
- Writes out the cache state after each access to an output file.
- Only LRU and FIFO cache replacement policies are supported for now.
- The cache line length is just 1 word (bcoz, as I said, this task was solely motivated by the exercise problem).
- A poor-man's "GUI visualizer" is available by running a Python script on the generated output file. The script generates an Excel file which is much more readable. To use the Python formatter on the generated output, run it as follows (needs Python 3, it has been tested on Python v3.14.7)
  > `python <Path to CacheSimOutToXLSX.py> <Path to the generated output.txt> -o <Path to the generated output.xlsx>`


*Caveats*:
- None.

<br>

**Prerequisites for building:**
- Ensure that the C++ Boost library is installed and its location is made known to your build system (MSVS 2026 Community Edition in my case, though you can use whatever you want).
- Ensure that the IniReader library (a simple key-value parser that I built) is built and staged in a suitable directory (inferable from the project settings). The IniReader library is available at [IniReader GitHub Repository](https://github.com/vharihar2/IniReader).

<br>

***Future TODOs (@@TODO)***:
- P1 (top priority):
    - The `map` member variable in `MainMemory` class is presently `public`, as a quickfix. Need to make it `private` to be cleaner.
    - Uninitialized memory (ie. junk) loaded from MM is presently deemed as -99999. A better implementation is to have a `bool junk_data` flag.
- P2:
    - None.
- P3:
    - Fix the visualization script `test\CacheSimOutToXLSX.py` to handle any number of cache levels, instead of just 2.
- P4:
    - Add performance modeling capability.
