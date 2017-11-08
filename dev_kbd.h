#ifndef keyb_h_
#define keyb_h_ 1

typedef struct KeyEvent {
  int kc;
  int vkc;
  char printableChar;
  int modifier;
  bool printable;
  const char* descr;
} KeyEvent;

enum {
  MOD_NONE = 0,
	MOD_CTRL,
	MOD_ALT,
  MOD_CTRL_ALT,
	MOD_CTRL_RIGHT,
	MOD_ALT_RIGTH,
};

enum {
  KEY_VIRT_NONE = 0,
  KEY_VIRT_BREAK,
  KEY_VIRT_ESC,
};

#define KEY_VIRT_CTRLC KEY_VIRT_BREAK

// @Deprecated
extern void onKeyReceived(struct KeyEvent ke);

#define KBD_ESC   27
#define KBD_ENTER 13
#define KBD_BCKSP 8
#define KBD_TAB   9

#define KBD_CTRL_C 3
#define KBD_BREAK 1000

#define KBD_F1    1101
#define KBD_F2    1102
#define KBD_F3    1103
#define KBD_F4    1104
#define KBD_F5    1105
#define KBD_F6    1106
#define KBD_F7    1107
#define KBD_F8    1108
#define KBD_F9    1109
#define KBD_F10   1110
#define KBD_F11   1111
#define KBD_F12   1112

#define KBD_ARW_UP    1201
#define KBD_ARW_DOWN  1202
#define KBD_ARW_LEFT  1203
#define KBD_ARW_RIGHT 1204


static void setup_kbd();
//static void poll_kbd();
static int read_kbd(bool *printable); // return -1 if no key pressed

#if (defined(KEYB_TYPE_PS2_COMFILE) && KEYB_TYPE_PS2_COMFILE > 0) 
 // deprecated implementation ... 
 //#include "dev_kbd_ps2_comfile.h"
#elif (defined(KEYB_TYPE_USB_HOBYTRONIC_V1) && KEYB_TYPE_USB_HOBYTRONIC_V1 > 0) 
 #include "dev_kbd_usb_hobytronic_v1.h"
#else
 #error("No Keyboard type defined")
#endif



#endif
