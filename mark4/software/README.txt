*** d11amp
*** dettus@dettus.net
*** (C)opyright 2022
*********************


Hello! Thank you for choosing d11amp. It is a frontend to libmpg123, designed
to handle themes from WinAMP. This release is usable.


0. COMPILATION
==============
Make sure the following libraries are installed on your system:
- gdk-pixbuf 2.0
- gtk4
- libmpg123
- libzip
- portaudio

Afterwards you can run

% make

and start it with

% ./d11amp

It will create a directory in the users home directory ~/.d11amp to store
its preferences, theme and caching data.


1. USER INTERFACE
=================
You can load one file. You can play it, you can repeat it. Clicking on "D"
cycles through the scaling factors. 


2. COMMAND LINE OPTIONS
=======================
Type in ./d11amp --help to get a list of all the command line options.
It is possible to load playlists in the .m3u format, for example, by typing

% ./d11amp --playlist.m3u=mysongs.m3u

The location of the caching data can be controlled with the command line
option --dir

% ./d11amp --dir=/tmp/d11amp

More command line options are described in the help screen

% ./d11amp --help


3. POST-COMPILATION CHECKS
==========================
To check if the compilation was sucessful, d11amp offers the possibility of
a "make check" target. This check is meant for numerous operating systems, 
where the name and location of tools vary. Hence, the name and location of
the tools can be customized via environment variables. 

For example, the command for calculating the SHA256 checksum is different
on OpenBSD and Linux. So on those operating systems, the command to run the
checks looks as followed:

OpenBSD: make SHA256_CMD=sha256     check
Linux:   make SHA256_CMD=sha256_sum check

The way the check works is by writing some files to /tmp and checking their
SHA256 sum. The location of the TMP_DIR can also be customized by using the
TMP_DIR=/somewhereelse/ variable.


4. KNOWN ISSUES
===============
- Not all the buttons work (yet)
- The windows are not sticky/magnetic
- The files stored in the directory ~/.d11amp are not parsed robustly. If
  d11amp behaves weird, it is probably a good idea to erase the directory
  and start over again.

-------------------------------------------------------------------------------
Thomas Dettbarn (dettus@dettus.net)

