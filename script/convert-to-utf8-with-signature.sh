#!/bin/bash

# echo $*

function get_type () {
    INFO=`file - < "$1" | cut -d: -f2`
    TYPE=`echo $INFO | cut -d, -f2`
    TYPE=`echo $TYPE | sed 's,^ *,,; s, *$,,'`
    echo "$TYPE"
}

function trim_string () {
    result=${1##}
    # result=${result%%}
    echo $result
}

function print_with_spaces () {
    echo "-$1-"
}

function test_trim() {
    STR="   Hello  World!       "
    print_with_spaces "$STR"
    STR=`echo $STR | sed 's,^ *,,; s, *$,,'`   # this line do the "Trim" action
    print_with_spaces "$STR"
    exit 0
}

function do_print_type () {
    echo "    $1"
}

function print_type () {
    FILE=$1
    TYPE=`echo $TYPE | sed 's,^ *,,; s, *$,,'`
    echo "$FILE type: -$TYPE-" 1>&2
    if [ "$TYPE" = "ASCII text" ]
    then
        do_print_type "ascii file"
    elif [ "$TYPE" = "UTF-8 Unicode (with BOM) text" ]
    then
        do_print_type "utf-8 with BOM"
    elif [ "$TYPE" = "UTF-8 Unicode text" ]
    then
        do_print_type "utf-8 without BOM"
    elif [ "$TYPE" = "ISO-8859 text" ]
    then
        do_print_type "GB2312"
    else
        do_print_type "========== unknown type: $TYPE"
    fi
}

# test_trim

if [ $# -lt 1 ]
then
    echo "Usage: $0 File [SourceEncoding]"
    exit
fi

File=$1
Src="UTF-8"
# if [ $# -ge 2 ]
# then
#     Src=$2
# fi


for File in $@
do
    echo "converting $File from $Src"
    uconv -f $Src -t UTF-8 --add-signature "$File" -o "$File.new"
    mv "$File.new" "$File"
done

exit


if [ $# -eq 0 ]
then
    echo usage $0 files ...
    exit 1
fi

for file in "$@"
do
    # echo "# Processing: $file" 1>&2
    if [ ! -f "$file" ]
    then
        echo Not a file: "$file" 1>&2
        exit 1
    fi
    TYPE=`get_type "$file"`
    # echo "$file type: -$TYPE-" 1>&2
    print_type "$file" "$TYPE"
    if echo "$TYPE" | grep -q '(with BOM)'
    then
        :
        # echo "# $file already has BOM, skipping." 1>&2
    else
        :
        # echo 1>&2
        # ( mv "${file}" "${file}"~ && uconv -f utf-8 -t utf-8 --add-signature < "${file}~" > "${file}" ) || ( echo Error processing "$file" 1>&2 ; exit 1)
    fi
done
