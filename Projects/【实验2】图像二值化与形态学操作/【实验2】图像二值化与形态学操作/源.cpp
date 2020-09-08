#include<iostream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"BMPDef.h"
using namespace std;

int OTSU(BYTE* data, int size);
void binarization(BYTE *data, int size, int t);
void AdaptiveBi(BYTE *data, int w, int h, int rblk, int cblk);
BYTE *dilation(BYTE *data, int w, int h, int value);
BYTE *erosion(BYTE *data, int w, int h, int value);
BYTE *opening(BYTE *data, int w, int h, int value);
BYTE *closing(BYTE *data, int w, int h, int value);
void imwrite(BITMAPFILEHEADER file, BITMAPINFOHEADER info, RGBQUAD *palette, BYTE *data, int size, const char name[]);

int main() {
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
	BYTE *GSData = new BYTE[ah*aw];
	fread(GSData, aw*ah * sizeof(BYTE), 1, fp);
	//全局二值化
	cout << "正在进行全局二值化...";
	BYTE *BiData = new BYTE[ah*aw];
	for (int i = 0; i <= aw * ah - 1; i++) {
		BiData[i] = GSData[i];
	}
	int treshold = OTSU(BiData, aw*ah);
	binarization(BiData, aw*ah, treshold);
	imwrite(file, info, palette, BiData, aw*ah, "BinarizedBMP.bmp");
	delete[] BiData;
	cout << "完成" << endl;
	//自适应二值化
	cout << "正在进行局部自适应二值化...";
	BYTE *AdBiData = new BYTE[ah*aw];
	for (int i = 0; i <= aw * ah - 1; i++) {
		AdBiData[i] = GSData[i];
	}
	AdaptiveBi(AdBiData, aw, ah, 2, 2);
	imwrite(file, info, palette, AdBiData, aw*ah, "AdaptiveBinarizedBMP.bmp");
	cout << "完成" << endl;
	//膨胀操作
	cout << "正在进行膨胀操作...";
	BYTE *DiData = dilation(AdBiData, aw, ah, 0);
	imwrite(file, info, palette, DiData, aw*ah, "DilatedBMP.bmp");
	delete[] DiData;
	cout << "完成" << endl;
	//腐蚀操作
	cout << "正在进行腐蚀操作...";
	BYTE *EroData = erosion(AdBiData, aw, ah, 0);
	imwrite(file, info, palette, EroData, aw*ah, "ErodedBMP.bmp");
	delete[] EroData;
	cout << "完成" << endl;
	//开操作
	cout << "正在进行开操作...";
	BYTE *OpData = opening(AdBiData, aw, ah, 0);
	imwrite(file, info, palette, OpData, aw*ah, "OpenedBMP.bmp");
	delete[] OpData;
	cout << "完成" << endl;
	//闭操作
	cout << "正在进行闭操作...";
	BYTE *CloData = closing(AdBiData, aw, ah, 0);
	imwrite(file, info, palette, CloData, aw*ah, "ClosedBMP.bmp");
	delete[] CloData;
	cout << "完成" << endl;
	//释放空间
	delete[] AdBiData;
	delete[] GSData;
	delete[] palette;
	fclose(fp);

	system("pause");
	return 0;
}

/*大津算法实现函数*/
/*参数：接受指向包含图像灰度信息的空间的BYTE型指针以及空间大小*/
/*返回值：使得类间方差最大的阈值*/
int OTSU(BYTE* data, int size) {
	int mings = 255;
	int maxgs = 0;
	for (int i = 0; i <= size - 1; i++) {
		if (data[i] > maxgs) maxgs = data[i];
		if (data[i] < mings) mings = data[i];
	}
	double avg_f, avg_b, portion_f, portion_b;
	int num_f = 0, num_b = 0, total_f = 0, total_b = 0;
	int treshold = mings;
	double var, tempvar;
	for (int gs = mings; gs <= maxgs - 1; gs++) {
		for (int i = 0; i <= size - 1; i++) {
			if (data[i] <= gs) {
				num_b++;
				total_b += data[i];
			}
			else {
				num_f++;
				total_f += data[i];
			}
		}
		portion_f = num_f / size;
		portion_b = num_b / size;
		avg_f = total_f / num_f;
		avg_b = total_b / num_b;
		tempvar = portion_f * portion_b*pow(avg_f - avg_b, 2);
		if (gs == mings) {
			treshold = mings;
			var = tempvar;
		}
		else {
			if (tempvar > var) {
				treshold = gs;
				var = tempvar;
			}
		}
	}
	return treshold;
}

