#include<iostream>
#include<cstdlib>
#include<string>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"bmpdef.h"
#pragma warning(disable:4996)
using namespace std;

int main(void) {
	BITMAPFILEHEADER file;
	BITMAPINFOHEADER info;
	string path;
	FILE *fp;
	fp = fopen("DSC_0129_BMP.bmp", "rb");
	//提取BMP文件信息
	fread(&(file.bfType), 1, sizeof(WORD), fp);
	fread(&(file.bfSize), 1, sizeof(DWORD), fp);
	fread(&(file.bfReserved1), 1, sizeof(WORD), fp);
	fread(&(file.bfReserved2), 1, sizeof(WORD), fp);
	fread(&(file.bfOffBits), 1, sizeof(DWORD), fp);
	fread(&info, sizeof(BITMAPINFOHEADER), 1, fp);
	//读取RGB数据
	int actheight = info.biHeight;
	int actwidth = (info.biWidth*info.biBitCount + 31) / 32 * 32 / info.biBitCount;
	BYTE* rgbdataptr_b = (BYTE*)malloc(actwidth*actheight * sizeof(BYTE) * 3);
	fseek(fp, file.bfOffBits, SEEK_SET);
	fread(rgbdataptr_b, sizeof(BYTE), actwidth*actheight * 3, fp);
	//RGB转YUV
	short *yuvdataptr = (short*)malloc(actwidth*actheight * sizeof(short) * 3);
	for (long i = 0; i < actwidth * actheight; i++) {
		yuvdataptr[3 * i] = 0.114*rgbdataptr_b[3 * i] + 0.587*rgbdataptr_b[3 * i + 1] + 0.299*rgbdataptr_b[3 * i + 2];
		yuvdataptr[3 * i + 1] = 0.435*rgbdataptr_b[3 * i] - 0.289*rgbdataptr_b[3 * i + 1] - 0.147*rgbdataptr_b[3 * i + 2];
		yuvdataptr[3 * i + 2] = -0.1*rgbdataptr_b[3 * i] - 0.515*rgbdataptr_b[3 * i + 1] + 0.615*rgbdataptr_b[3 * i + 2];;
	}

	//建立黑白图片RGB信息
	BYTE *rgbdataptr_bw_b = (BYTE*)malloc(actwidth*actheight * sizeof(BYTE) * 3);
	for (long i = 0; i < actheight * actwidth; i++) {
		rgbdataptr_bw_b[3 * i] = rgbdataptr_bw_b[3 * i + 1] = rgbdataptr_bw_b[3 * i + 2] = yuvdataptr[3 * i];
	}
	//输出黑白图片
	FILE* fpbw;
	fpbw = fopen("BWBMP.bmp", "wb");
	fwrite(&(file.bfType), 1, sizeof(file.bfType), fpbw);
	fwrite(&(file.bfSize), 1, sizeof(file.bfSize), fpbw);
	fwrite(&(file.bfReserved1), 1, sizeof(file.bfReserved1), fpbw);
	fwrite(&(file.bfReserved2), 1, sizeof(file.bfReserved2), fpbw);
	fwrite(&(file.bfOffBits), 1, sizeof(file.bfOffBits), fpbw);
	fwrite(&info, 1, sizeof(info), fpbw);
	fwrite(rgbdataptr_bw_b, sizeof(BYTE), actwidth*actheight * 3, fpbw);
	free(rgbdataptr_bw_b);
	fclose(fpbw);

	//调整图片欠曝
	for (long i = 0; i < actwidth*actheight; i++) {
		yuvdataptr[3 * i] /= 1.2;
	}
	//YUV转RGB
	BYTE *rgbdataptr_ue_b = (BYTE*)malloc(actheight*actwidth * sizeof(BYTE) * 3);
	for (long i = 0; i < actwidth*actheight; i++) {
		rgbdataptr_ue_b[3 * i] = yuvdataptr[3 * i] + 2.032*yuvdataptr[3 * i + 1];
		rgbdataptr_ue_b[3 * i + 1] = yuvdataptr[3 * i] - 0.395*yuvdataptr[3 * i + 1] - 0.581*yuvdataptr[3 * i + 2];
		rgbdataptr_ue_b[3 * i + 2] = yuvdataptr[3 * i] + 1.14*yuvdataptr[3 * i + 2];
	}
	//输出欠曝图片
	FILE* fpue;
	fpue = fopen("UEBMP.bmp", "wb");
	fwrite(&(file.bfType), 1, sizeof(file.bfType), fpue);
	fwrite(&(file.bfSize), 1, sizeof(file.bfSize), fpue);
	fwrite(&(file.bfReserved1), 1, sizeof(file.bfReserved1), fpue);
	fwrite(&(file.bfReserved2), 1, sizeof(file.bfReserved2), fpue);
	fwrite(&(file.bfOffBits), 1, sizeof(file.bfOffBits), fpue);
	fwrite(&info, 1, sizeof(info), fpue);
	fwrite(rgbdataptr_ue_b, sizeof(BYTE), actwidth*actheight * 3, fpue);
	free(rgbdataptr_ue_b);
	fclose(fpue);

	//调整图片过曝
	for (long i = 0; i < actwidth*actheight; i++) {
		if (yuvdataptr[3 * i] * 1.2 <= 255) {
			yuvdataptr[3 * i] *= 1.2;
		}
		else yuvdataptr[3 * i] = 255;
	}
	//YUV转RGB
	BYTE *rgbdataptr_oe_b = (BYTE*)malloc(actheight*actwidth * sizeof(BYTE) * 3);
	for (long i = 0; i < actwidth*actheight; i++) {
		rgbdataptr_oe_b[3 * i] = yuvdataptr[3 * i] + 2.032*yuvdataptr[3 * i + 1];
		rgbdataptr_oe_b[3 * i + 1] = yuvdataptr[3 * i] - 0.395*yuvdataptr[3 * i + 1] - 0.581*yuvdataptr[3 * i + 2];
		rgbdataptr_oe_b[3 * i + 2] = yuvdataptr[3 * i] + 1.14*yuvdataptr[3 * i + 2];
	}
	//输出过曝图片
	FILE* fpoe;
	fpoe = fopen("OEBMP.bmp", "wb");
	fwrite(&(file.bfType), 1, sizeof(file.bfType), fpoe);
	fwrite(&(file.bfSize), 1, sizeof(file.bfSize), fpoe);
	fwrite(&(file.bfReserved1), 1, sizeof(file.bfReserved1), fpoe);
	fwrite(&(file.bfReserved2), 1, sizeof(file.bfReserved2), fpoe);
	fwrite(&(file.bfOffBits), 1, sizeof(file.bfOffBits), fpoe);
	fwrite(&info, 1, sizeof(info), fpoe);
	fwrite(rgbdataptr_oe_b, sizeof(BYTE), actwidth*actheight * 3, fpoe);
	free(rgbdataptr_oe_b);
	fclose(fpoe);

	free(yuvdataptr);
	free(rgbdataptr_b);
	fclose(fp);

	return 0;
}