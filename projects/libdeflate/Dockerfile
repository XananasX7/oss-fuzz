FROM gcr.io/oss-fuzz-base/base-builder

RUN apt-get update && apt-get install -y --no-install-recommends cmake

RUN git clone --depth 1 https://github.com/ebiggers/libdeflate.git

WORKDIR libdeflate
COPY build.sh $SRC/
COPY *.c $SRC/
