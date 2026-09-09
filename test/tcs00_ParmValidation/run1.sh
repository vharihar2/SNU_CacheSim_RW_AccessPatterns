#
#$Id: run1.sh,v 1.6 2026/09/01 11:13:04 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt
tmp_dummy_file=/tmp/vh2_$$.txt

egrep -v '^\s*$' golden_output1.txt > $tmp_gold_file && \
../../cache_sim.x -ap access_pattern.dat -c config_file1.dat \
                              -o $tmp_dummy_file > output1.txt
diff $tmp_gold_file output1.txt && \
echo "-I-: SUCCESS" || \
{ echo "-F-: FAILURE $(realpath $0)"; exit 1; }

