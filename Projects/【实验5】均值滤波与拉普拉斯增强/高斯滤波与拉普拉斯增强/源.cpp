#include<iostream>
#include"bmp.h"
using namespace std;

RGBInfo median_filtering(RGBInfo& origin);
RGBInfo laplacian_enhancement(RGBInfo& origin);
RGBInfo get_difference(RGBInfo& img_a, RGBInfo& img_b);

int main() {
	RGBInfo origin = imread_RGB("Original Image.bmp");
	//高斯滤波
	cout << "均值滤波...";
	RGBInfo temp1 = median_filtering(origin);
	RGBInfo temp2 = median_filtering(temp1);
	delete[] temp1.data;
	RGBInfo temp3 = median_filtering(temp2);
	delete[] temp2.data;
	RGBInfo temp4 = median_filtering(temp3);
	delete[] temp3.data;
	RGBInfo gaussian = median_filtering(temp4);
	delete[] temp4.data;
	imwrite(gaussian, "Median Filtered Image.bmp");
	delete[] gaussian.data;
	cout << "完成" << endl;
	//拉普拉斯增强
	cout << "拉普拉斯增强...";
	RGBInfo laplacian = laplacian_enhancement(origin);
	imwrite(laplacian, "Laplacian Enhanced Image.bmp");
	cout << "完成" << endl;
	//求残差
	cout << "求残差...";
	RGBInfo difference = get_difference(origin, laplacian);
	imwrite(difference, "Differential Image.bmp");
	cout << "完成" << endl;

	delete[] difference.data;
	delete[] laplacian.data;
	delete[] origin.data;
	system("pause");
	return 0;
}

//均值滤波函数
//参数：接受原始图像的RGBInfo对象
//返回值：返回经过中值滤波的图像的RGBInfo对象
RGBInfo median_filtering(RGBInfo& origin) {
	RGBInfo new_img(origin);
	int w = origin.InfoHeader.biWidth;
	int w_b = origin.width_byte;
	int h = origin.InfoHeader.biHeight;
	BYTE* New = new_img.data = new BYTE[w_b*h]();
	int x, y, layer;
	//对内部进行均值滤波
	for (y = 3; y <= h - 2; y++) {
		for (x = 3; x <= w - 2; x++) {
			for (layer = 1; layer <= 3; layer++) {
				double sum = 0;
				int i, j;
				//对以响应位置为中心的5*5矩阵内的像素求和
				for (i = -2; i <= 2; i++) {
					for (j = -2; j <= 2; j++) {
						sum += GetPixel(origin, x + i, y + j, layer);
					}
				}
				GetPixel(new_img, x, y, layer) = round(sum / 25);
			}
		}
	}
	//用新图像的内部像素为边界赋值
	for (y = 3; y <= h - 2; y++) {
		for (x = 1; x <= 2; x++) {
			for (layer = 1; layer <= 3; layer++) {
				GetPixel(new_img, x, y, layer) = GetPixel(new_img, 3, y, layer);
			}
		}
		for (x = w-1; x <= w; x++) {
			for (layer = 1; layer <= 3; layer++) {
				GetPixel(new_img, x, y, layer) = GetPixel(new_img, w - 2, y, layer);
			}
		}
	}
	for (x = 1; x <= w; x++) {
		for (y = 1; y <= 2; y++) {
			for (layer = 1; layer <= 3; layer++) {
				GetPixel(new_img, x, y, layer) = GetPixel(new_img, x, 3, layer);
			}
		}
		for (y = h-1; y <= h; y++) {
			for (layer = 1; layer <= 3; layer++) {
				GetPixel(new_img, x, y, layer) = GetPixel(new_img, x, h - 2, layer);
			}
		}
	}
	return new_img;
}

//拉普拉斯增强函数
//参数：接受原始图像的RGBInfo结构体
//返回值：返回经过拉普拉斯增强的图像的RGBInfo结构体
RGBInfo laplacian_enhancement(RGBInfo& origin) {
	int* Lab_ori = RGB2Lab(origin);
	int x, y;
	int w = origin.InfoHeader.biWidth;
	int h = origin.InfoHeader.biHeight;
	int w_b = origin.width_byte;
	//只对L层进行处理
	int* L_new = new int[w*h];
	for (y = 2; y <= h - 1; y++) {
		for (x = 2; x <= w - 1; x++) {
			int i, j;
			int sum = 0;
			//对以响应位置为中心的3*3矩阵内像素的负值求和
			for (i = -1; i <= 1; i++) {
				for (j = -1; j <= 1; j++) {
					sum -= GetPixel(Lab_ori, w, x + i, y + j, 1);
				}
			}
			sum += GetPixel(Lab_ori, w, x, y, 1) * 9;
			//将滤波器的相应除以一个系数来获得更好的效果
			L_new[(y - 1)*w + (x - 1)] = GetPixel(Lab_ori, w, x, y, 1) + round(sum / 12.0);
			if (L_new[(y - 1)*w + (x - 1)] > 100) L_new[(y - 1)*w + (x - 1)] = 100;
			else if (L_new[(y - 1)*w + (x - 1)] < 0) L_new[(y - 1)*w + (x - 1)] = 0;
		}
	}
	//用新L数据区的内部补齐边界
	for (y = 2; y <= h - 1; y++) {
		L_new[(y - 1)*w] = L_new[(y - 1)*w + 1];
		L_new[(y - 1)*w + (w - 1)] = L_new[(y - 1)*w + (w - 2)];
	}
	for (x = 1; x <= w; x++) {
		L_new[x - 1] = L_new[w + (x - 1)];
		L_new[(h - 1)*w + (x - 1)] = L_new[(h - 2)*w + (x - 1)];
	}
	//用新L数据替换旧数据
	for (y = 1; y <= h; y++) {
		for (x = 1; x <= w; x++) {
			GetPixel(Lab_ori, w, x, y, 1) = L_new[(y - 1)*w + (x - 1)];
		}
	}
	//Lab转RGB
	RGBInfo img_new(origin);
	img_new.data = Lab2RGB(Lab_ori, w, w_b, h);
	delete[] L_new;
	return img_new;
}
RGBInfo get_difference(RGBInfo& img_a, RGBInfo& img_b) {
	RGBInfo diff(img_a);
	int w = img_a.InfoHeader.biWidth;
	int h = img_a.InfoHeader.biHeight;
	int w_b = img_a.width_byte;
	diff.data = new BYTE[w_b*h]();
	int x, y, layer;
	for (y = 1; y <= h; y++) {
		for (x = 1; x <= w; x++) {
			for (layer = 1; layer <= 3; layer++) {
				GetPixel(diff, x, y, layer) = abs(GetPixel(img_a, x, y, layer) - GetPixel(img_b, x, y, layer));
			}
		}
	}
	return diff;
}