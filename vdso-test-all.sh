#!/bin/sh

: ${vdsotest:=vdsotest}

opts="$@"

for api in $($vdsotest list-apis)
do
    for testtype in verify abi
    do
        echo -n "$vdsotest $opts $api $testtype"
        $vdsotest $opts $api $testtype
	echo ": $?"
    done
done
