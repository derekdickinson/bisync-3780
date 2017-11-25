# Bisync 3780

This is an old DOS program that used UDS' SyncUp Modems to communicate with 
IBM [1] mainframes. It makes a PC look like a [_IBM 3780 Data Communication 
Terminal_](https://en.wikipedia.org/wiki/IBM_2780/3780).

I wrote this in the late eighties and packaged and sold it to accounting 
companies for filing taxes electronically to the IRS.

Not sure if it is useful anymore, but thought I should post it here just in 
case.

Some peculiarities that may make it useful as example code:

  * It has an extensive scripting language. DOS batch files of the day 
    were not great. This allowed easier customization.
  * There is more assembly language than you would expect. In the eighties,
    4.77MHz PCs were common. Most of the display routines are direct memory 
    writes in assembly for performance reasons.
  * Some of the configuration could be embedded in the executable itself via
    a utility (packinst.exe). 

The manual is setup for Xerox Ventura Publisher [2]. Fortunately, Ventura uses 
a decorated file so the manual can be read fairly easily with a standard text 
editor. The text files are of the form manual/read?.txt.

The file [manread.txt](manual/docs/manread.txt) has much of the manual's information 
without the Ventura markup. The file [readme.txt](d3b/readme.txt) has a list 
of the files and what they do. 

Feel free to email if you have a use for this. Will make an effort to elucidate
the parts of the code you intend to use.

[1] IBM is a registered trademark of International Business Machines Corporation
[2] Trademark Xerox or Corel? More [here](https://en.wikipedia.org/wiki/Corel_Ventura).

Reference:
GA27-3063-3
File No. 3780-09
Component Information for the IBM 3780 Data Communication Terminal

I have a hard copy of this document. Strangely, it does not contain a copyright
message from IBM. I'm tempted to upload this somewhere, but want to be certain 
of the legality.
