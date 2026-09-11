#
#$Id: run1.sh,v 1.6 2026/09/01 11:13:04 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt
tmp_dummy_file=/tmp/vh2_$$.txt

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
  #progname="$OneDrive/VenkatsFiles/WorkFiles/SoftwareDevp/ECE303/CacheSim_RW_AccessPatterns/x64/Debug/CacheSim_RW_AccessPatterns.exe"	#Windows
  progname=../../x64/Debug/CacheSim_RW_AccessPatterns.exe	#Windows
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
      progname=../../cache_sim.x	#Linux
fi

egrep -v '^\s*$' golden_output1.txt > $tmp_gold_file && \
$progname -ap access_pattern.dat -c config_file1.dat \
                              -o $tmp_dummy_file > output1.txt
diff --strip-trailing-cr $tmp_gold_file output1.txt && \
echo "-I-: SUCCESS" || \
{ echo "-F-: FAILURE $(realpath $0)"; exit 1; }

