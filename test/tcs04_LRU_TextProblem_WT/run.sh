#
#$Id: run.sh,v 1.7 2026/09/04 06:58:16 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt

pwd && \
egrep -v '^\s*$' golden_output.txt > $tmp_gold_file && \
../../cache_sim.x access_pattern.dat config_file.dat && \
../../cache_sim.x access_pattern.dat config_file.dat | \
  egrep 'V:.*D:' > output.txt && \
diff $tmp_gold_file output.txt && \
echo "-I-: SUCCESS" || \
{ echo "-F-: FAILURE $(realpath $0) when running: diff $tmp_gold_file output.txt"; exit 1; }

