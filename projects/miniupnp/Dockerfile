FROM gcr.io/oss-fuzz-base/base-builder

RUN apt-get update && apt-get install -y --no-install-recommends cmake

RUN git clone --depth 1 https://github.com/miniupnp/miniupnp.git

WORKDIR miniupnp/miniupnpc
COPY build.sh $SRC/
COPY *.c $SRC/
