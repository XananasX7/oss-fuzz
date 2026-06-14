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

# Build libzstd and the fuzz target.

cd $SRC/zstd

# Build libzstd static library.
make -C lib libzstd.a \
    CC="$CC" \
    CFLAGS="$CFLAGS" \
    -j$(nproc)

# Build fuzz target.
$CXX $CXXFLAGS -std=c++11 \
    $SRC/zstd_fuzzer.cc \
    -I lib \
    lib/libzstd.a \
    $LIB_FUZZING_ENGINE \
    -o $OUT/zstd_fuzzer
