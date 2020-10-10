#!/bin/bash

if [ $# -lt 1 ]
then
    echo "Usage: $0 path-of-files-to-convert"
    exit
fi

./find-file-with-encoding.sh -e=UTF-8Unicodetext -t=$1 | xargs ./convert-to-utf8-with-signature.sh
