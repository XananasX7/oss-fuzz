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

# Build aklomp/base64 and the fuzz target.

cd $SRC/base64

# Build via cmake (preferred; selects SSSE3/AVX2 codecs automatically).
cmake -S . -B build \
    -DCMAKE_C_COMPILER="$CC" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DCMAKE_C_FLAGS="$CFLAGS" \
    -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBASE64_BUILD_TESTS=OFF

cmake --build build --parallel $(nproc)

# Build fuzz target.
$CXX $CXXFLAGS -std=c++11 \
    $SRC/base64_fuzzer.cc \
    -I include \
    build/lib/libbase64.a \
    $LIB_FUZZING_ENGINE \
    -o $OUT/base64_fuzzer
