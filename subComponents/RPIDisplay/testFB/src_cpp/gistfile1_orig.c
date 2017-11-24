// https://gist.github.com/robmint/4753401
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/input.h>
#include <linux/kd.h>

void handler (int sig)
{
  printf ("nexiting...(%d)n", sig);
  exit (0);
}
 
void perror_exit (char *error)
{
  perror (error);
  handler (9);
}

int main (int argc, char *argv[])
{

    /* framebuffer */
    int fbfd = 0, tty = 0;;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0, i = 0;
    uint8_t pixel = 0;
    bool stripe = true;
    long int location = 0;

    /* keyboard input */
    struct input_event ev[64];
    int fd, rd, value, size = sizeof (struct input_event);
    char name[256] = "Unknown";
    char *device = NULL;
    int key = 0;

    if ((getuid ()) != 0) {
        printf ("You are not root, run this as root or sudo\n");
        exit(1);
    }

    /* command line parameter of keyboard device */
    if (argv[1] == NULL){
      printf("Please specify (on the command line) the path to the dev event interface device\n");
      exit (0);
    }

    if (argc > 1)
      device = argv[1];
/* open keyboard */
    if ((fd = open (device, O_RDONLY|O_NONBLOCK)) == -1) {
        printf ("%s is not a vaild device\n", device);
        perror_exit("not a vaild device");
    }

    /* open framebuffer */
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");


    tty = open("/dev/tty1", O_RDWR);

    if(ioctl(tty, KDSETMODE, KD_GRAPHICS) == -1)
	printf("Failed to set graphics mode on tty1\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    //screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    screensize = vinfo.yres_virtual * finfo.line_length;
    printf("screensize: %d\n",screensize); 


    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");


    while (rd = read (fd, ev, size * 64) ){
      value = ev[0].value;
 
      if (value != ' ' && ev[1].value == 1 && ev[1].type == 1){ // Only read the key press event
	key = ev[1].code;
       printf ("Code[%d]\n", (ev[1].code));
      }

    /* blit the screen */    
	for (y = 0; y <vinfo.yres; y++) {
		for (x = 0; x <vinfo.xres; x++) {
		    location = (y*finfo.line_length)+x;
		    //pixel = (x%128)/8;
		    pixel = (x%128)/(key+1);
		    if(y%32<16) pixel = 0;
		    *((uint8_t*)(fbp + location)) = pixel;
		}
	}



	//	nanosleep((struct timespec[]){{0, 50000000}}, NULL);
	//nanosleep((struct timespec[]){{0, 100000000}}, NULL); // 100th sec

  }


    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
