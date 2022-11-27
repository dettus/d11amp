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

AWK_CMD?=awk
ECHO_CMD?=echo
MKDIR_CMD?=mkdir -p
SHA256_CMD?=sha256

TMP_DIR?=/tmp/d11amp/

#CHECKSUM="b4f4712166cc737023b41e5455cd1ce35149a7ec8d4a22123493771774a8a3ee" # unix line endings
CHECKSUM="28e22bf9721e3ac3b9cd59836b215b45e96e1bf31e2d0170896d4b2b501db4a4" # dos line endings

## the check works by checking the sha256 sum of the output. 
## Normally, d11amp starts by creating a directory under the users $HOME/.
## In it, numerous files are being written. One of them is the cc-0-10.txt
## file. Its sha256 sum is being checked. If all is well, the test succeeds.

check: all
	${MKDIR_CMD} ${TMP_DIR}
	./d11amp --dir=${TMP_DIR} --gui.theme.dumpdefault=${TMP_DIR}
	if [ "`cat ${TMP_DIR}/cc-0-10.txt | ${SHA256_CMD} | ${AWK_CMD} -F' ' '{ print $$1; }' - `" = ${CHECKSUM} ]       ; then ${ECHO_CMD} "$@ OK" ; else ${ECHO_CMD} "$@ failed" ; exit 1 ; fi

check-clean:
	rm -rf ${TMP_DIR}
