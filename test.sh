#!/bin/bash

echo "test1"
./proj2 3 5 100 100 1000
./kontrola-vystupu.sh < proj2.out
echo "endtest1"

echo "test2"
./proj2 3 5 100 100 0
./kontrola-vystupu.sh < proj2.out
echo "endtest2"

echo "test3"
./proj2 30 50 100 100 1000
./kontrola-vystupu.sh < proj2.out
echo "endtest3"

echo "test4"
./proj2 30 50 100 100 0
./kontrola-vystupu.sh < proj2.out
echo "endtest4"

echo "test5"
./proj2 30 50 0 100 1000
./kontrola-vystupu.sh < proj2.out
echo "endtest5"

echo "test6"
./proj2 30 50 0 100 0
./kontrola-vystupu.sh < proj2.out
echo "endtest6"

echo "test7"
./proj2 0 1 0 100 100
./kontrola-vystupu.sh < proj2.out
echo "endtest7