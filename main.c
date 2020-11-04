#include <stdio.h>
#include "lcd.h"

int main()
{
	initset();
	clear();
	setbkcolor(0, 0, 0);
//	getchar();
	setcolor(255, 0, 0);
//	draw_circle(512, 300, 200);
//	getchar();
//	rect(200, 200, 824, 400);
//	getchar();
	printlcd(200, 200, "Hello, NJU!");
	getchar();
	display_bmp("a1.bmp");
	getchar();
	return 0;
}
