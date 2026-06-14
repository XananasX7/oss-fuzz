#!/bin/bash -eu

# Copyright 2026 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################

# Build simdjson using its amalgamated single-file distribution.

cd $SRC/simdjson

# Generate the amalgamated single-file header + source if not present.
if [ ! -f singleheader/simdjson.h ]; then
    cmake -S . -B build_amalgamate \
        -DSIMDJSON_JUST_LIBRARY=ON \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_CXX_FLAGS="$CXXFLAGS"
    cmake --build build_amalgamate --target amalgamate 2>/dev/null || true
fi

# The singleheader directory should now contain simdjson.h / simdjson.cpp.
SRC_DIR=$SRC/simdjson

# Compile the simdjson amalgamate source.
$CXX $CXXFLAGS -std=c++17 \
    -c singleheader/simdjson.cpp \
    -I singleheader \
    -o simdjson.o

# Build fuzz target.
$CXX $CXXFLAGS -std=c++17 \
    $SRC/simdjson_fuzzer.cc \
    simdjson.o \
    -I singleheader \
    $LIB_FUZZING_ENGINE \
    -o $OUT/simdjson_fuzzer
