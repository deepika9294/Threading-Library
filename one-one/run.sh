
make matrix_test
make spin_test
make mutex1_test
make mutex2_test

./matrix_test
echo "SPINLOCK TEST WITH RACE PROBLEM\n";
./spin_test
echo "MUTEX1 TEST \n";
./mutex1_test
echo "MUTEX2 TEST \n";
./mutex2_test

