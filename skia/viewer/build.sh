#!/bin/bash

# dir=$(pwd)

# cd ../renderer
# ./build.sh $@

# cd $dir

cd build

OPTION=$1

if [ "$OPTION" = 'help' ]; then
    echo build.sh - build debug library
    echo build.sh clean - clean the build
    echo build.sh release - build release library
elif [ "$OPTION" = "clean" ]; then
    echo Cleaning project ...
    # TODO: fix premake5 clean to bubble the clean command to dependent projects
    premake5 gmake && make clean
elif [ "$OPTION" = "release" ]; then
    premake5 gmake && make config=release -j7
else
    premake5 gmake && make config=debug -j7
fi
