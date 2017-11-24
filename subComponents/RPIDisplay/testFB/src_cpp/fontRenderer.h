#include "../fonts/font_plain.h"

void dispChar(int code, int x, int y);
void dispCharColor(Color* fb, int code, int x, int y, Color color);
void dispCharScale(int code, int x, int y, int scale);

void dispChar(int code, int x, int y) {
	dispCharColor(VRAM_A, code, x, y, RGB15(31, 31, 31));
}

void dispCharColor(Color* fb, int code, int x, int y, Color color) {
	int offset = code * (fontWidth*fontHeight);
	int xx,yy;
	for(yy=0; yy < fontHeight; yy++) {
		for(xx=0; xx < fontWidth; xx++) {
			if ( fontPLAIN_data[ offset++ ] > 0 ) {
				fb[((y+yy) * SCREEN_WIDTH) + (x+xx)] = color;
			}
		}
	}
}

void dispCharColorTrurnLeft(Color* memory, int code, int x, int y, Color color) {
	int offset = code * (fontWidth*fontHeight);
	int xx,yy;
	for(yy=0; yy < fontHeight; yy++) {
		for(xx=0; xx < fontWidth; xx++) {
			if ( fontPLAIN_data[ offset++ ] > 0 ) {
				memory[(( (x+xx)) * SCREEN_WIDTH) + (SCREEN_WIDTH-(y+yy))] = color;
			}
		}
	}
}

void dispCharScale(int code, int x, int y, int scale) {
	Color color = RGB15(31,31,31);
	int offset = code * (fontWidth*fontHeight);
	int xx,yy;
	for(yy=0; yy < fontHeight; yy++) {
		for(xx=0; xx < fontWidth; xx++) {
			if ( fontPLAIN_data[ offset++ ] > 0 ) {
				Video_fillRectangle(VRAM_A, ((xx*scale)+x), ((yy*scale)+y), scale, scale, color, color, 1.0f);
			}
		}
	}
}
