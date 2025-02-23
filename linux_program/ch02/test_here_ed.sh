#!/bin/sh

ed a_text_file << !ZXC!
3
d
.,\$s/is/was/
w
q
!ZXC!

exit 0
