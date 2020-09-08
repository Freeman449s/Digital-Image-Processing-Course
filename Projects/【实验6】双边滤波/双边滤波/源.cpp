#include<iostream>
#include<string>
#include"bmp.h"
using namespace std;

int main() {
	cout << "此程序用于对图像进行双边滤波处理。" << endl;
	cout << "请输入同级目录下BMP图像的完整名称（包含后缀名）：" << endl;
	string name;
	cin >> name;
	RGBInfo origin = imread_RGB(name.c_str());
	while (origin.data == NULL) {
		cout << "请重新输入文件名：" << endl;
		cin >> name;
		origin = imread_RGB(name.c_str());
	}
	cout << "请输入空间域标准差与强度域标准差，中间以一个空格分隔：" << endl;
	double sig_s, sig_r;
	cin >> sig_s >> sig_r;
	cout << "正在进行双边滤波处理...";
	RGBInfo bi = bilateral_Lab(origin, sig_s, sig_r);
	imwrite(bi, "Bilaterally Filtered.bmp");
	cout << "完成" << endl;

	system("pause");
	return 0;
}