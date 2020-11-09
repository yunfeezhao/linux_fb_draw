#include <stdio.h>
#include "lcd.h"
int type=0;
int main()
{
	int i=0,k=0;
	initset();
	clear();
	setbkcolor(0, 0, 0);
	getchar();
	setcolor(255, 0, 0);
	
	
	while(1){
	clear();
	circle(640+i, 360+k, 50);
	circle(640+i, 360-k, 50);
	circle(640-i, 360+k, 50);
	circle(640-i, 360-k, 50);
	show();

	if(i>590)i=0,k=0;
	else if(i>=310) --k,++i;
	else k++,i++;
	usleep(10000);
	if(type==1) break;
	}
	
	rect(200, 200, 824, 400);
	printlcd(200, 200, "Hello, NJU!");
	display_bmp("a1.bmp");
	getchar();
	return 0;
}
