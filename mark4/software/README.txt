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



3. KNOWN ISSUES
===============
- Not all the buttons work (yet)
- The windows are not sticky/magnetic
- The files stored in the directory ~/.d11amp are not parsed robustly. If
  d11amp behaves weird, it is probably a good idea to erase the directory
  and start over again.

-------------------------------------------------------------------------------
Thomas Dettbarn (dettus@dettus.net)

