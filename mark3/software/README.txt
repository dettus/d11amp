*** d11amp
*** (C)opyright 2022
*** dettus@dettus.net
*********************

Hello! Thank you for choosing d11amp. It is a frontend to libmpg123, designed
to handle themes from WinAMP. This release is not yet stable, though it usable.


0. COMPILATION
==============
Make sure the following libraries are installed on your system:
- gdk-pixbuf 2.0
- gtk4
- libmpg123
- portaudio

Afterwards you can run

% make


1. FIRST RUN (IMPORTANT)
========================
This version is not able to handle themes from .wsz files or has a default
theme. Therefore, it is IMPERATIVE that you download one from a website, 
(for example https://skins.webamp.org/) unpack it, and load the theme from
that directory.

Since the .wsz files are simple ZIP files, albeit flat ones, they can be
unpacked as followed:

% mkdir base-2.91
% cd base-2.91
% unzip ../base-2.91.wsz
% cd ..
% ./d11amp --gui.theme.dir=base-2.91/

The directory should be the one with all the .BMP files. 



2. USER INTERFACE
=================
You can load one file. You can play it, you can repeat it. Clicking on "D" 
cycles through the scaling factors. At the moment, the windows can be resized
through means of your favourite window manager. But that will be removed.
Clicking on "V" cycles through the Visualizations. Clicking on "I" shows the
license.
Feel free to play around with it.
Clicking on the Equalizer and Playlist buttons will open the windows.


3. COMPILE TIME PARAMETERS
==========================
The only way to properly scale ALL THE WINDOWS is by finding the SCALEFACTOR=2
line in the Makefile and change it. 


4. KNOWN ISSUES
===============
- Windows can be resized
- Lots of GTK warnings on the console
- Cannot deal with incomplete themes
- Windows tend to freeze
- Closing the main window will not terminate the application
- Opening a file without opening the playlist window first will crash the
  player at the end of the song.


-------------------------------------------------------------------------------
Thomas Dettbarn (dettus@dettus.net)
