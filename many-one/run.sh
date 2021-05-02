#!/bin/sh

CYAN="\e[36m"
ENDCOLOR="\e[0m"



echo "${CYAN}\n\n\n####### MANY-ONE TESTING BEGINS #######${ENDCOLOR}\n";

./methods_test
./spin_test
./matrix_test

echo "\n\n${CYAN}####### MANY-ONE TESTING FINISHED #######${ENDCOLOR}\n";
