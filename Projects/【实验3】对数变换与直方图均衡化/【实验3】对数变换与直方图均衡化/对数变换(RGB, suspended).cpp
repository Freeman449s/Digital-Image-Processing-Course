#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"BMPDef.h"
using namespace std;

BYTE *GetRGBValue(BYTE *RGBData, int w, int row, int col, int layer);
double *GetYUVValue(double *YUVData, int w, int row, int col, int layer);
double iLog(double antilog, double base);
void LogTrans(double *YUVData, int w, int h);
void imwrite(BITMAPFILEHEADER file, BITMAPINFOHEADER info, BYTE *data, int size, const char name[]);

int main(void) {
	BITMAPFILEHEADER file;
	BITMAPINFOHEADER info;
	FILE *fp;
	fp = fopen("DSC00175.bmp", "rb");
	//提取BMP文件信息
	fread(&(file.bfType), 1, sizeof(WORD), fp);
	fread(&(file.bfSize), 1, sizeof(DWORD), fp);
	fread(&(file.bfReserved1), 1, sizeof(WORD), fp);
	fread(&(file.bfReserved2), 1, sizeof(WORD), fp);
	fread(&(file.bfOffBits), 1, sizeof(DWORD), fp);
	fread(&info, sizeof(BITMAPINFOHEADER), 1, fp);
	//读取RGB数据
	int ah = info.biHeight;
	int aw = (info.biWidth*info.biBitCount + 31) / 32 * 32 / info.biBitCount;
	BYTE* RGBData = new BYTE[ah*aw * 3];
	fseek(fp, file.bfOffBits, SEEK_SET);
	fread(RGBData, sizeof(BYTE), aw*ah * 3, fp);
	//RGB转YUV
	double *YUVData = new double[ah*aw * 3];
	for (int row = 1; row <= ah; row++) {
		for (int col = 1; col <= aw; col++) {
			*GetYUVValue(YUVData, aw, row, col, 1) = *GetRGBValue(RGBData, aw, row, col, 1)*0.114 + *GetRGBValue(RGBData, aw, row, col, 2)*0.587 + *GetRGBValue(RGBData, aw, row, col, 3)*0.299;
			*GetYUVValue(YUVData, aw, row, col, 2) = *GetRGBValue(RGBData, aw, row, col, 1)*0.437 + *GetRGBValue(RGBData, aw, row, col, 2)*(-0.289) + *GetRGBValue(RGBData, aw, row, col, 3)*(-0.148);
			*GetYUVValue(YUVData, aw, row, col, 3) = *GetRGBValue(RGBData, aw, row, col, 1)*(-0.1) + *GetRGBValue(RGBData, aw, row, col, 2)*(-0.515) + *GetRGBValue(RGBData, aw, row, col, 3)*0.615;
		}
	}
	LogTrans(YUVData, aw, ah);
	//YUV转RGB
	for (int row = 1; row <= ah; row++) {
		for (int col = 1; col <= aw; col++) {
			*GetRGBValue(RGBData, aw, row, col, 1) = *GetYUVValue(YUVData, aw, row, col, 1) + *GetYUVValue(YUVData, aw, row, col, 2)*2.032 + 0.5;
			*GetRGBValue(RGBData, aw, row, col, 2) = *GetYUVValue(YUVData, aw, row, col, 1) + *GetYUVValue(YUVData, aw, row, col, 2)*(-0.395) + *GetYUVValue(YUVData, aw, row, col, 3)*(-0.581) + 0.5;
			*GetRGBValue(RGBData, aw, row, col, 2) = *GetYUVValue(YUVData, aw, row, col, 1) + *GetYUVValue(YUVData, aw, row, col, 3)*1.14 + 0.5;
		}
	}
	imwrite(file, info, RGBData, aw*ah, "Logarithmic Transformed Image.bmp");

	delete[] YUVData;
	delete[] RGBData;
	return 0;
}

//取RGB值函数
//参数：接受RGB数据区，像素在数据区中的行指标、列指标，数据区的宽度作为参数，并接受一个参数指示返回R/G/B中的哪一个值
//返回值：返回一个指向数据区中所求元素的指针
//注意：在文件中，一个像素三个通道的值的存储顺序为B,G,R
BYTE *GetRGBValue(BYTE *RGBData, int w, int row, int col, int layer) {
	return &RGBData[((row - 1)*w + col - 1) * 3 + layer - 1];
}

//取YUV值函数
//参数：接受YUV数据区，像素在数据区中的行指标、列指标，数据区的宽度作为参数，并接受一个参数指示返回Y/U/V中的哪一个值
//返回值：返回一个指向数据区中所求元素的指针
double *GetYUVValue(double *YUVData, int w, int row, int col, int layer) {
	return &YUVData[((row - 1)*w + col - 1) * 3 + layer - 1];
}

//对数计算函数
//参数：接受真数antilog与底base作为参数
//返回值：返回以base为底antilog的对数
double iLog(double antilog, double base) {
	return log(antilog) / log(base);
}

//对数变换函数
//参数：接受YUV数据区，数据区宽度、高度作为参数
//返回值：无返回值
void LogTrans(double *YUVData, int w, int h) {
	double *LogData = new double[w*h];
	double MaxV = 0;
	for (int row = 1; row <= h; row++) {
		for (int col = 1; col <= w; col++) {
			if (*GetYUVValue(YUVData, w, row, col, 1) > MaxV) MaxV = *GetYUVValue(YUVData, w, row, col, 1);
		}
	}
	for (int row = 1; row <= h; row++) {
		for (int col = 1; col <= w; col++) {
			LogData[(row - 1)*w + col - 1] = iLog(*GetYUVValue(YUVData, w, row, col, 1) + 1, MaxV + 1);
		}
	}
	//求线性拉伸函数
	double MaxLog = 0, MinLog = 1;
	for (int i = 0; i <= w * h - 1; i++) {
		if (LogData[i] > MaxLog) MaxLog = LogData[i];
		if (LogData[i] < MinLog) MinLog = LogData[i];
	}
	double slope = 255 / (MaxLog - MinLog);
	//线性拉伸
	for (int row = 1; row <= h; row++) {
		for (int col = 1; col <= w; col++) {
			*GetYUVValue(YUVData, w, row, col, 1) = (LogData[(row - 1)*w + col - 1] - MinLog)*slope;
		}
	}
	delete[] LogData;
}

//RGB图片输出函数
//参数：接受文件头对象、信息头对象、数据区、图像尺寸、文件名作为参数
//返回值：无返回值
void imwrite(BITMAPFILEHEADER file, BITMAPINFOHEADER info, BYTE *data, int size, const char name[]) {
	FILE *fp;
	fp = fopen(name, "wb");
	fwrite(&(file.bfType), 1, sizeof(file.bfType), fp);
	fwrite(&(file.bfSize), 1, sizeof(file.bfSize), fp);
	fwrite(&(file.bfReserved1), 1, sizeof(file.bfReserved1), fp);
	fwrite(&(file.bfReserved2), 1, sizeof(file.bfReserved2), fp);
	fwrite(&(file.bfOffBits), 1, sizeof(file.bfOffBits), fp);
	fwrite(&info, 1, sizeof(info), fp);
	fwrite(data, 1, size * sizeof(BYTE) * 3, fp);
	fclose(fp);
}