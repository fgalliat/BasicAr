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

extern void onKeyReceived(struct KeyEvent ke);


static void setup_kbd();
static void poll_kbd();

#if (defined(KEYB_TYPE_PS2_COMFILE) && KEYB_TYPE_PS2_COMFILE > 0) 
 #include "dev_kbd_ps2_comfile.h"
#elif (defined(KEYB_TYPE_USB_HOBYTRONIC_V1) && KEYB_TYPE_USB_HOBYTRONIC_V1 > 0) 
 #include "dev_kbd_usb_hobytronic_v1.h"
#else
 #error("No Keyboard type defined")
#endif



#endif
