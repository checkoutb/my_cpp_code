#!/bin/sh

echo "is it morning? yes or no"
read t2

case "$t2" in 
    yes)    echo "Good morning";;
    no )    echo "Good afternoon";;
    y  )    echo "Good morning";;
    n  )    echo "Good afternoon";;
    *  )    echo "Sorry, answter not recognized";;
esac

exit 0
