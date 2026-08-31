#
#$Id: run.sh,v 1.3 2026/08/31 06:28:31 venkatnarayan.h Exp venkatnarayan.h $
#
tmp_gold_file=/tmp/vh_$$.txt

dos2unix output.txt && \
egrep -v '^\s*$' golden_output.txt > $tmp_gold_file && \
../../cache_sim.x access_pattern.dat config_file.dat | \
  egrep 'V:.*D:' > output.txt && \
tkdiff $tmp_gold_file output.txt &

