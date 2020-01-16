FROM nvidia/cuda:latest

RUN apt-get update
RUN apt-get install -y --no-install-recommends build-essential wget unzip
RUN apt-get install -y --no-install-recommends cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

RUN \
    mkdir build \
    && cd build \
    && wget -O opencv-4.2.0.zip https://github.com/opencv/opencv/archive/4.2.0.zip \
    && wget -O opencv_contrib-4.2.0.zip https://github.com/opencv/opencv_contrib/archive/4.2.0.zip \
    && unzip opencv-4.2.0.zip \
    && unzip opencv_contrib-4.2.0.zip

RUN cd build/opencv-4.2.0 \
    && mkdir build \
    && cd build \
    && cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=./../../opencv_contrib-4.2.0/modules -D WITH_JAVA=OFF -D WITH_PYTHON=OFF -D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF -D BUILD_EXAMPLES=OFF .. \
    && make

RUN git clone https://github.com/durswd/vfs.git

RUN cd build/opencv-4.2.0/build \
    && make install

RUN cd vfs \
    && mkdir build \
    && cd build \
    && cmake -D CMAKE_BUILD_TYPE=Release -D OpenCV_DIR=/usr/local .. \
    && make