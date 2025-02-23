#!/bin/sh

trap 'rm -f /tmp/my_tmp_$$' INT
echo creating file /tmp/my_tmp_$$
date > /tmp/my_tmp_$$

echo "press ctrl+c to interrupt...."
while [ -f /tmp/my_tmp_$$ ]; do
    echo File exists
    sleep 1
done

echo file no longer exists

trap INT
echo creating file /tmp/my_tmp_$$
date > /tmp/my_tmp_$$

echo "press ctrl+c to interrupt ..."
while [ -f /tmp/my_tmp_$$ ]; do
    echo file exists
    sleep 1
done

echo we never get there

exit 0


