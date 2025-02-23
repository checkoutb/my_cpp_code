#!/bin/sh

echo "is it morning?"
read t2

case "$t2" in
    yes | y | YES )
        echo "Good morning"
        echo "Up bright"
        ;;
    [nN]*)
        echo "Good afternoon"
        ;;
    *)
        echo "no recognized"
        echo "plz check"
        exit 1
        ;;
esac

exit 0

