#!/bin/sh

# for foo in bar fud 43
for foo in $(ls t*.sh)
do
    echo -n $foo
done
exit 0

