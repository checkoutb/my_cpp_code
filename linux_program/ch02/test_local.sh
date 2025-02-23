#!/bin/sh

txt="global v"

foo(){
    local txt="local v"
    echo "function foo is executing"
    echo $txt
}

echo "start"
echo $txt

foo

echo "en"
echo $txt

exit 0
