#
#$Id: run2.sh,v 1.2 2026/09/01 11:13:04 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt

egrep -v '^\s*$' golden_output2.txt > $tmp_gold_file && \
../../cache_sim.x access_pattern.dat config_file2.dat > output2.txt
diff $tmp_gold_file output2.txt && \
echo "-I-: SUCCESS" || \
{ echo "-F-: FAILURE $(realpath $0)"; exit 1; }

