#
#$Id: run.sh,v 1.6 2026/09/01 11:12:45 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
  #progname="$OneDrive/VenkatsFiles/WorkFiles/SoftwareDevp/ECE303/CacheSim_RW_AccessPatterns/x64/Debug/CacheSim_RW_AccessPatterns.exe"	#Windows
  progname=../../x64/Debug/CacheSim_RW_AccessPatterns.exe	#Windows
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
      progname=../../cache_sim.x	#Linux
fi

pwd && \
egrep -v '^\s*$' golden_output.txt > $tmp_gold_file && \
$progname  -ap access_pattern.dat -c config_file.dat -o output.txt && \
diff --strip-trailing-cr $tmp_gold_file output.txt && \
echo "-I-: SUCCESS" || \
{ echo "-F-: FAILURE $(realpath $0) when running: diff $tmp_gold_file output.txt"; exit 1; }
