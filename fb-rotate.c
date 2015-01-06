// fb-rotate.c
//
// Compile with: 
// gcc -Wall -o fb-rotate fb-rotate.c -framework IOKit -framework ApplicationServices
   
#include <getopt.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
   
#define PROGNAME "fb-rotate"
#define MAX_DISPLAYS 16
   
// kIOFBSetTransform comes from <IOKit/graphics/IOGraphicsTypesPrivate.h>
// in the source for the IOGraphics family


enum {
    kIOFBSetTransform = 0x00000400,
};
   
void
usage(void)
{
    fprintf(stderr, "usage: %s -l\n"
                    "       %s -i\n"
                    "       %s -d <display ID> -m\n"
                    "       %s -d <display ID> -r <0|90|180|270>\n"
	            "\n"
	            "-d 0 can be used for the <display ID> of the main monitor\n"
	            "-d 1 can be used for the <display ID> of the first non-main monitor\n",
                    PROGNAME, PROGNAME, PROGNAME, PROGNAME);
    exit(1);
}
   
void
listDisplays(void)
{
   CGDisplayErr      dErr;
    CGDisplayCount    displayCount, i;
    CGDirectDisplayID mainDisplay;
    CGDisplayCount    maxDisplays = MAX_DISPLAYS;
    CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
   
    mainDisplay = CGMainDisplayID();
   
    dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
    if (dErr != kCGErrorSuccess) {
        fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
        exit(1);
    }
   
    printf("Display ID       Resolution\n");
    for (i = 0; i < displayCount; i++) {
        CGDirectDisplayID dID = onlineDisplays[i];
        printf("%-16p %lux%lu %32s", dID,
               CGDisplayPixelsWide(dID), CGDisplayPixelsHigh(dID),
               (dID == mainDisplay) ? "[main display]\n" : "\n");
    }
   
    exit(0);
}
   
void
infoDisplays(void)
{
    CGDisplayErr      dErr;
    CGDisplayCount    displayCount, i;
    CGDirectDisplayID mainDisplay;
    CGDisplayCount    maxDisplays = MAX_DISPLAYS;
    CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
    
    CGEventRef ourEvent = CGEventCreate(NULL);
    CGPoint ourLoc = CGEventGetLocation(ourEvent);
    
    CFRelease(ourEvent);
    
    mainDisplay = CGMainDisplayID();
   
    dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
    if (dErr != kCGErrorSuccess) {
        fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
        exit(1);
    }
   
    printf("#  Display_ID  Resolution  ____Display_Bounds____  Rotation\n");
    for (i = 0; i < displayCount; i++) {
        CGDirectDisplayID dID = onlineDisplays[i];
        printf("%-2d %10p  %4lux%-4lu  %5.0f %5.0f %5.0f %5.0f    %3.0f    %s%s%s", 
               CGDisplayUnitNumber (dID), dID,
               CGDisplayPixelsWide(dID), CGDisplayPixelsHigh(dID),
               CGRectGetMinX (CGDisplayBounds (dID)),
               CGRectGetMinY (CGDisplayBounds (dID)),
               CGRectGetMaxX (CGDisplayBounds (dID)),
               CGRectGetMaxY (CGDisplayBounds (dID)),           
               CGDisplayRotation (dID),
               (CGDisplayIsActive (dID)) ? "" : "[inactive]",
               (dID == mainDisplay) ? "[main]" : "",
               (CGDisplayIsBuiltin (dID)) ? "[internal]\n" : "\n");
    }
    
    printf("Mouse Cursor Position:  ( %5.0f , %5.0f )\n",
               (float)ourLoc.x, (float)ourLoc.y);
   
    exit(0);
}

