# c-descriptor

## prepare

I use system installed libsecp256k1(built with `--enable-module-recovery`).

```console
$ git clone https://github.com/bitcoin-core/secp256k1.git
$ cd secp256k1
$ git checkout -b v0.6.0 refs/tags/v0.6.0
$ ./autogen.sh
$ ./configure --enable-module-recovery
$ make
$ sudo make install
$ cd ..
```

Build libwally-core with `tr()` support.

```console
$ git clone https://github.com/ElementsProject/libwally-core.git
$ cd libwally-core
$ git checkout 7d2b2cdc2132015b37aadc41b140c5537aceeb6a

.$ /tools/autogen.sh
$ ./configure --prefix $HOME/.local --enable-minimal --disable-elements --enable-standard-secp --with-system-secp256k1 --disable-shared
$ make
$ make install
```

Install [cpprestsdk](https://github.com/microsoft/cpprestsdk) to access Esplora API

```console
$ sudo apt-get install libcpprest-dev
```

## Build and Run

```console
$ git clone https://github.com/hirokuma/cpp-descriptor.git
$ cd cpp-descriptor
$ make
$ ./tst
```
