#!/bin/bash
#
#$Id: run_tests.sh,v 1.1 2026/09/01 11:18:43 venkatnarayan.h Exp venkatnarayan.h $
#
#Run all testcases (tcs dirs)
for i in tcs*
do
  pushd $i
  ./run.sh || { echo "-F-: Regression tests FAILED!"; exit 1; }
  popd
done

echo
echo "-I-: All regression tests PASSED!"
echo

