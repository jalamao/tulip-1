#!/bin/sh

# This script allows the run of an already recorded test
# Giving a <test_name> it assumes the existence of
# <test_name>.xns which is the cnee events file
# and if no tlp_output_file is given,
# <test_name>.tlp which is the expected tulip result graph
# If no tlp_input_file is given, tlp_output_file is set
# to empty_graph.tlp.
# The test must locally produces a tulip result graph
# in the test_gui.tlp file
# After the test run, the files <test_name>.tlp and test_gui.tlp
# are compared; the test is successful if they only differ from the date field

if [ $# -lt 1 ]; then
    echo "$0 running test failed"
    echo "usage: $0 <test_name> [<tlp_input_file> tlp_output_file = test_name.tlp]"
    exit
fi

TEST_NAME=$1
TLP_INPUT_FILE=$2
TLP_OUTPUT_FILE=$3

echo "***********************************************"
echo "Running ${TEST_NAME} test"
echo "***********************************************"

# check if $TLP_INPUT_FILE exists
if [ "$TLP_INPUT_FILE" != "" ]; then
  if [ ! -f $TLP_INPUT_FILE ]; then
    echo "$TEST_NAME test failed: $TLP_INPUT_FILE does not exist"
    exit
  fi
fi

# $TEST_NAME.xns must exist
if [ ! -f $TEST_NAME.xns ]; then
    echo "$TEST_NAME test failed: $TEST_NAME.xns does not exist"
    exit
fi

# $TEST_NAME.tlp must exist
if [ "$TLP_OUTPUT_FILE" = "" ]; then
  if [ ! -f $TEST_NAME.tlp ]; then
    echo "$TEST_NAME test failed: $TEST_NAME.tlp does not exist"
    exit
  fi
  TLP_OUTPUT_FILE=$TEST_NAME.tlp
fi

# remove test_gui.tlp which is always
# the name of the result graph file
if [ -f test_gui.tlp ]; then
    rm test_gui.tlp
fi

if [ -f data/test_gui.tlp ]; then
    rm data/test_gui.tlp
fi

# run the test
sh tulip_replay.sh $TEST_NAME.xns $TLP_INPUT_FILE

# move data/test_gui.tlp if it exist
if [ -f data/test_gui.tlp ]; then
    mv data/test_gui.tlp .
fi

# test_gui.tlp must exist
if [ ! -f test_gui.tlp ]; then
    echo "$TEST_NAME test failed: test_gui.tlp does not exist"
    exit
fi

# check the result graph file (see tlp_diff.sh)
if [ $(sh tlp_diff.sh $TLP_OUTPUT_FILE test_gui.tlp | wc -l) -gt 0 ]; then
    mv test_gui.tlp failed_${TEST_NAME}.tlp
    echo "$TEST_NAME test failed: diff failure between ${TLP_OUTPUT_FILE} & failed_${TEST_NAME}.tlp"
else
    rm test_gui.tlp
    echo "*** $TEST_NAME test OK ***"
fi
