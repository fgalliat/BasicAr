// #include <avr/io.h>
// #include <util/delay.h>

#include "ili9341.h"
#include "ili9341gfx.h"

extern uint16_t vsetx,vsety,vactualx,vactualy,isetx,isety,iactualx,iactualy;

// static FILE mydata = FDEV_SETUP_STREAM(ili9341_putchar_printf, NULL, _FDEV_SETUP_WRITE);

void setup();
void loop();

#ifdef COMPUTER
int main(int argc, char** argv) {
	setup();
return 0;
}
#else
 #import <SPIFFS.h>;
#endif




void setup_DISABLED() {
	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, HIGH);
	Serial.begin(115200);
	Serial.println("Formatting starts");
	#ifdef BUT_ESP32
	SPIFFS.format();
	#endif
	Serial.println("Formatting stops");
	digitalWrite(BUILTIN_LED, LOW);
}


void setup() {
	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, HIGH);
	Serial.begin(115200);
	Serial.println("Starting");
	

	// stdout = & mydata;

	ili9341_init(); //initial driver setup to drive ili9341
	ili9341_clear(BLACK); //fill screen with black colour
	_delay_ms(1000);
	ili9341_setRotation(3); //rotate screen
	_delay_ms(2);
	display_init(); //display initial data

	Serial.println("Ready");
	ili9341_settextsize(2);

	while (1)
	{
		ili9341_settextcolour(CYAN,BLACK);

		ili9341_setcursor(vsetx,vsety);
		// _delay_ms(2);
		//ili9341_settextsize(3);
		
		ili9341_write('1');
		// _delay_ms(2);
		ili9341_write('0');
		// _delay_ms(2);
		ili9341_write('.');
		// _delay_ms(2);
		ili9341_write('2');
		// _delay_ms(2);
		//ili9341_write('3');
		ili9341_write(  ('0' + (char)(random(10)) ) );
		// _delay_ms(2);

 ili9341_setcursor(vactualx,vactualy);
 ili9341_write('0');
		ili9341_write('8');
		ili9341_write('.');
		ili9341_write(  ('0' + (char)(random(10)) ) );
		ili9341_write('2');


_delay_ms(200);


		// ili9341_setcursor(vactualx,vactualy);
		// _delay_ms(2);
		// ili9341_settextsize(5);
		// ili9341_write('1');
		// _delay_ms(2);
		// ili9341_write('0');
		// _delay_ms(2);
		// ili9341_write('.');
		// _delay_ms(2);
		// ili9341_write('2');
		// _delay_ms(2);
		// ili9341_write('3');
		// _delay_ms(2);

		// _delay_ms(2000);

		// ili9341_setcursor(vsetx,vsety);
		// _delay_ms(2);
		// ili9341_settextsize(3);
		// ili9341_write('9');
		// _delay_ms(2);
		// ili9341_write('0');
		// _delay_ms(2);
		// ili9341_write('.');
		// _delay_ms(2);
		// ili9341_write('4');
		// _delay_ms(2);
		// ili9341_write('5');
		// _delay_ms(2);



		// ili9341_setcursor(vactualx,vactualy);
		// _delay_ms(2);
		// ili9341_settextsize(5);
		// ili9341_write('9');
		// _delay_ms(2);
		// ili9341_write('0');
		// _delay_ms(2);
		// ili9341_write('.');
		// _delay_ms(2);
		// ili9341_write('4');
		// _delay_ms(2);
		// ili9341_write('5');
		// _delay_ms(2);

		// _delay_ms(2000);
	}
}

void loop() {

}

