#include "bmp.h"
#include <iostream>
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

BYTE& GetPixel(RGBInfo& info, int x, int y, int layer) {
	BYTE* data = info.data;
	int w_byte = info.width_byte;
	int index = (y - 1)*w_byte + (x - 1) * 3 + layer - 1;
	return data[index];
}
BYTE& GetPixel(BYTE* data, int w_byte, int x, int y, int layer) {
	int index = (y - 1)*w_byte + (x - 1) * 3 + layer - 1;
	return data[index];
}
int& GetPixel(int* data, int w_pixel, int x, int y, int layer) {
	int index = ((y - 1)*w_pixel + x - 1) * 3 + layer - 1;
	return data[index];
}
double& GetPixel(double* data, int w_pixel, int x, int y, int layer) {
	int index = ((y - 1)*w_pixel + x - 1) * 3 + layer - 1;
	return data[index];
}

int* RGB2Lab(RGBInfo& info) {
	int w = info.InfoHeader.biWidth;
	int h = info.InfoHeader.biHeight;
	//临时数据区，以double形式存储XYZ值
	double *temp = new double[h*w * 3];
	//L*a*b*数据区
	int *Lab = new int[w*h * 3];
	int x, y;
	int layer;
	for (y = 1; y <= h; y++) {
		for (x = 1; x <= w; x++) {
			//RGB转XYZ
			double X = GetPixel(temp, w, x, y, 1) = 0.412453*(GetPixel(info, x, y, 3)) + 0.357580*(GetPixel(info, x, y, 2)) + 0.180423*(GetPixel(info, x, y, 1));
			double Y = GetPixel(temp, w, x, y, 2) = 0.212671*(GetPixel(info, x, y, 3)) + 0.715160*(GetPixel(info, x, y, 2)) + 0.072169*(GetPixel(info, x, y, 1));
			double Z = GetPixel(temp, w, x, y, 3) = 0.019334*(GetPixel(info, x, y, 3)) + 0.119193*(GetPixel(info, x, y, 2)) + 0.950227*(GetPixel(info, x, y, 1));
			//XYZ归一化
			X = GetPixel(temp, w, x, y, 1) = X / 0.950456 / 255;
			Y = GetPixel(temp, w, x, y, 2) = Y / 255;
			Z = GetPixel(temp, w, x, y, 3) = Z / 1.088754 / 255;
			//XYZ转L*a*b*
			GetPixel(Lab, w, x, y, 1) = round(116 * f_Lab(Y) - 16);
			GetPixel(Lab, w, x, y, 2) = round(500 * (f_Lab(X) - f_Lab(Y)));
			GetPixel(Lab, w, x, y, 3) = round(200 * (f_Lab(Y) - f_Lab(Z)));
		}
	}
	delete[] temp;
	return Lab;
}
BYTE* Lab2RGB(int *Lab, int width_pixel, int width_byte, int height) {
	//临时数据区，以double形式存储XYZ值
	double *temp = new double[width_pixel*height * 3];
	int x, y, layer;
	//RGB数据区
	BYTE* RGB = new BYTE[width_byte*height]();
	for (y = 1; y <= height; y++) {
		for (x = 1; x <= width_pixel; x++) {
			int L = GetPixel(Lab, width_pixel, x, y, 1);
			int a = GetPixel(Lab, width_pixel, x, y, 2);
			int b = GetPixel(Lab, width_pixel, x, y, 3);
			//L*a*b*转XYZ
			double Y = if_Lab((L + 16) / 116.0);
			double X = if_Lab((L + 16) / 116.0 + a / 500.0);
			double Z = if_Lab((L + 16) / 116.0 - b / 200.0);
			//XYZ映射到[0,255]
			X = X * 0.950456 * 255;
			Y = Y * 255;
			Z = Z * 1.088754 * 255;
			//为RGB数据区赋值
			int r_tmp = round(3.240479*X - 1.537150*Y - 0.498535*Z);
			if (r_tmp > 255) r_tmp = 255;
			else if (r_tmp < 0) r_tmp = 0;
			GetPixel(RGB, width_byte, x, y, 3) = r_tmp;
			int g_tmp = round(-0.969256*X + 1.875992*Y + 0.041556*Z);
			if (g_tmp > 255) g_tmp = 255;
			else if (g_tmp < 0) g_tmp = 0;
			GetPixel(RGB, width_byte, x, y, 2) = g_tmp;
			int b_tmp = round(0.055648*X - 0.204043*Y + 1.057311*Z);
			if (b_tmp > 255) b_tmp = 255;
			else if (b_tmp < 0) b_tmp = 0;
			GetPixel(RGB, width_byte, x, y, 1) = b_tmp;
		}
	}
	delete[] temp;
	return RGB;
}
double f_Lab(double x) {
	if (x > pow(6 / 29.0, 3)) return pow(x, 1 / 3.0);
	else return pow(29 / 6.0, 2)*x / 3 + 4 / 29.0;
}
double if_Lab(double x) {
	if (x > 6 / 29.0) return pow(x, 3);
	else return pow(6 / 29.0, 2)*(x - 4 / 29.0) * 3;
}

