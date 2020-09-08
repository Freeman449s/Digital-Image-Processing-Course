#ifndef _BMPDEF_H_
#define _BMPDEF_H_

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

class BITMAPFILEHEADER {
public:
	WORD bfType; //文件类型（对于BMP文件，该值恒为"BM",十六进制值为42 4D）
	DWORD bfSize; //文件大小
	WORD bfReserved1; //保留字，忽略
	WORD bfReserved2; //保留字，忽略
	DWORD bfOffBits; //数据区相比文件起始位置的偏移量
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
};
class RGBQUAD {
	BYTE R;
	BYTE G;
	BYTE B;
	BYTE reserved;
public:
	//构造函数：将reserved自动赋0.
	RGBQUAD() {
		this->reserved = 0;
	}
	BYTE getR() {
		return R;
	}
	BYTE getG() {
		return G;
	}
	BYTE getB() {
		return B;
	}
	void setR(BYTE R) {
		this->R = R;
	}
	void setG(BYTE G) {
		this->G = G;
	}
	void setB(BYTE B) {
		this->B = B;
	}
};

#endif