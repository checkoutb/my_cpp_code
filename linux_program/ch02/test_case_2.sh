#!/bin/sh

echo "is it morning?"
read t2

case "$t2" in
    yes|y|YES)  echo "Good Morning";;
    n*|N*)  echo "Good afternoon";;
    *)  echo "not recognized";;
esac

exit 0
