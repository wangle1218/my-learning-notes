#!/bin/bash

mkdir -p output
g++ -g -o recsys recsys.cpp recfunc.cpp matop.cpp str_util.cpp

if [[ $? != 0 ]]; then
    echo "error code:$?"
fi

cp recsys output
