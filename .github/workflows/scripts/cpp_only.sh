#!/bin/bash

$CXX --version

mkdir debug_test && cd "$_"
cmake -GNinja -DNETWORKIT_BUILD_TESTS=ON -DNETWORKIT_MONOLITH=$MONOLITH -DNETWORKIT_CXX_STANDARD=$CXX_STANDARD -DNETWORKIT_WARNINGS=ON -DCMAKE_BUILD_TYPE=Debug ..
ninja

ctest -V