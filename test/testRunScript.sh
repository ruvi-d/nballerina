#!/bin/bash
# echo "Input file name is $1"
FILE=$(basename $1)
filename="${FILE%.*}"

if [ -z "$JAVA_HOME" ]
then
  echo "\$JAVA_HOME not set."
  exit 1
fi

ballerina build --dump-bir-file=$filename-bir-dump $1 1>out.log 2>err.log

if [ -s ./err.log ]
then
  echo "JBallerina error . Check err.log"
  exit 1
fi

$2 $filename-bir-dump  2>err.log

if [ -s ./err.log ]
then
  echo "nballerinacc error. Check err.log"
  exit 1
fi

clang -O0 -o $filename.out $filename-bir-dump.ll 2>err.log

./$filename.out
echo RETVAL=$?

rm $filename-bir-dump.ll $filename-bir-dump $filename.out $filename.jar
