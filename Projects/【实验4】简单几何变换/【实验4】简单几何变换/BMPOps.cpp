#include "bmp.h"
#include<iostream>
using namespace std;

RGBInfo imread_RGB(const char path[]) {
	FILE *fp = fopen(path, "rb");
	RGBInfo info;
	if (fp == NULL) {
		cout << "Error: File Not Exist" << endl;
		return info;
	}
	fread(&(info.FileHeader.bfType), 1, sizeof(WORD), fp);
	fread(&(info.FileHeader.bfSize), 1, sizeof(DWORD), fp);
	fread(&(info.FileHeader.bfReserved1), 1, sizeof(WORD), fp);
	fread(&(info.FileHeader.bfReserved2), 1, sizeof(WORD), fp);
	fread(&(info.FileHeader.bfOffBits), 1, sizeof(DWORD), fp);
	fread(&info.InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	info.width_byte = (info.InfoHeader.biWidth*info.InfoHeader.biBitCount + 31) / 32 * 4;
	info.data = new BYTE[3 * info.InfoHeader.biHeight*info.width_byte];
	fseek(fp, info.FileHeader.bfOffBits, SEEK_SET);
	fread(info.data, sizeof(BYTE), 3 * info.InfoHeader.biHeight*info.width_byte, fp);
	fclose(fp);
	return info;
}
GSInfo imread_GS(const char path[]) {
	FILE *fp = fopen(path, "rb");
	GSInfo info;
	if (fp == NULL) {
		cout << "Error: File Not Exist" << endl;
		return info;
	}
	fread(&(info.FileHeader.bfType), 1, sizeof(WORD), fp);
	fread(&(info.FileHeader.bfSize), 1, sizeof(DWORD), fp);
	fread(&(info.FileHeader.bfReserved1), 1, sizeof(WORD), fp);
	fread(&(info.FileHeader.bfReserved2), 1, sizeof(WORD), fp);
	fread(&(info.FileHeader.bfOffBits), 1, sizeof(DWORD), fp);
	fread(&info.InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	info.width_byte = (info.InfoHeader.biWidth*info.InfoHeader.biBitCount + 31) / 32 * 4;
	info.palette = new RGBQUAD[256];
	fread(info.palette, 256 * sizeof(RGBQUAD), 1, fp);
	info.data = new BYTE[info.InfoHeader.biHeight*info.width_byte];
	fread(info.data, 1, info.InfoHeader.biHeight*info.width_byte * sizeof(BYTE), fp);
	fclose(fp);
	return info;
}
void imwrite(RGBInfo info, const char name[]) {
	FILE* fp;
	fp = fopen(name, "wb");
	if (fp == NULL) {
		cout << "Error: Cannot Open This File" << endl;
		return;
	}
	fwrite(&(info.FileHeader.bfType), 1, sizeof(WORD), fp);
	fwrite(&(info.FileHeader.bfSize), 1, sizeof(DWORD), fp);
	fwrite(&(info.FileHeader.bfReserved1), 1, sizeof(WORD), fp);
	fwrite(&(info.FileHeader.bfReserved2), 1, sizeof(WORD), fp);
	fwrite(&(info.FileHeader.bfOffBits), 1, sizeof(DWORD), fp);
	fwrite(&info.InfoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
	fwrite(info.data, 1, info.InfoHeader.biHeight*info.width_byte * sizeof(BYTE), fp);
	fclose(fp);
}
void imwrite(GSInfo info, const char name[]) {
	FILE* fp;
	fp = fopen(name, "wb");
	if (fp == NULL) {
		cout << "Error: Cannot Open This File" << endl;
		return;
	}
	fwrite(&(info.FileHeader.bfType), 1, sizeof(WORD), fp);
	fwrite(&(info.FileHeader.bfSize), 1, sizeof(DWORD), fp);
	fwrite(&(info.FileHeader.bfReserved1), 1, sizeof(WORD), fp);
	fwrite(&(info.FileHeader.bfReserved2), 1, sizeof(WORD), fp);
	fwrite(&(info.FileHeader.bfOffBits), 1, sizeof(DWORD), fp);
	fwrite(&info.InfoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
	fwrite(info.palette, 1, 256 * sizeof(RGBQUAD), fp);
	fwrite(info.data, 1, info.InfoHeader.biHeight*info.width_byte * sizeof(BYTE), fp);
	fclose(fp);
}