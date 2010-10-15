
Display Rotation for the Mac: fb-rotate
=======================================

A Unix utility able to rotate the display on any Mac, including the internal display on Apple notebooks, and able to switch the primary display, the one with the menu bar, back and forth between displays.


Compiling fb-rotate
-------------------

Assuming you have Xcode installed, you can compile the C-code yourself on any Mac OS from 10.3 to 10.6. 

In the Terminal app, after you've changed the current directory to the one `fb-rotate.c` is stored, using

     cd <path to the directory>

then,

     gcc -Wall -o fb-rotate fb-rotate.c -framework IOKit -framework ApplicationServices

will compile the utility.


Use of fb-rotate
----------------

     fb-rotate -l

will list the display id's, e.g. in Terminal,
 
     $ ./fb-rotate -l
     Display ID       Resolution
     0x19156030       1280x800                  [main display]
     0x76405c2d       1344x1008 


     fb-rotate -i

will list the display id's with other information, e.g.

     $ ./fb-rotate -i
     #  Display_ID  Resolution  ____Display_Bounds____  Rotation    
     0  0x19156030  1280x800       0     0  1280   800      0    [main][internal]
     1  0x76405c2d  1344x1008   1280     0  2624  1008      0    
     Mouse Cursor Position:  (   528 ,   409 )

(Unlike the file: `com.apple.windowserver.plist`, fb-rotate's information is always accurate and current.)


     fb-rotate -d 0 -r 180

will rotate the main display 180 degrees, e.g.

     $ ./fb-rotate -d 0 -r 180
     $ ./fb-rotate -i
     #  Display_ID  Resolution  ____Display_Bounds____  Rotation
     0  0x19156030  1280x800       0     0  1280   800    180    [main][internal]
     1  0x76405c2d  1344x1008   1280     0  2624  1008      0    
     Mouse Cursor Position:  (  1047 ,   359 )

(You can rotate to the 0, 90 and 270 degree orientations as well.)


     fb-rotate -d <display ID> -r 0

will rotate the display with the indicated ID back to the standard orientation, e.g.

     $ ./fb-rotate -d 0x19156030 -r 0
     $ ./fb-rotate -i
     #  Display_ID  Resolution  ____Display_Bounds____  Rotation
     0  0x19156030  1280x800       0     0  1280   800      0    [main][internal]
     1  0x76405c2d  1344x1008   1280     0  2624  1008      0    
     Mouse Cursor Position:  (   226 ,   103 )

(Again, you can also rotate to the 90, 180 and 270 degree orientations.)


Finally,

     fb-rotate -d <display ID> -m

will set the display with the indicated ID to be the primary (main) display that has the menu bar, e.g.

     $ ./fb-rotate -d 0x76405c2d -m
     $ ./fb-rotate -i
     #  Display_ID  Resolution  ____Display_Bounds____  Rotation
     1  0x76405c2d  1344x1008      0     0  1344  1008      0    [main]
     0  0x19156030  1280x800   -1280     0     0   800      0    [internal]
     Mouse Cursor Position:  (  1122 ,   438 )



Downloads
---------

[A binary version of fb-rotate][fb-rotate] is available at Modbookish, a forum focused on the [Axiotron Modbook][Modbook].


Caveats
-------

Warning: Some white MacBooks, namely those using Intel's integrated graphics, have difficulty rotating to the 90º or 270º orientations and the resulting display may be difficult to use. 


Credits and License 
-------------------

The original code for fb-rotate comes from a programming example in
the book **Mac OS X Internals: A Systems Approach** by Amit Singh (© 2006). The source is made available under the GNU General Public License (GPL). For more information, see the book's associated web site: [http://osxbook.com][osxbook]

Changes were made by [Eric Nitardy][ericn] (© 2010) which have to be made available under the same license.

[osxbook]: http://osxbook.com
[ericn]: http://modbookish.lefora.com/members/ericn/
[fb-rotate]: http://modbookish.lefora.com/2010/06/29/a-unix-utility-to-change-the-primary-display-on-os/
[Modbook]: http://www.axiotron.com/index.php?id=modbook
