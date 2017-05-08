#! /bin/bash

MATDIR=`echo $0 | sed -e "s/test_main.sh//"`
if [ ! -d "$MATDIR" ]; then
	echo "!* Error: can't find {MATDIR}!"
	exit
fi

PROGRAM=./test_main

SHORT_TEST="$@"

if [ "$SHORT_TEST" != "" ]; then
POSKI_TEST="Short"
MATFILE=(Small)
MODE=(SHARE_INPUTMAT)
TUNE=(NO_TUNE TUNE)
HINT=(HINT_SINGLE_BLOCK)
else
POSKI_TEST="Completed"
MATFILE=(Dense raefsky6.pua)
MODE=(SHARE_INPUTMAT COPY_INPUTMAT)
TUNE=(NO_TUNE TUNE)
HINT=(HINT_SINGLE_BLOCK HINT_MULTIPLE_BLOCKS)
fi

KERNEL=(KERNEL_MatMult)
OP=(OP_NORMAL OP_TRANS)

report=1
iter=3
count=0;


echo ""
echo "!*** $POSKI_TEST Testing... ***!"

if [ -f test_main ]; then
for (( i=0; i < ${#MATFILE[@]}; i++ )); do
for (( mode=0; mode < ${#MODE[@]}; mode++ )); do
for (( tune=0; tune < ${#TUNE[@]}; tune++ )); do
for (( hint=0; hint < ${#HINT[@]}; hint++ )); do
for (( kernel=0; kernel < ${#KERNEL[@]}; kernel++ )); do
for (( op=0; op < ${#OP[@]}; op++ )); do
count=`expr $count + 1`
echo "Test$count: $iter iters, ${MATFILE[$i]}, ${MODE[$mode]}, ${TUNE[$tune]}, ${HINT[$hint]}, ${KERNEL[$kernel]}, ${OP[$op]}"
env POSKI_MESSAGE_LEVEL=0 $PROGRAM -num $count -iter $iter -file ${MATDIR}/${MATFILE[$i]} -mode $mode -tune $tune -hint $hint -kernel $kernel -op $op -report $report 
done; done; done; done; done; done
else
echo "!*** Error: can't find an execution file ***!"
fi

echo "!*** End of total $count testing. ***!"
echo ""

