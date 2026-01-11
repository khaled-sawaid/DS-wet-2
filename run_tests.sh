#!/bin/bash

# Define colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Updated executable name for wet-2 based on your CMakeLists.txt
EXEC="cmake-build-debug/DS_wet2_Winter_2026_01.exe"

if [ ! -f "$EXEC" ]; then
   echo "File $EXEC not found!"
   echo "Make sure to build the project in CLion first."
   exit 1
fi

echo "Starting tests for wet-2..."
echo "---------------------------"

for test in tests/*.in; do
    # Skip if no tests found
    [ -e "$test" ] || continue

    # Define expected output file name
    expected_output="${test%.in}.out"
    my_output="${test%.in}.res"

    # Run the test
    ./"$EXEC" < "$test" > "$my_output"

    # Compare output using the same flags as your old script
    if diff --brief --ignore-all-space "$my_output" "$expected_output" > /dev/null; then
        echo -e "Test $test: ${GREEN}PASS${NC}"
        # Optional: remove result file if passed to keep folder clean
        rm "$my_output"
    else
        echo -e "Test $test: ${RED}FAIL${NC}"
        echo "Differences:"
        diff --ignore-all-space "$my_output" "$expected_output"
        # Optional: exit on first failure
        # exit 1
    fi
done

echo "---------------------------"
echo "All tests completed."
exit 0