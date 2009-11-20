#!/bin/sh

# This script allows the run of an already recorded test
# Giving a <test_name> it assumes the existence of 2 files
# <test_name>.xns which is the cnee events file
# <test_name>.tlp which is the expected tulip result graph
# The test must locally produces a tulip result graph
# in the test_gui.tlp file
# After the test run, the files <test_name>.tlp and test_gui.tlp
# are compared; the test is successful if they only differ from the date field

if [ $# -ne 2 ]; then
    echo "$0 running test failed"
    echo "usage: $0 <test_name> <tlp_input_file>"
    exit
fi

TEST_NAME=$1
TLP_INPUT_FILE=$2

echo "***********************************************"
echo "Running ${TEST_NAME} test"
echo "***********************************************"

# $TLP_INPUT_FILE must exist
if [ ! -f $TLP_INPUT_FILE ]; then
    echo "$TEST_NAME test failed: $TLP_INPUT_FILE does not exist"
    exit
fi

# $TEST_NAME.xns must exist
if [ ! -f $TEST_NAME.xns ]; then
    echo "$TEST_NAME test failed: $TEST_NAME.xns does not exist"
    exit
fi

# $TEST_NAME.tlp must exist
if [ ! -f $TEST_NAME.tlp ]; then
    echo "$TEST_NAME test failed: $TEST_NAME.tlp does not exist"
    exit
fi

# remove test_gui.tlp which is always
# the name of the result graph file
if [ -f test_gui.tlp ]; then
    rm test_gui.tlp
fi

# run the test
sh tulip_replay.sh $TEST_NAME.xns $TLP_INPUT_FILE

# test_gui.tlp must exist
if [ ! -f test_gui.tlp ]; then
    echo "$TEST_NAME test failed: test_gui.tlp does not exist"
    exit
fi

# check the result graph file
# first check dates
if [ "$(grep '(date ' $TEST_NAME.tlp)" = "$(grep '(date ' test_gui.tlp)" ]; then
     NB_DIFFS=0
else
     NB_DIFFS=4
fi
# the 2 files may only have different dates
if [ $(diff $TEST_NAME.tlp test_gui.tlp | wc -l) -gt $NB_DIFFS ]; then
    mv test_gui.tlp failed_${TEST_NAME}.tlp
    echo "$TEST_NAME test failed: diff failure between ${TEST_NAME}.tlp & failed_${TEST_NAME}.tlp"
else
    rm test_gui.tlp
    echo "*** $TEST_NAME test OK ***"
fi