/*二值化实现函数*/
/*参数：接受指向包含图像灰度信息的空间的BYTE型指针，空间大小以及阈值*/
/*返回值：无返回值*/
void binarization(BYTE *data, int size, int t) {
	for (int i = 0; i <= size - 1; i++) {
		if (data[i] <= t) data[i] = 0;
		else data[i] = 255;
	}
}

/*自适应二值化函数*/
/*参数：接受指向包含图像灰度信息的空间的BYTE型指针、图像宽度、图像高度、横向块数以及纵向块数*/
/*返回值：无返回值*/
/*注意：用户必须保证图像的行数能够整除纵向块数，列数能够整除横向块数*/
void AdaptiveBi(BYTE *data, int w, int h, int rblk, int cblk) {
	BYTE *block = new BYTE[w*h / (rblk*cblk)];
	int block_it, data_it; //data_it是作用于data数据区的迭代器，用于指示data中当前数据块有多少行已经写入block.
	int startpos; //指示当前data中数据块的起始位置（以下标形式指示位置）。
	int treshold;
	for (int i = 0; i <= rblk - 1; i++) {
		for (int j = 0; j <= cblk - 1; j++) {
			startpos = i * h / rblk * w + j * w / cblk;
			block_it = 0;
			//将data数据区中的块赋给block.
			for (data_it = 0; data_it <= h / rblk - 1; data_it++) {
				for (int k = 0; k <= w / cblk - 1; k++) {
					block[block_it++] = data[startpos + w * data_it + k];
				}
			}
			treshold = OTSU(block, h*w / (rblk*cblk));
			binarization(block, h*w / (rblk*cblk), treshold);
			//将block数据块写回data.
			block_it = 0;
			for (data_it = 0; data_it <= h / rblk - 1; data_it++) {
				for (int k = 0; k <= w / cblk - 1; k++) {
					data[startpos + w * data_it + k] = block[block_it++];
				}
			}
		}
	}
	delete[] block;
}

/*膨胀操作*/
/*参数：接受指向图像数据区的指针、图像宽度、图像高度以及目标（前景）的像素值*/
/*返回值：返回指向经膨胀操作的数据区的指针*/
BYTE *dilation(BYTE *data, int w, int h, int value) {
	BYTE *DiData = new BYTE[w*h];
	int pos; //指示结构元中心位置
	for (int i = 0; i <= w * h; i++) {
		DiData[i] = data[i];
	}
	for (int i = 1; i <= h - 2; i++) {
		for (int j = 1; j <= w - 2; j++) {
			pos = i * w + j;
			if (data[pos] == value) {
				continue;
			}
			else if (data[pos - 1] == value || data[pos + 1] == value || data[pos - w] == value || data[pos + w] == value) {
				DiData[pos] = value;
			}
		}
	}
	for (int i = 1; i <= h - 2; i++) {
		pos = i * w;
		if (data[pos] == value);
		else if (data[pos - w] == value || data[pos + 1] == value || data[pos + w] == value) {
			DiData[pos] = value;
		}
		pos += w - 1;
		if (data[pos] == value);
		else if (data[pos - w] == value || data[pos - 1] == value || data[pos + w] == value) {
			DiData[pos] = value;
		}
	}
	for (int i = 1; i <= w - 2; i++) {
		pos = i;
		if (data[pos] == value);
		else if (data[pos - 1] == value || data[pos + 1] == value || data[pos + w] == value) {
			DiData[pos] = value;
		}
		pos += (h - 1)*w;
		if (data[pos] == value);
		else if (data[pos - 1] == value || data[pos + 1] == value || data[pos - w] == value) {
			DiData[pos] = value;
		}
	}
	if (data[0] == value);
	else if (data[w] == value || data[1] == value) DiData[0] = value;
	if (data[w - 1] == value);
	else if (data[w - 2] == value || data[2 * w - 1] == value) DiData[w - 1] = value;
	if (data[(h - 1)*w] == value);
	else if (data[(h - 1)*w + 1] == value || data[(h - 2)*w] == value) DiData[(h - 1)*w] = value;
	if (data[h*w - 1] == value);
	else if (data[h*w - 2] == value || data[(h - 1)*w - 1] == value) DiData[h*w - 1] = value;
	return DiData;
}

