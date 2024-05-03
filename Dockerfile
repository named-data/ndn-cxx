# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS build

RUN apt-get install -Uy --no-install-recommends \
        dpkg-dev \
        g++ \
        git \
        libboost-chrono-dev \
        libboost-dev \
        libboost-filesystem-dev \
        libboost-log-dev \
        libboost-program-options-dev \
        libboost-stacktrace-dev \
        libboost-thread-dev \
        libsqlite3-dev \
        libssl-dev \
        pkgconf \
        python3 \
    && apt-get distclean

ARG JOBS
ARG SOURCE_DATE_EPOCH
RUN --mount=rw,target=/src <<EOF
set -eux
cd /src
./waf configure \
    --prefix=/usr \
    --libdir=/usr/lib \
    --sysconfdir=/etc \
    --localstatedir=/var \
    --sharedstatedir=/var \
    --disable-static \
    --enable-shared
./waf build
./waf install
mkdir -p /deps/debian
touch /deps/debian/control
cd /deps
dpkg-shlibdeps --ignore-missing-info /usr/lib/libndn-cxx.so.* /usr/bin/ndnsec -O \
    | sed -n 's|^shlibs:Depends=||p' | sed 's| ([^)]*),\?||g' > ndn-cxx
EOF


FROM ubuntu:24.04 AS runtime

RUN --mount=from=build,source=/deps,target=/deps \
    apt-get install -Uy --no-install-recommends $(cat /deps/ndn-cxx) \
    && apt-get distclean

RUN --mount=from=build,source=/usr,target=/build \
    cp -av /build/lib/libndn-cxx.so.* /usr/lib/ \
    && cp -av /build/bin/ndnsec* /usr/bin/
