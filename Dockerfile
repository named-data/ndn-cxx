FROM gcc:12-bookworm

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        dpkg-dev \
        libboost-chrono-dev \
        libboost-date-time-dev \
        libboost-dev \
        libboost-filesystem-dev \
        libboost-iostreams-dev \
        libboost-log-dev \
        libboost-program-options-dev \
        libboost-stacktrace-dev \
        libboost-thread-dev \
        pkg-config \
    && rm -rf /var/lib/apt/lists/*

COPY . /ndn-cxx

RUN cd /ndn-cxx \
    && ./waf configure --without-pch --prefix=/usr --sysconfdir=/etc --localstatedir=/var --disable-static --enable-shared \
    && ./waf \
    && ./waf install \
    && cd \
    && rm -rf /ndn-cxx
