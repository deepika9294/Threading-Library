#!/bin/sh

CYAN="\e[36m"
ENDCOLOR="\e[0m"



echo "${CYAN}\n\n\n####### ONE-ONE TESTING BEGINS #######${ENDCOLOR}\n";

./methods_test
./matrix_test
./spin_test
./mutex1_test
./mutex2_test

echo "\n\n${CYAN}####### ONE-ONE TESTING FINISHED #######${ENDCOLOR}\n";




