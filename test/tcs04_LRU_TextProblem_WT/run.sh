#
#$Id: run.sh,v 1.6 2026/09/01 11:12:45 venkatnarayan.h Exp venkatnarayan.h $
#
diff_xlsx() {
    local file1="$1"
    local file2="$2"
    local dir1="/tmp/vh_xlsx_cmp1_$$"
    local dir2="/tmp/vh_xlsx_cmp2_$$"

    mkdir -p "$dir1" "$dir2"
    unzip -q -o "$file1" -d "$dir1"
    unzip -q -o "$file2" -d "$dir2"

    # Compare worksheet XML files, excluding volatile metadata/timestamps
    diff -q -r -x "core.xml" -x "app.xml" -x "calcChain.xml" "$dir1/xl" "$dir2/xl" > /dev/null
    local status=$?

    rm -rf "$dir1" "$dir2"
    return $status
}

tmp_gold_file=/tmp/vh_$$.txt

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
  progname=../../x64/Debug/CacheSim_RW_AccessPatterns.exe	#Windows
  python_gui_name=../CacheSimOutToXLSX.py
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
  progname=../../cache_sim.x	#Linux
fi

pwd && \
egrep -v '^\s*$' golden_output.txt > $tmp_gold_file && \
$progname -ap access_pattern.dat -c config_file.dat -o output.txt && \
diff --strip-trailing-cr $tmp_gold_file output.txt && \
python $python_gui_name -c config_file.dat -o output.xlsx output.txt &&
diff_xlsx golden_output.xlsx output.xlsx && \
echo "-I-: SUCCESS" || \
{ echo "-F-: FAILURE $(realpath $0) when running: diff $tmp_gold_file output.txt   OR   diff_xlsx golden_output.xlsx output.xlsx"; exit 1; }
