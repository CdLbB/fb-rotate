// fb-rotate.c
   
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
                    "       %s -d <display ID> -r <0|90|180|270>\n",
                    PROGNAME, PROGNAME);
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
   
    while ((i = getopt(argc, argv, "d:lr:")) != -1) {
        switch (i) {
        case 'd':
            targetDisplay = (CGDirectDisplayID)strtol(optarg, NULL, 16);
            if (targetDisplay == 0)
                targetDisplay = CGMainDisplayID();
            break;
        case 'l':
            listDisplays();
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
