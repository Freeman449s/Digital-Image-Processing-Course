#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<map>
#include"BMPDef.h"
using namespace std;

double iLog(double antilog, double base);
void LogTrans(BYTE *data, int w, int h);
map<int, double> GeneMap(BYTE *data, int size);
void imwrite(BITMAPFILEHEADER file, BITMAPINFOHEADER info, RGBQUAD *palette, BYTE *data, int size, const char name[]);

int main(void) {
	BITMAPFILEHEADER file;
	BITMAPINFOHEADER info;
	FILE *fp;
	fp = fopen("GrayScaleBMP.bmp", "rb");
	//提取BMP文件信息
	fread(&(file.bfType), sizeof(WORD), 1, fp);
	fread(&(file.bfSize), sizeof(DWORD), 1, fp);
	fread(&(file.bfReserved1), sizeof(WORD), 1, fp);
	fread(&(file.bfReserved2), sizeof(WORD), 1, fp);
	fread(&(file.bfOffBits), sizeof(DWORD), 1, fp);
	fread(&info, sizeof(BITMAPINFOHEADER), 1, fp);
	RGBQUAD *palette = new RGBQUAD[256];
	fread(palette, 256 * sizeof(RGBQUAD), 1, fp);
	int ah = info.biHeight;
	int aw = (info.biWidth*info.biBitCount + 31) / 32 * 32 / info.biBitCount;
	BYTE *OriData = new BYTE[ah*aw];
	fread(OriData, aw*ah * sizeof(BYTE), 1, fp);

	//对数变换
	cout << "正在进行对数变换..." << endl;
	BYTE *data = new BYTE[aw*ah];
	for (int i = 0; i <= aw * ah - 1; i++) {
		data[i] = OriData[i];
	}
	LogTrans(data, aw, ah);
	imwrite(file, info, palette, data, aw*ah, "Logarithmic Transformed Image.bmp");
	cout << "对数变换完成" << endl;

	//直方图均衡化
	cout << "正在进行直方图均衡化..." << endl;
	cout << "创建数据区..." << endl;
	for (int i = 0; i <= aw * ah - 1; i++) {
		data[i] = OriData[i];
	}
	cout << "取得映射..." << endl;
	map<int, double> NewGS = GeneMap(data, aw*ah);
	cout << "应用更改..." << endl;
	for (int i = 0; i <= aw * ah - 1; i++) {
		data[i] = NewGS[data[i]];
	}
	imwrite(file, info, palette, data, aw*ah, "Equalized Image.bmp");
	cout << "直方图均衡化完成" << endl;

	delete[] data;
	delete[] OriData;
	delete[] palette;
	system("pause");
	return 0;
}

//对数计算函数
//参数：接受真数antilog与底base作为参数
//返回值：返回以base为底antilog的对数
double iLog(double antilog, double base) {
	return log(antilog) / log(base);
}

//对数变换函数
//参数：接收数据区，数据区宽度、高度作为参数
//返回值：无返回值
void LogTrans(BYTE *data, int w, int h) {
	double *LogData = new double[w*h];
	int MaxV = 0;
	for (int i = 0; i <= w * h - 1; i++) {
		if (data[i] > MaxV) MaxV = data[i];
	}
	for (int i = 0; i <= w * h - 1; i++) {
		LogData[i] = iLog(data[i] + 1, MaxV + 1);
	}
	//求线性拉伸函数
	double MaxLog = 0, MinLog = 1;
	for (int i = 0; i <= w * h - 1; i++) {
		if (LogData[i] > MaxLog) MaxLog = LogData[i];
		if (LogData[i] < MinLog) MinLog = LogData[i];
	}
	double slope = 255 / (MaxLog - MinLog);
	//线性拉伸
	for (int i = 0; i <= w * h - 1; i++) {
		data[i] = (LogData[i] - MinLog)*slope + 0.5;
	}
	delete[] LogData;
}

//灰度映射创建函数
//参数：接受数据区，以及数据区尺寸作为参数
//返回值：返回原始灰度-新灰度映射
map<int, double> GeneMap(BYTE *data, int size) {
	//创建原始灰度-概率映射
	map<int, double> probabilities;
	for (int i = 0; i <= size - 1; i++) {
		if (probabilities.count(data[i]) == 0) {
			probabilities[data[i]] = 1;
		}
		else probabilities[data[i]]++;
	}
	map<int, double>::iterator it;
	for (it = probabilities.begin(); it != probabilities.end(); it++) {
		it->second /= size;
	}
	//创建原始灰度-新灰度映射
	map<int, double> NewGS;
	map<int, double>::iterator it2;
	for (it = probabilities.begin(); it != probabilities.end(); it++) {
		if (it == probabilities.begin()) {
			NewGS[it->first] = it->second;
			it2 = NewGS.begin();
		}
		else {
			NewGS[it->first] = NewGS[it2->first] + it->second;
			it2++;
		}
	}
	for (it = NewGS.begin(); it != NewGS.end(); it++) {
		//由于NewGS中存储的新灰度(double)会被强制转为BYTE，因而增加0.5来实现舍入效果
		NewGS[it->first] = NewGS[it->first] * 255 + 0.5;
	}
	return NewGS;
}

//灰度图片输出函数
//参数：接受文件头对象、信息头对象、调色盘、数据区、图像尺寸、文件名作为参数
//返回值：无返回值
void imwrite(BITMAPFILEHEADER file, BITMAPINFOHEADER info, RGBQUAD *palette, BYTE *data, int size, const char name[]) {
	FILE *fp;
	fp = fopen(name, "wb");
	fwrite(&(file.bfType), 1, sizeof(file.bfType), fp);
	fwrite(&(file.bfSize), 1, sizeof(file.bfSize), fp);
	fwrite(&(file.bfReserved1), 1, sizeof(file.bfReserved1), fp);
	fwrite(&(file.bfReserved2), 1, sizeof(file.bfReserved2), fp);
	fwrite(&(file.bfOffBits), 1, sizeof(file.bfOffBits), fp);
	fwrite(&info, 1, sizeof(info), fp);
	fwrite(palette, 1, 256 * sizeof(RGBQUAD), fp);
	fwrite(data, 1, size * sizeof(BYTE), fp);
	fclose(fp);
}