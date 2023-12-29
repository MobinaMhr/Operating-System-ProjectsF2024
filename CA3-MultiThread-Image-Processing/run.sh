#!/bin/env bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
LIGHT_GREEN='\033[0;92m'

test_count=1
input="./../assets/input1.bmp"
test_iterations=1
thread_count=4

cd ./serial || exit
make clean > /dev/null
make > /dev/null || { echo "Error: Compilation failed for serial"; exit 1; }
echo "$(ls)"

touch ./serial.txt
echo "--------------"

echo "$(ls)"

./ImageFilters.out "$input" "$test_iterations" >> ./serial.txt

echo -e "${LIGHT_GREEN}-------------- Running Serial -------------${NC}"
echo -e "${GREEN}$(cat serial.txt)${NC}"
serial=$(grep "Total Execution Time:" ./serial.txt | awk '{print $5}')

echo 
cd ..

cd ./parallel || exit

make clean > /dev/null
make > /dev/null || { echo "Error: Compilation failed for parallel"; exit 1; }
touch ./parallel.txt

./ImageFilters.out "$input" "$test_iterations" "$thread_count" >> ./parallel.txt

echo -e "${LIGHT_GREEN}------------- Running Parallel -------------${NC}"
echo -e "${GREEN}$(cat parallel.txt)${NC}"
parallel=$(grep "Total Execution Time:" ./parallel.txt | awk '{print $5}')

echo 
cd ..

speedup=$(bc -l <<< "$serial / $parallel")
echo -e "${RED}Speedup = ${speedup::6}${NC}"
