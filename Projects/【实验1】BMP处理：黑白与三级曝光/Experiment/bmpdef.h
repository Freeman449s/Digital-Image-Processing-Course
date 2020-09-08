#ifndef _BMPDEF_H_
#define _BMPDEF_H_

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

class BITMAPFILEHEADER {
public:
	WORD bfType;
	DWORD bfSize; //文件大小
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits; //数据区相比文件起始位置的偏移量
};
class BITMAPINFOHEADER {
public:
	DWORD biSize;
	LONG biWidth; //图像宽度（像素）
	LONG biHeight; //图像高度（像素）
	WORD biPlanes;
	WORD biBitCount; //颜色位数
	DWORD biCompression; 
	DWORD biSizeImage; //数据区大小（字节）
	LONG biXPelsPerMeter; //横向像素密度
	LONG biYPelsPerMeter; //纵向像素密度
	DWORD biClrUsed;
	DWORD biClrImportant;
};

#endif