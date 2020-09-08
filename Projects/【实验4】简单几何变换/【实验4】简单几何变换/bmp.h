#ifndef _BMP_H_
#define _BMP_H_
#include<stdio.h>

//声明别名
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

//结构体声明
class BITMAPFILEHEADER {
public:
	WORD bfType; //文件类型（对于BMP文件，该值恒为"BM",十六进制值为42 4D）
	DWORD bfSize; //文件大小
	WORD bfReserved1; //保留字，忽略
	WORD bfReserved2; //保留字，忽略
	DWORD bfOffBits; //数据区相比文件起始位置的偏移量
	//默认构造函数：所有成员变量初始化为0
	BITMAPFILEHEADER() {
		bfType = 0;
		bfSize = 0;
		bfReserved1 = 0;
		bfReserved2 = 0;
		bfOffBits = 0;
	}
};
class BITMAPINFOHEADER {
public:
	DWORD biSize; //本结构大小。在Windows下总为40（字节）。
	LONG biWidth; //图像宽度（像素）
	LONG biHeight; //图像高度（像素）
	WORD biPlanes;
	WORD biBitCount; //颜色位数
	DWORD biCompression;
	DWORD biSizeImage; //数据区大小（字节），往往比实际的数据区大小大2.
	LONG biXPelsPerMeter; //横向像素密度
	LONG biYPelsPerMeter; //纵向像素密度
	DWORD biClrUsed;
	DWORD biClrImportant;
	//默认构造函数：所有成员变量初始化为0
	BITMAPINFOHEADER() {
		biSize = 0;
		biWidth = 0;
		biHeight = 0;
		biPlanes = 0;
		biBitCount = 0;
		biCompression = 0;
		biSizeImage = 0;
		biXPelsPerMeter = 0;
		biYPelsPerMeter = 0;
		biClrUsed = 0;
		biClrImportant = 0;
	}
};
class RGBQUAD {
public:
	BYTE R;
	BYTE G;
	BYTE B;
	BYTE reserved;
	//默认构造函数：所有成员变量初始化为0
	RGBQUAD() {
		R = G = B = reserved = 0;
	}
};
class RGBInfo {
public:
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER InfoHeader;
	BYTE *data;
	int width_byte;
	//默认构造函数：所有成员变量初始化为0
	RGBInfo() {
		data = NULL;
		width_byte = 0;
	}
};
class GSInfo {
public:
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER InfoHeader;
	RGBQUAD *palette;
	BYTE *data;
	int width_byte;
	//默认构造函数：所有成员变量初始化为0
	GSInfo() {
		palette = NULL;
		data = NULL;
		width_byte = 0;
	}
};

//函数原型声明
RGBInfo imread_RGB(const char name[]);
GSInfo imread_GS(const char name[]);
void imwrite(RGBInfo info, const char name[]);
void imwrite(GSInfo info, const char name[]);

#endif