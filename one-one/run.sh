#!/bin/sh

make matrix_test
make spin_test
make mutex1_test
make mutex2_test
make methods_test

./methods_test
./spin_test
./mutex1_test
./mutex2_test
./matrix_test



