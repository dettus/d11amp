#Copyright 2022, dettus@dettus.net
#
#Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:
#
#1. Redistributions of source code must retain the above copyright notice, this 
#   list of conditions and the following disclaimer.
#
#2. Redistributions in binary form must reproduce the above copyright notice, 
#   this list of conditions and the following disclaimer in the documentation 
#   and/or other materials provided with the distribution.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


### the following lines are the post-compilation test. This is a formality on some operating systems ###########################

SHA256_CMD?=sha256
ECHO_CMD?=echo
AWK_CMD?=awk
MKDIR_CMD?=mkdir -p
TMP_DIR?=/tmp/d11amp/

CHECKSUM="b4f4712166cc737023b41e5455cd1ce35149a7ec8d4a22123493771774a8a3ee"

## the check works by checking the sha256 sum of the output. 
## It is unpacking the default theme and verifies the SHA256 sum of one file.

check:
	${MKDIR_CMD} ${TMP_DIR}
	./d11amp --gui.theme.dumpdefault=${TMP_DIR}
	if [ "`${SHA256_CMD} ${TMP_DIR}/cc-0-10.txt | ${AWK_CMD} -F' ' '{ print $$1; }' - `" = ${CHECKSUM} ]       ; then ${ECHO_CMD} "$@ OK" ; else ${ECHO_CMD} "$@ failed" ; exit 1 ; fi

