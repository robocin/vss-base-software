FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    build-essential \
    libssl-dev \
    libffi-dev \
    python3-setuptools \
    python3-venv \
    python3-wheel \
    git \
    wget \
    unzip

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends libxkbcommon-dev \
    libxkbcommon-x11-dev libxcb-icccm4-dev libxcb-image0-dev \
    libxcb-keysyms1-dev libxcb-render-util0-dev libxcb-shape0-dev

RUN git clone https://github.com/robocin/soccer-common.git /soccer-common

WORKDIR /soccer-common

RUN git checkout e8aecd4

RUN git submodule update --init

WORKDIR /soccer-common/scripts

RUN python3 setup.py --essentials

RUN python3 setup.py --install libtorch

COPY . /vss-base-software

WORKDIR /vss-base-software

RUN [ -d ./build ] && rm -rf ./build || echo "Directory build does not exist"

RUN mkdir build

WORKDIR /vss-base-software/build

RUN cmake ..

RUN make -j8

WORKDIR /vss-base-software/bin

RUN rm -rf /vss-base-software/.git

RUN rm -rf /vss-base-software/src

CMD ["./vss-base-software", "-platform", "minimal", "-no-gui", "-running"]
