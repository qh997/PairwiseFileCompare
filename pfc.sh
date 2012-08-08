#!/bin/bash

function init {
    ARGS=$(getopt -o hd --long debug --long hash -n "$0" -- "$@")
    if [ $? != 0 ]; then
        exit 1
    fi
    eval set -- "$ARGS"

    while true ; do
        case "$1" in
            -h|--hash) HASH=1; shift;;
            -d) : ${DEBUG:=1}; shift;;
            --debug) DEBUG=$2; shift 2;;
            --) shift; break;;
            *) echo "Internal error!"; exit 1;;
        esac
    done

    : ${DEBUG:=0}

    paths[1]="$1"
    for ((i=1; i<=${#@}; i++)); do
        paths[$i]=$(eval echo -n \$$i)
    done

    : ${paths[1]:=$(pwd)}
}

function debug {
    if [[ -z $@ ]]; then
        return 0
    elif [[ $# == 2 ]]; then
        local debug_level=$1
        shift
    fi
    : ${debug_level:=1}

    if [ $debug_level -le $DEBUG ]; then
        local print_time=$(date +%H:%M:%S.%N | sed 's/[[:digit:]]\{6\}$//')
        echo "[DebugMessage $print_time $debug_level] $1"
    fi
}

OLD_IFS=$IFS
IFS=$(echo -en "\n\b")

init $@

declare -a files
file_count=0
for path in ${paths[*]}; do
    debug 1 "path = $path"
    filelist=$(find $path -type f)
    for file in $filelist; do
        debug 2 "file = $file"
        files[$((file_count++))]=$file
    done
done

debug 1 "file total : ${#files[*]}"

declare -a already_compare
ac_index=0
for ((i=0; i<${#files[*]}; i++)); do
    left_file=${files[$i]}
    lf_hash=$(cat "$left_file" 2>/dev/null | sha1sum | sed -n -e '2d' -e 's/ .*$//p')

    frist=
    for ((j=$i+1; j<${#files[*]}; j++)); do
        right_file=${files[$j]}
        skip=
        for alc in ${already_compare[*]}; do
            if ((alc == j)); then
                skip=1
                break
            fi
        done

        if [[ -z $skip ]]; then
            rf_hash=$(cat "$right_file" 2>/dev/null | sha1sum | sed -n -e '2d' -e 's/ .*$//p')
            debug 3 "$left_file <=> $right_file"

            if [[ $lf_hash == $rf_hash ]]; then
                if [[ -z $frist ]]; then
                    if [[ -n $HASH ]]; then
                        echo $lf_hash
                    fi

                    echo $left_file
                    frist=1
                fi
                echo $right_file

                already_compare[$((ac_index++))]=$j
            fi
        else
            debug 2 "Skip $right_file"
        fi
    done

    if [[ -n $frist ]]; then
        echo
    fi
done

IFS=$OLD_IFS
