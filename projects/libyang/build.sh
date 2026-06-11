#!/bin/bash -eu
# Copyright 2021 Google LLC
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

git checkout devel

mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=OFF -DENABLE_TOOLS=OFF ..
make

static_pcre=($(find /src/pcre2 -name "libpcre2-8.a"))

# Original fuzzers + yang_parse_module (exists in upstream tests/fuzz/ but was not built)
# + lyd_parse_lyb_fuzzer (new: covers the LYB binary format, zero prior fuzzing coverage)
#   lyd_parse_lyb_fuzzer.c is kept in the oss-fuzz projects/libyang/ dir until
#   the upstream PR (CESNET/libyang) lands; the cp below stages it alongside.
cp $SRC/lyd_parse_lyb_fuzzer.c ../tests/fuzz/

for fuzzer in lyd_parse_mem_json lyd_parse_mem_xml lys_parse_mem yang_parse_module lyd_parse_lyb_fuzzer; do
  $CC $CFLAGS -c ../tests/fuzz/${fuzzer}.c -I./libyang -I./compat -I../src
  $CXX $CXXFLAGS $LIB_FUZZING_ENGINE ${fuzzer}.o -o $OUT/${fuzzer} \
    ./libyang.a ${static_pcre}
done

# Build test
mkdir $SRC/libyang/build-test
cd $SRC/libyang/build-test
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
