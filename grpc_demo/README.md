# grpc_demo

### grpc dependencies

# install cmake > 3.13

```
sudo apt-get install -y g++ make libssl-dev

cd third_party/cmake-3.17.2
 
./configure
 
sudo make && make install

cmake --version 
```

# install grpc

```
$ sudo apt-get install -y build-essential autoconf libtool pkg-config

install 
  cd third_party/grpc-1.41.0
  cd cmake
  mkdir build
  cd build
  cmake -DgRPC_INSTALL=ON \
        -DgRPC_BUILD_TESTS=OFF \
        -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
        ../..
  make -j4
  sudo make install

```

# install protobuf  注意：要是没有XXtest文件时，手动创建一下

```
$ sudo apt-get install -y autoconf automake libtool curl make g++ unzip

install
    cd third_party/grpc-1.41.0/third_party/protobuf
    sudo ./autogen.sh
    sudo ./configure --prefix=/usr/local
    sudo make -j4
    sudo make check
    sudo make install
    sudo ldconfig # refresh shared library cache.
```

### grpc use

```
cd src
mkdir build
cd build
cmake ..
make
开启服务端
./greeter_server
打开另外的terminal 开启客户端 
./greeter_client

```
