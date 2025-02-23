#!/bin/sh

echo "is is morning? yes or no"
read t2

if [ "$t2" = "yes" ]; then
    echo "Good morning"
elif [ "$t2" = "no" ]; then
    echo "Good afternoon"
else
    echo "??? yes or no"
    exit 1
fi

exit 0
