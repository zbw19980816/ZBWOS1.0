#!/bin/bash

echo "user build $1 begin"

#编译zjpeg
make -C ./zjpeg/ $1

#编译zui
make -C ./app/win/zui $1

#编译app
make -C ./app/ $1

#编译APP
make $1

echo "user build $1 end"