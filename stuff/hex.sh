#!/bin/sh

hex()
{
  filename=$1
  start=$2 
  end=$3
  head -c $end $filename | tail -c $(($end - $start)) | hexdump -C 
}

if [ -z ${1+x} ] || [ -z ${2+x} ] ||  [ -z ${3+x} ]
then 
  echo "Arguments are 'filename', 'start byte', 'end byte'" 
else 
  hex $1 $2 $3
fi
