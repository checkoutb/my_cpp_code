#!/bin/sh

touch file1
rm -f file2

if [ -f file1 ] && echo "1111" && [ -f file2 ] && echo "2222"
then
    echo "in if"
else
    echo "in else"
fi

exit 0