/*腐蚀操作*/
/*参数：接受指向图像数据区的指针、图像宽度、图像高度以及目标（前景）的像素值*/
/*返回值：返回指向经腐蚀操作的数据区的指针*/
BYTE *erosion(BYTE *data, int w, int h, int value) {
	BYTE *EroData = new BYTE[w*h];
	int pos; //指示结构元中心位置
	if (value) value = 0;
	else value = 255;
	for (int i = 0; i <= w * h; i++) {
		EroData[i] = data[i];
	}
	for (int i = 1; i <= h - 2; i++) {
		for (int j = 1; j <= w - 2; j++) {
			pos = i * w + j;
			if (data[pos] == value) {
				continue;
			}
			else if (data[pos - w - 1] == value || data[pos - w] == value || data[pos - w + 1] == value || data[pos - 1] == value || data[pos + 1] == value || data[pos + w - 1] == value || data[pos + w] == value || data[pos + w + 1] == value) {
				EroData[pos] = value;
			}
		}
	}
	for (int i = 1; i <= h - 2; i++) {
		pos = i * w;
		if (data[pos] == value);
		else if (data[pos - w] == value || data[pos - w + 1] == value || data[pos + 1] == value || data[pos + w] == value || data[pos + w + 1] == value) {
			EroData[pos] = value;
		}
		pos += w - 1;
		if (data[pos] == value);
		else if (data[pos - w] == value || data[pos - w - 1] == value || data[pos - 1] == value || data[pos + w - 1] == value || data[pos + w] == value) {
			EroData[pos] = value;
		}
	}
	for (int i = 1; i <= w - 2; i++) {
		pos = i;
		if (data[pos] == value);
		else if (data[pos - 1] == value || data[pos + w - 1] == value || data[pos + w] == value || data[pos + w + 1] == value || data[pos + 1] == value) {
			EroData[pos] = value;
		}
		pos += (h - 1)*w;
		if (data[pos] == value);
		else if (data[pos - 1] == value || data[pos - w - 1] == value || data[pos - w] == value || data[pos - w + 1] == value || data[pos + 1] == value) {
			EroData[pos] = value;
		}
	}
	if (data[0] == value);
	else if (data[1] == value || data[w + 1] == value || data[w] == value) EroData[0] = value;
	if (data[w - 1] == value);
	else if (data[w - 2] == value || data[2 * w - 2] == value || data[2 * w - 1] == value) EroData[w - 1] = value;
	if (data[(h - 1)*w] == value);
	else if (data[(h - 2)*w] == value || data[(h - 2)*w + 1] == value || data[(h - 1)*w + 1] == value) EroData[(h - 1)*w] = value;
	if (data[h*w - 1] == value);
	else if (data[h*w - 2] == value || data[(h - 1)*w - 2] == value || data[(h - 1)*w - 1] == value) EroData[h*w - 1] = value;
	return EroData;
}

/*开操作*/
/*参数：接受指向图像数据区的指针、图像宽度、图像高度以及目标（前景）的像素值*/
/*返回值：返回指向经开操作的数据区的指针*/
BYTE *opening(BYTE *data, int w, int h, int value) {
	BYTE *temp;
	BYTE *OpData;
	temp = erosion(data, w, h, value);
	OpData = dilation(temp, w, h, value);
	delete[] temp;
	return OpData;
}

/*闭操作*/
/*参数：接受指向图像数据区的指针、图像宽度、图像高度以及目标（前景）的像素值*/
/*返回值：返回指向经闭操作的数据区的指针*/
BYTE *closing(BYTE *data, int w, int h, int value) {
	BYTE *temp;
	BYTE *CloData;
	temp = dilation(data, w, h, value);
	CloData = erosion(temp, w, h, value);
	delete[] temp;
	return CloData;
}

/*图片输出函数*/
/*参数：接受文件头对象、信息头对象、调色盘、数据区、图像尺寸、文件名作为参数*/
/*返回值：无返回值*/
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