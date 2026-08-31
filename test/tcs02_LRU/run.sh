#
#$Id: run.sh,v 1.5 2026/08/31 11:02:05 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt

egrep -v '^\s*$' golden_output.txt > $tmp_gold_file && \
../../cache_sim.x access_pattern.dat config_file.dat && \
../../cache_sim.x access_pattern.dat config_file.dat | \
  egrep 'V:.*D:' > output.txt && \
tkdiff $tmp_gold_file output.txt &

