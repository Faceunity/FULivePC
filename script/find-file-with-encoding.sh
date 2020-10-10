#!/bin/bash

# ENCODING=UTF-8Unicodetext
TARGET=.
usage(){
    echo "find all files with specified encoding in a directory and all subdirectories"
    echo ""
    echo "$0"
    echo "  -h --help           show this message and exit"
    echo "  -e --encoding       specified encoding -e=$ENCODING"
    echo "  -l --list-encodings list all encodings and one file can find currently"
    echo "  -t --target-dir     target directory to check -t=$TARGET"
    echo ""
}

parse_arg() {
    while [ "$1" != "" ]; do
        PARAM=`echo $1 | awk -F= '{print $1}'`
        VALUE=`echo $1 | awk -F= '{print $2}'`
        case $PARAM in
            -h | --help)
                usage
                exit
                ;;
            -e | --encoding)
                ENCODING=$VALUE
                # echo "got ENCODING=$ENCODING"
                ;;
            -l | --list-encodings)
                LIST="1"
                # echo "got LIST=$LIST"
                ;;
            -t | --target-dir)
                TARGET=$VALUE
                # echo "got TARGET=$TARGET"
                ;;
            *)
                echo "ERROR: unknown parameter \"$PARAM\""
                usage
                exit 1
                ;;
        esac
        shift
    done
    echo $*
}

get_type () {
    INFO=`file - < "$1" | cut -d: -f2`
    TYPE=`echo $INFO | cut -d, -f2`
    TYPE=`echo $TYPE | sed 's,^ *,,; s, *$,,'`
    TYPE=`echo ${INFO//[[:space:]]/}`
    echo "$TYPE"
}

declare -a EMap

# BOMs:
# UTF-8Unicode(withBOM)text
# UTF-8Unicodetext
# ASCIItext
# ISO-8859text

find() {
    for file in $1/*
    do
        if [ -d "$file" ]
        then
            if [ -z "$(ls -A $file)" ]
            then
                :
            else
                if (( $# > 1 ))
                then
                    find "$file" "$2"
                else
                    find "$file"
                fi
            fi
        else
            # echo "$file"
            TYPE=`get_type "$file"`
            # echo "$file : $TYPE"
            EMap[$TYPE]="$file"
            if [ -z "$2" ] || [[ "$TYPE" != *$2* ]]
            then
                :
            else
                echo "$file"
            fi
        fi
    done
}

main(){
    parse_arg $*

    find $TARGET $ENCODING

    if [ -z "$LIST" ]
    then
        :
    else
        echo ""
        echo "All encodings:"
        for i in "${!EMap[@]}"
        do
        echo "$i:    ${EMap[$i]}"
        done
    fi
}

main $*
