#!/bin/sh

INSTALL_DIR=/home/qing.guojq/nginx
ROOT=$(cd "$(dirname "$0")"; pwd)

cd $ROOT/deps
unzip pcre-8.12.zip
tar zxvf nginx-1.0.6.tar.gz
cd nginx-1.0.6
 ./configure --add-module=$ROOT --with-pcre=../pcre-8.12 --prefix=$INSTALL_DIR
make && make install
cp conf/* $INSTALL_DIR/conf 
