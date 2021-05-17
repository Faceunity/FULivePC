#! /bin/bash

# install_name_tool -id  @executable_path/../Frameworks/libCNamaSDK.dylib libCNamaSDK.dylib

demofun(){
    for file in `ls $1 | grep dylib`
    do
        if test -f $file
        then
            echo "file:  $file"
            install_name_tool -id  @executable_path/../Frameworks/$file $file
        fi
    done
}

path="./"
demofun $path