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

# Build yyjson and the fuzz target.
# yyjson is a single-header / single-source library.

cd $SRC/yyjson

# Build yyjson as an object file (C source, compile with CXX driver).
$CC $CFLAGS -c src/yyjson.c -o yyjson.o

# Build fuzz target (C++ wrapper).
$CXX $CXXFLAGS -std=c++11 \
    $SRC/yyjson_fuzzer.cc \
    yyjson.o \
    -I src \
    $LIB_FUZZING_ENGINE \
    -o $OUT/yyjson_fuzzer
