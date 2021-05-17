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


function makeRelativeDyLibs() {
    local location=${1}
    local dylib=""
    local absPath=""
    if pushd ${location} > /dev/null; then
        absPath=$(pwd)
        for dylib in $(ls *.dylib*)
        do
            makeRelativeDyLib ${absPath}/${dylib}
        done
        popd > /dev/null
    fi
}

function makeRelativeDyLib() {
    local dylib=${1}
    local dependency
    echo "Make ${dylib} use relative dependencies"

    # /usr/local用来过滤需要修改的库名称，可以据实际情况修改
    for dependency in $(otool -L ${dylib} | grep '^\s\+\/usr/local/.*(compatibility version.*)' | sed -e 's/ (compatibility version.*)//g')
    do
        install_name_tool -change ${dependency} @executable_path/../Frameworks/$(basename ${dependency}) ${dylib}
    done
}

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
#demofun $path

makeRelativeDyLibs $path