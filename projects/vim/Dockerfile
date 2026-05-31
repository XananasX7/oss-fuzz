# Copyright 2025 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License")

FROM gcr.io/oss-fuzz-base/base-builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    libncurses5-dev libncursesw5-dev libacl1-dev libgpm-dev \
    python3-dev lua5.3 liblua5.3-dev perl libperl-dev \
    libxt-dev autoconf

RUN git clone --depth=1 https://github.com/vim/vim

WORKDIR vim
COPY build.sh fuzz_regexp.c fuzz_vimscript.c $SRC/
