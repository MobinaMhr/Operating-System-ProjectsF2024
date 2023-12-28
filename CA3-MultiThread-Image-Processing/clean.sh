#!/bin/env bash

NC='\033[0m'
LIGHT_GREEN='\033[0;92m'


cd ./serial || exit
make clean > /dev/null
touch ./serial.txt
cd ..

cd ./parallel || exit
make clean > /dev/null
touch ./parallel.txt
cd ..


echo -e "${LIGHT_GREEN}-------------- Successfully Cleaned Up -------------${NC}"
