#!/bin/sh

echo "enter pwd"
read t2

while [ "$t2" != "pwd" ]; do
    echo "again."
    read t2
done
exit 0