RGBInfo bilateral_Lab(RGBInfo& image, double spa_std_devia, double int_std_devia) {
	double sig_s = spa_std_devia;
	double sig_r = int_std_devia;
	RGBInfo ret(image);
	//将RGB空间转为L*a*b*空间
	int *Lab_ori = RGB2Lab(image);
	//依据传入的空间域标准差确定窗口尺寸，并确保窗口边长为奇数
	int filter_w = round(2 * sig_s);
	if (filter_w % 2 == 0) filter_w++;
	int x, y, layer;
	int w = image.InfoHeader.biWidth;
	int h = image.InfoHeader.biHeight;
	int w_b = image.width_byte;
	//新建数据区，存放经过双边滤波的图像数据
	int *Lab_new = new int[h*w * 3]();
	//计算新数据区的值
	for (y = filter_w / 2 + 1; y <= h - filter_w / 2; y++) {
		for (x = 1 + filter_w / 2; x <= w - filter_w / 2; x++) {
			for (layer = 1; layer <= 3; layer++) {
				int i, j;
				double value = 0;
				double coef_sum = 0;
				//计算新像素值和系数
				for (i = -filter_w / 2; i <= filter_w / 2; i++) {
					for (j = -filter_w / 2; j <= filter_w / 2; j++) {
						//计算几何距离
						double spa_dist = sqrt(pow(i, 2) + pow(j, 2));
						//计算像素值差
						int int_dist = abs(GetPixel(Lab_ori, w, x, y, layer) - GetPixel(Lab_ori, w, x + i, y + j, layer));
						//调用norm_dis（一元正态分布密度函数）计算权和新像素值
						double coef = norm_dis(spa_dist, 0, pow(sig_s, 2))*norm_dis(int_dist, 0, pow(sig_r, 2));
						value += GetPixel(Lab_ori, w, x + i, y + j, layer)*coef;
						coef_sum += coef;
					}
				}
				//归一化
				value = round(value / coef_sum);
				//边界判断
				if (layer == 1) {
					if (value > 100) value = 100;
				}
				else {
					if (value > 127) value = 127;
					else if (value < -128) value = -128;
				}
				GetPixel(Lab_new, w, x, y, layer) = value;
			}
		}
	}
	delete[] Lab_ori;
	//边界补齐
	int margin = filter_w / 2;
	for (y = 1 + margin; y <= h - margin; y++) {
		for (layer = 1; layer <= 3; layer++) {
			for (x = 1; x <= margin; x++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, 1 + margin, y, layer);
			}
			for (x = w + 1 - margin; x <= w; x++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, w - margin, y, layer);
			}
		}
	}
	for (x = 1; x <= w; x++) {
		for (layer = 1; layer <= 3; layer++) {
			for (y = 1; y <= margin; y++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, x, 1 + margin, layer);
			}
			for (y = h + 1 - margin; y <= h; y++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, x, h - margin, layer);
			}
		}
	}
	//转回RGB空间
	ret.data = Lab2RGB(Lab_new, w, w_b, h);
	delete[] Lab_new;
	return ret;
}
RGBInfo bilateral(RGBInfo& image, double spa_std_devia, double int_std_devia) {
	double sig_s = spa_std_devia;
	double sig_r = int_std_devia;
	RGBInfo ret(image);
	int filter_w = round(2 * sig_s);
	if (filter_w % 2 == 0) filter_w++;
	int x, y, layer;
	int w = image.InfoHeader.biWidth;
	int h = image.InfoHeader.biHeight;
	int w_b = image.width_byte;
	ret.data = new BYTE[w_b*h];
	//计算新数据区的值
	for (y = filter_w / 2 + 1; y <= h - filter_w / 2; y++) {
		for (x = 1 + filter_w / 2; x <= w - filter_w / 2; x++) {
			for (layer = 1; layer <= 3; layer++) {
				int i, j;
				double value = 0;
				double coef_sum = 0;
				//计算新像素值和系数
				for (i = -filter_w / 2; i <= filter_w / 2; i++) {
					for (j = -filter_w / 2; j <= filter_w / 2; j++) {
						double spa_dist = sqrt(pow(i, 2) + pow(j, 2));
						int int_dist = abs(GetPixel(image, x, y, layer) - GetPixel(image, x + i, y + j, layer));
						double coef = norm_dis(spa_dist, 0, pow(sig_s, 2))*norm_dis(int_dist, 0, pow(sig_r, 2));
						value += GetPixel(image, x + i, y + j, layer)*coef;
						coef_sum += coef;
					}
				}
				value = round(value / coef_sum);
				//边界判断
				if (value > 255) value = 255;
				GetPixel(ret, x, y, layer) = value;
			}
		}
	}
	//边界补齐
	int margin = filter_w / 2;
	for (y = 1 + margin; y <= h - margin; y++) {
		for (layer = 1; layer <= 3; layer++) {
			for (x = 1; x <= margin; x++) {
				GetPixel(ret, x, y, layer) = GetPixel(ret, 1 + margin, y, layer);
			}
			for (x = w + 1 - margin; x <= w; x++) {
				GetPixel(ret, x, y, layer) = GetPixel(ret, w - margin, y, layer);
			}
		}
	}
	for (x = 1; x <= w; x++) {
		for (layer = 1; layer <= 3; layer++) {
			for (y = 1; y <= margin; y++) {
				GetPixel(ret, x, y, layer) = GetPixel(ret, x, 1 + margin, layer);
			}
			for (y = h + 1 - margin; y <= h; y++) {
				GetPixel(ret, x, y, layer) = GetPixel(ret, x, h - margin, layer);
			}
		}
	}
	return ret;
}
RGBInfo bilateral_quan(RGBInfo& image, double spa_std_devia, double int_std_devia) {
	double sig_s = spa_std_devia;
	double sig_r = int_std_devia;
	RGBInfo ret(image);
	int *Lab_ori = RGB2Lab(image);
	int filter_w = round(2 * sig_s);
	if (filter_w % 2 == 0) filter_w++;
	int x, y, layer;
	int w = image.InfoHeader.biWidth;
	int h = image.InfoHeader.biHeight;
	int w_b = image.width_byte;
	int *Lab_new = new int[h*w * 3]();
	//计算新数据区的值
	for (y = filter_w / 2 + 1; y <= h - filter_w / 2; y++) {
		for (x = 1 + filter_w / 2; x <= w - filter_w / 2; x++) {
			for (layer = 1; layer <= 3; layer++) {
				int i, j;
				double value = 0;
				double coef_sum = 0;
				//计算新像素值和系数
				for (i = -filter_w / 2; i <= filter_w / 2; i++) {
					for (j = -filter_w / 2; j <= filter_w / 2; j++) {
						double spa_dist = abs(i) + abs(j);
						int int_dist = abs(GetPixel(Lab_ori, w, x, y, layer) - GetPixel(Lab_ori, w, x + i, y + j, layer));
						double coef = norm_dis(spa_dist, 0, pow(sig_s, 2))*norm_dis(int_dist, 0, pow(sig_r, 2));
						value += GetPixel(Lab_ori, w, x + i, y + j, layer)*coef;
						coef_sum += coef;
					}
				}
				value = round(value / coef_sum);
				//边界判断
				if (layer == 1) {
					if (value > 100) value = 100;
				}
				else {
					if (value > 127) value = 127;
					else if (value < -128) value = -128;
				}
				GetPixel(Lab_new, w, x, y, layer) = value;
			}
		}
	}
	delete[] Lab_ori;
	//边界补齐
	int margin = filter_w / 2;
	for (y = 1 + margin; y <= h - margin; y++) {
		for (layer = 1; layer <= 3; layer++) {
			for (x = 1; x <= margin; x++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, 1 + margin, y, layer);
			}
			for (x = w + 1 - margin; x <= w; x++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, w - margin, y, layer);
			}
		}
	}
	for (x = 1; x <= w; x++) {
		for (layer = 1; layer <= 3; layer++) {
			for (y = 1; y <= margin; y++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, x, 1 + margin, layer);
			}
			for (y = h + 1 - margin; y <= h; y++) {
				GetPixel(Lab_new, w, x, y, layer) = GetPixel(Lab_new, w, x, h - margin, layer);
			}
		}
	}
	ret.data = Lab2RGB(Lab_new, w, w_b, h);
	delete[] Lab_new;
	return ret;
}
double norm_dis(double x, double expect, double var) {
	return 1 / sqrt(2 * pi*var)*exp(-pow(x - expect, 2) / (2 * var));
}