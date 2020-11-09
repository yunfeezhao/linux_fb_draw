/*
 * LCD graphics user interface library
 *
 * Author:    Chenglei Peng
 * Created:   Sep 01, 2010
 * Updated:   Oct 10, 2018
 * Copyright: (C) Chenglei Peng <pcl@nju.edu.cn>
 *
 * This program is an example for BBB LCD GUI library. You can modify it under the 
 * terms of the GNU GPL v2. 
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include <linux/fb.h>
#include<fcntl.h>
#include<sys/mman.h>
#include "lcd.h"
#include<time.h>
#define PI 3.14159

unsigned char backgroundbuff[1280*720*3]={0};//read background picture
unsigned char buff_show[1280 * 720 * 2] = {255,0,255,0 };
unsigned short color;
int fd,mx=360,my=360;
unsigned char *addr;
struct fb_var_screeninfo vinfo;
int style = 0;
extern int type;
pthread_t ntid;
void show(void){
	draw_circle (mx, my, 5);
	memcpy(addr, buff_show, 1280 * 720 * 2);
}
void initset(void)
{
	int fd;
	int screensize = 0;

	fd = open("/dev/fb0", O_RDWR);
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	printf("Initializing screen parameters.\n");
        printf("bits_per_pixel:%d\n",vinfo.bits_per_pixel);
        printf("xres:%d\n",vinfo.xres);
        printf("yres:%d\n",vinfo.yres);

	screensize = vinfo.bits_per_pixel * vinfo.xres * vinfo.yres / 8;
	printf("screensize:%d\n", screensize);
	addr = (unsigned char *)mmap(0, screensize,
				     PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	int err = pthread_create(&ntid,NULL,mose,NULL);
    if(err!=0)
   {
       printf("thread_create Failed:%s\n",strerror(err));

   }else{
		printf("thread_create success\n");
   }
}

void clear(void)
{
	bzero(buff_show, vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8);
}

void setcolor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	printf("Setting foreground color. Red=%d, Green=%d, Blue=%d\n", Red,
	       Green, Blue);
	color = RGB565(Red, Green, Blue);
}

void dot(int x, int y)
{
	*(addr + vinfo.bits_per_pixel / 8 * (x + y * vinfo.xres) + 0) =
	    color & 0xFF;
	*(addr + vinfo.bits_per_pixel / 8 * (x + y * vinfo.xres) + 1) =
	    (color >> 8) & 0xFF;
}

void dottobuff(int x, int y)
{
	*(buff_show + vinfo.bits_per_pixel / 8 * (x + y * vinfo.xres) + 0) =
		color & 0xFF;
	*(buff_show + vinfo.bits_per_pixel / 8 * (x + y * vinfo.xres) + 1) =
		(color >> 8) & 0xFF;
}

void setbkcolor(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	int x, y;
	printf("Setting background color. Red=%d, Green=%d, Blue=%d\n", Red,
	       Green, Blue);
	color = RGB565(Red, Green, Blue);
	for (x = 0; x < vinfo.xres; x++) {
		for (y = 0; y < vinfo.yres; y++) {
			dottobuff(x, y);
		}
	}
	memcpy(addr, buff_show, 1280 * 720 * 2);
}

void setlinestyle(int sty)
{
	style = sty;
}

void circle(int x, int y, int r)
{
	int xtemp, ytemp, i;
	for (i = 0; i < 360; i++) {
		xtemp = x + r * cos(i * PI / 180);
		ytemp = y + r * sin(i * PI / 180);
		dottobuff(xtemp, ytemp);
	}

}

void line(int x1, int y1, int x2, int y2)
{
	float k;
	int i;
	switch (style) {
	case 0:
		{
			if (x1 == x2) {
				if (y1 > y2) {
					i = y1;
					y1 = y2;
					y2 = i;
				}
				for (i = y1; i <= y2; i++)
					dottobuff(x1, i);
			} else if (abs(x1 - x2) >= abs(y1 - y2)) {
				k = 1.0 * (y2 - y1) / (x2 - x1);
				if (x1 > x2)	//swap x,y 
				{
					i = x1;
					x1 = x2;
					x2 = i;
					i = y1;
					y1 = y2;
					y2 = i;
				}
				for (i = x1; i <= x2; i++) {
					dottobuff(i, (y1 + (i - x1) * k));
				}
			} else {
				k = 1.0 * (x2 - x1) / (y2 - y1);
				if (y1 > y2) {
					i = x1;
					x1 = x2;
					x2 = i;
					i = y1;
					y1 = y2;
					y2 = i;
				}
				for (i = y1; i <= y2; i++) {
					dottobuff((x1 + (i - y1) * k), i);
				}
			}
		}
	case 1:
		{
			if (x1 == x2) {
				if (y1 > y2) {
					i = y1;
					y1 = y2;
					y2 = i;
				}
				for (i = y1; i <= y2; i += 2)
					dottobuff(x1, i);
			} else if (abs(x1 - x2) >= abs(y1 - y2)) {
				k = 1.0 * (y2 - y1) / (x2 - x1);
				if (x1 > x2)	//swap x,y
				{
					i = x1;
					x1 = x2;
					x2 = i;
					i = y1;
					y1 = y2;
					y2 = i;
				}
				for (i = x1; i <= x2; i += 2) {
					dottobuff(i, (y1 + (i - x1) * k));
				}
			} else {
				k = 1.0 * (x2 - x1) / (y2 - y1);
				if (y1 > y2) {
					i = x1;
					x1 = x2;
					x2 = i;
					i = y1;
					y1 = y2;
					y2 = i;
				}
				for (i = y1; i <= y2; i += 2) {
					dottobuff((x1 + (i - y1) * k), i);
				}
			}

		}
	}
	memcpy(addr, buff_show, 1280 * 720 * 2);
}

void rect(int x1, int y1, int x2, int y2)
{
	printf("Drawing a rectangle.\n");
	line(x1, y1, x1, y2);
	line(x1, y1, x2, y1);
	line(x1, y2, x2, y2);
	line(x2, y1, x2, y2);
}

void mysin(int length)
{
	int x, y = 0, ybefore = 0;
	printf("Drawing a sin curve.\n");
	for (x = 0; x < length; x++) {
		y = 100 * sin(x / 3.14 * 40) + 120;
		line(x, ybefore, x + 1, y);
		ybefore = y;
	}
}
void *mose(void *arg){
           int fd, retval;
			char buf[6];
			fd_set readfds;
			struct timeval tv;
			// ������豸
			fd = open("/dev/input/mice", O_RDONLY);
			// �ж��Ƿ�򿪳ɹ�
			if (fd < 0) {
				printf("Failed to open \"/dev/input/mice\".\n");
				exit(1);
			}
			else {
				printf("open \"/dev/input/mice\" successfuly.\n");
			}	
			
		while(1){
			// ������ȴ�ʱ��
			tv.tv_sec = 5;
			tv.tv_usec = 0;

			FD_ZERO(&readfds);
			FD_SET(fd, &readfds);

			retval = select(fd + 1, &readfds, NULL, NULL, &tv);
			if (retval == 0) {
				printf("Time out!\n");
			}
			if (FD_ISSET(fd, &readfds)) {
				// ��ȡ����豸�е�����
				if (read(fd, buf, 6) <= 0) {
					continue;
				}
				// ��ӡ��������豸�ж�ȡ��������
				printf("Button type = %d, X = %d, Y = %d, Z = %d\n", (buf[0] & 0x07), mx, my, buf[3]);
			}
			type=buf[0] & 0x07;
			if (buf[1] & 0x80) mx = mx +  (buf[1] - 256);
			else mx = mx + buf[1];
			if (buf[2] & 0x80) my = my - (buf[2]- 256);
			else  my = my - buf[2];
			if (mx < 6) mx = 6;
			if (mx >= 1274 ) mx = 1274;
			if (my < 6) my = 6;
			if (my >= 714) my = 714;

			
		}	
			
	
}
void display_bmp(char *path)
{
	int file, x, y;


	int h[700][2]= {0};//ʱ��
	int m[750][2] = { 0 };//����
	int s[600][2] = { 0 };//����

	clock_t start,end;
	time_t now; //ʵ����time_t�ṹ 
	struct tm* timenow; //ʵ����tm�ṹָ�� 
	time(&now);    //time������ȡ���ڵ�ʱ��(���ʱ�׼ʱ��Ǳ���ʱ��)��Ȼ��ֵ��now 
	timenow = localtime(&now); //localtime�����Ѵ�timeȡ�õ�ʱ��now�����������е�ʱ��(���������õĵ���) 
	printf("Local time is %s\n", asctime(timenow)); //�Ͼ���asctime������ʱ��ת�����ַ���ͨ��printf()������� 



	file = open(path, O_RDONLY, 0666);
	if (file < 0) {
		perror("open");
		exit(1);
	}
	lseek(file, 54, SEEK_SET);
	read(file,backgroundbuff,1280*720*3);
	printf("blackstart\n");
	start=clock();
	memset(addr,0,1280*720*2);
	end=clock();
	printf("time=%f\n",(double)(end-start)/1000);

	for (x = 0; x < 100; ++x) {

		h[x][0] = x + 12;
		h[x + 100][0] = x + 13;
		h[x + 200][0] = x + 13;
		h[x+300][0] = x + 13;
		h[x + 400][0] = x + 13;
		h[x + 500][0] = x + 13;
		h[x + 600][0] = x + 12;


		h[x][1] = -3;
		h[x + 100][1] = -2;
		h[x + 200][1] = -1;
		h[x + 300][1] = 0;
		h[x + 400][1] = 1;
		h[x + 500][1] = 2;
		h[x + 600][1] = 3;

	}


	for (x = 0; x < 150; ++x) {
		m[x][0] = x + 12;
		m[x + 150][0] = x + 13;
		m[x + 300][0] = x + 13;
		m[x + 450][0] = x + 13;
		m[x + 600][0] = x + 12;

		m[x ][1] = -2;
		m[x + 150][1] = -1;
		m[x + 300][1] = 0;
		m[x + 450][1] = 1;
		m[x + 600][1] = 2;
	}

	for (x = 0; x < 200; ++x) {
		s[x][0] = x+13;
		s[x+200][0] = x+13;
		s[x+400][0] = x+13;
		s[x][1] = -1;
		s[x +200 ][1] = 0;
		s[x +400][1] = 1;
	}

	

	unsigned long j=1280*719*2,i=0;
	for (y = 719; y >=0; y--) {
		for (x = 0; x < 1280; x++) {
			color = RGB565(backgroundbuff[i+2], backgroundbuff[i+1], backgroundbuff[i]);
			buff_show[j]=color&0xFF;
			buff_show[j+1]=(color>>8)&0xFF;
			i=i+3;
			j=j+2;
		}
		j=j-1280*2*2;
	}
	memcpy(backgroundbuff,buff_show,1280*720*2);
	printf("beijingover\n");


	while (1) {
		start = clock();
		memcpy(buff_show,backgroundbuff,  1280 * 720 * 2);
		
		time(&now);    //time������ȡ���ڵ�ʱ��(���ʱ�׼ʱ��Ǳ���ʱ��)��Ȼ��ֵ��now 

		timenow = localtime(&now); //localtime�����Ѵ�timeȡ�õ�ʱ��now�����������е�ʱ��(���������õĵ���) 
		color = RGB565(240, 240, 240);//��ɫ
		printlcd(30, 20, asctime(timenow)); //asctime������ʱ��ת�����ַ���ͨ��printf()������� 
		printlcd(1080, 680, "by zhaoyunfei 181180202"); //asctime������ʱ��ת�����ַ���ͨ��printf()������� 

		for (i = 0; i < 700; i++) {

			float ang = 2 * PI * (timenow->tm_hour+ (float)(timenow->tm_min) / 60 - 3) / 12;
			x = h[i][0] * cos(ang) - h[i][1] * sin(ang);
			y = h[i][0] * sin(ang) + h[i][1] * cos(ang);
			*(buff_show + vinfo.bits_per_pixel / 8 * (x + 488 + (y + 361) * vinfo.xres) + 0) =
				color & 0xFF;
			*(buff_show + vinfo.bits_per_pixel / 8 * (x + 488 + (y + 361) * vinfo.xres) + 1) =
				(color >> 8) & 0xFF;
		};

		for (i = 0; i < 750; i++) {
			float ang = 2 * PI * (timenow->tm_min + (float)(timenow->tm_sec ) / 60 - 15) / 60;
			x = m[i][0] * cos(ang) - m[i][1] * sin(ang);
			y = m[i][0] * sin(ang) + m[i][1] * cos(ang);
			*(buff_show + vinfo.bits_per_pixel / 8 * (x + 488 + (y + 361) * vinfo.xres) + 0) =
				color & 0xFF;
			*(buff_show + vinfo.bits_per_pixel / 8 * (x + 488 + (y + 361) * vinfo.xres) + 1) =
				(color >> 8) & 0xFF;
		};




		color = RGB565(200, 47, 47);//��ɫָ��
		for (i = 0; i < 600; i++) {

			x = s[i][0] * cos(2 * PI * (timenow->tm_sec-15) / 60) - s[i][1] * sin(2 * PI *( timenow->tm_sec-15) / 60);
			y = s[i][0] * sin(2 * PI * (timenow->tm_sec-15) / 60) + s[i][1] * cos(2 * PI * (timenow->tm_sec-15) / 60);
			*(buff_show + vinfo.bits_per_pixel / 8 * (x + 488 + (y + 361) * vinfo.xres) + 0) =
				color & 0xFF;
			*(buff_show + vinfo.bits_per_pixel / 8 * (x + 488 + (y + 361) * vinfo.xres) + 1) =
				(color >> 8) & 0xFF;
		};
        draw_circle (mx, my, 5);
		memcpy(addr, buff_show, 1280 * 720 * 2);
		usleep(20000);
		end = clock();
		printf("time=%f\n", (double)(end - start) / 1000);
		if(type==1) break;		
	}

	close(fd);

	close(file);
}

void printlcd(int x, int y, char *s)
{
	register int i, j, k;
	char* temp;
	FILE *fpa, *fph;
	char buffera[16], bufferh[32];
	unsigned char qh, wh;
	unsigned long location;

	if ((fpa = fopen("./asc16", "rb")) == NULL)
		perror("open asc16");
	if ((fph = fopen("./hzk16", "rb")) == NULL)
		perror("open hzk16");
//	printf("Printing string: %s\n", s);
	temp = s;
	while (*s) {
		if (*s == '\n') *s = 0;
		++s;
	}
	s = temp;
	while (*s) {
		if ((char)(*s) > 0) {
			fseek(fpa, (*s) * 16, 0);
			fread(buffera, 1, 16, fpa);
			for (i = 0; i < 16; i++)
				for (j = 0; j < 8; j++) {
					if (((buffera[i] >> (7 - j)) & 0x01) !=
					    0)
						dottobuff(j + x, i + y);
				}
			s++;
			x += 8;
		} else {
			qh = *s - 0xa0;
			wh = *(s + 1) - 0xa0;
			printf("wh=%d\n", wh);
			location = (94 * (qh - 1) + (wh - 1)) * 32L;
			fseek(fph, location, SEEK_SET);
			fread(bufferh, 32, 1, fph);
			for (i = 0; i < 16; i++)
				for (j = 0; j < 2; j++)
					for (k = 0; k < 8; k++)
						if (((bufferh[i * 2 + j] >>
						      (7 - k)) & 0x01) != 0)
							dottobuff(x + 8 * j + k,
							    y + i);
			s += 2;
			x += 16;
		}
	}
	fclose(fpa);
	fclose(fph);
}

void _draw_circle_8(int xc, int yc, int x, int y)
{
	dot(xc + x, yc + y);
	dot(xc - x, yc + y);
	dot(xc + x, yc - y);
	dot(xc - x, yc - y);
	dot(xc + y, yc + x);
	dot(xc - y, yc + x);
	dot(xc + y, yc - x);
	dot(xc - y, yc - x);
}
void _draw_circle_8_buff(int xc, int yc, int x, int y)
{
	dottobuff(xc + x, yc + y);
	dottobuff(xc - x, yc + y);
	dottobuff(xc + x, yc - y);
	dottobuff(xc - x, yc - y);
	dottobuff(xc + y, yc + x);
	dottobuff(xc - y, yc + x);
	dottobuff(xc + y, yc - x);
	dottobuff(xc - y, yc - x);
}

void draw_circle(int xc, int yc, int r)
{

	printf("Drawing a circle. radius=%d\n", r);
	if (xc + r < 0 || xc - r >= 1280 || yc + r < 0 || yc - r >= 720)
		return;

	int x = 0, y = r, yi, d;
	d = 3 - 2 * r;

	while (x <= y) {
		for (yi = x; yi <= y; yi++)
			_draw_circle_8_buff(xc, yc, x, yi);

		if (d < 0) {
			d = d + 4 * x + 6;
		} else {
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}
