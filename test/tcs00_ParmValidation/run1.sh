#
#$Id: run1.sh,v 1.5 2026/08/31 11:06:20 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt

egrep -v '^\s*$' golden_output1.txt > $tmp_gold_file && \
../../cache_sim.x access_pattern.dat config_file1.dat > output1.txt
diff $tmp_gold_file output1.txt

