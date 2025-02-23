#!/bin/sh

hi="hello"
echo $hi
echo "the program $0 is now running"
echo "the second parameter was $2"
echo "the first parameter was $1"
echo "the parameter list was $*"
echo "user's home dir is $HOME"

echo "please enter a new greeting"
read hi

echo $hi
echo "script is end"

exit 0

