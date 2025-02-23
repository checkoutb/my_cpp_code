#!/bin/sh

yesOrNo() {
    echo "is your name $* ?"
    while true
    do
        echo -n "Enter yes or no: "
        read x
        case "$x" in 
            y | yes )   return 0;;
            n | no )    return 1;;
            * )         echo "yes or no!!!"
        esac
    done
}

echo "Original parameter are $*"

if yesOrNo "$1"
then
    echo "hi, $1, nice name"
else
    echo "never mind"
fi
exit 0

