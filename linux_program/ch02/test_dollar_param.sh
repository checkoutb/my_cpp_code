#!/bin/sh

unset foo
echo ${foo:-bar}

foo=fud
echo ${foo:-bar2}

foo=/usr/bin/X11/startx
echo ${foo#*/}
echo ${foo##*/}

bar=/usr/local/etc/local/networks
echo ${bar%local*}
echo ${bar%%local*}

exit 0
