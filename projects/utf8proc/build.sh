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

# Build utf8proc and the fuzz target.

cd $SRC/utf8proc

# Build in strict C99 mode matching the library's own Makefile.
$CC $CFLAGS -c utf8proc.c -o utf8proc.o

# Build fuzz target (C++ wrapper links against the object file).
$CXX $CXXFLAGS -std=c++11 \
    $SRC/utf8proc_fuzzer.cc \
    utf8proc.o \
    -I . \
    $LIB_FUZZING_ENGINE \
    -o $OUT/utf8proc_fuzzer