void
setMainDisplay(CGDirectDisplayID targetDisplay)
{
    int				   deltaX, deltaY, flag;
    CGDisplayErr       dErr;
    CGDisplayCount     displayCount, i;
    CGDirectDisplayID mainDisplay;
    CGDisplayCount     maxDisplays = MAX_DISPLAYS;
    CGDirectDisplayID  onlineDisplays[MAX_DISPLAYS]; 
	CGDisplayConfigRef config;

	mainDisplay = CGMainDisplayID();
	
	if (mainDisplay == targetDisplay) {
	exit(0);
	}
	
    dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
    if (dErr != kCGErrorSuccess) {
        fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
        exit(1);
    }
	
	flag = 0;
    for (i = 0; i < displayCount; i++) {
    	CGDirectDisplayID dID = onlineDisplays[i];
			if (dID == targetDisplay) { flag = 1; }
	}	
	if (flag == 0) {
        fprintf(stderr, "No such display ID: %10p.\n", targetDisplay);
        exit(1);
    }

	deltaX = -CGRectGetMinX (CGDisplayBounds (targetDisplay));
    deltaY = -CGRectGetMinY (CGDisplayBounds (targetDisplay));

    CGBeginDisplayConfiguration (&config);
    
    for (i = 0; i < displayCount; i++) {
        CGDirectDisplayID dID = onlineDisplays[i];
    
    CGConfigureDisplayOrigin (config, dID,
    	CGRectGetMinX (CGDisplayBounds (dID)) + deltaX,
    	CGRectGetMinY (CGDisplayBounds (dID)) + deltaY );
	}

    CGCompleteDisplayConfiguration (config, kCGConfigureForSession);
   
   
    exit(0);
}

CGDirectDisplayID
nonMainID(void) {
   // returns the ID of the first active monitor that is not main or 0 if only one monitor;
    CGDisplayErr      dErr;
    CGDisplayCount    displayCount, i;
    CGDisplayCount    maxDisplays = MAX_DISPLAYS;
    CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
    CGDirectDisplayID mainID = CGMainDisplayID();
    CGDirectDisplayID fallbackID = 0;
    dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
    if (dErr != kCGErrorSuccess) {
        fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
        exit(1);
    }
    for (i = 0; i < displayCount; i++) {
        CGDirectDisplayID dID = onlineDisplays[i];
	if ((dID != mainID) && (CGDisplayIsActive (dID))) {
	  return dID;
	}
    }
    return fallbackID;
}

IOOptionBits
angle2options(long angle)
{
    static IOOptionBits anglebits[] = {
               (kIOFBSetTransform | (kIOScaleRotate0)   << 16),
               (kIOFBSetTransform | (kIOScaleRotate90)  << 16),
               (kIOFBSetTransform | (kIOScaleRotate180) << 16),
               (kIOFBSetTransform | (kIOScaleRotate270) << 16)
           };
                                 
    if ((angle % 90) != 0) // Map arbitrary angles to a rotation reset
        return anglebits[0];
   
    return anglebits[(angle / 90) % 4];
}
   
int
main(int argc, char **argv)
{
    int  i;
    long angle = 0;
   
    io_service_t      service;
    CGDisplayErr      dErr;
    CGDirectDisplayID targetDisplay = 0;
    IOOptionBits      options;
   
    while ((i = getopt(argc, argv, "d:limr:")) != -1) {
        switch (i) {
        case 'd':
	  targetDisplay = (CGDirectDisplayID)strtoul(optarg, NULL, 16);
          if (targetDisplay == 0)
              targetDisplay = CGMainDisplayID();
	  if (targetDisplay == 1) {
              targetDisplay = nonMainID();
              if (targetDisplay == 0) {
                  fprintf(stderr, "Could not find a non-main monitor.\n");
                  exit(1);
	      }
	  }
          break;
        case 'l':
            listDisplays();
            break;
        case 'i':
            infoDisplays();
            break;
        case 'm':
            setMainDisplay(targetDisplay);
            break;
        case 'r':
            angle = strtol(optarg, NULL, 10);
            break;
        default:
            break;
        }
    }
   
    if (targetDisplay == 0)
        usage();
   
    options = angle2options(angle);
   
    // Get the I/O Kit service port of the target display
    // Since the port is owned by the graphics system, we should not destroy it
    service = CGDisplayIOServicePort(targetDisplay);
   
    // We will get an error if the target display doesn't support the
    // kIOFBSetTransform option for IOServiceRequestProbe()
    dErr = IOServiceRequestProbe(service, options);
    if (dErr != kCGErrorSuccess) {
        fprintf(stderr, "IOServiceRequestProbe: error %d\n", dErr);
        exit(1);
    }
   
    exit(0);
}
