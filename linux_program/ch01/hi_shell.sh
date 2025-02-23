#!/bin/sh

for file in *
do
	if grep -q include $file
	then
		echo $file
	fi
done

exit 0

