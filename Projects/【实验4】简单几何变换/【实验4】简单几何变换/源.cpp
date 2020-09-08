#include<iostream>
#include<string>
#include<math.h>
#include<vector>
#include"bmp.h"
using namespace std;

#define pi 3.14159

//函数原型声明
BYTE* GetPixel(RGBInfo* info, int x, int y, int layer);
int* GetPixel(int* data, int width, int x, int y, int layer);
RGBInfo Translation(RGBInfo* Origin, int DeltaX, int DeltaY);
RGBInfo Rotation(RGBInfo* Origin, double theta);
void Bilinear(int* data, int width, int height, int x, int y);
void BubbleSort(int arr[], int size);
int NewX_r(int OriginX, int OriginY, int deltaX, double theta);
int NewY_r(int OriginX, int OriginY, int deltaY, double theta);
int OriginX_r(int NewX, int NewY, int deltaX, int deltaY, double theta);
int OriginY_r(int NewX, int NewY, int deltaX, int deltaY, double theta);
bool isHole(RGBInfo *info, int *data, int NewW, int NewX, int NewY, int deltaX, int deltaY, double theta);
RGBInfo Scale(RGBInfo *origin, double CoefX, double CoefY);
RGBInfo Shear(RGBInfo *origin, char axis, double Coef);
RGBInfo Mirror(RGBInfo *origin, char axis);

int main() {
	cout << "(C)2019, Gordon Freeman" << endl;
	string name;
	cout << "本程序用于对BMP图像进行简单的几何变换。" << endl;
	cout << "用户需要将待变换的图像置于程序的同级目录下，并输入包含后缀名在内的完整文件名。" << endl;
	cout << "现在，请输入完整的文件名：" << endl;
	RGBInfo origin;
	while (true) {
		getline(cin, name);
		origin = imread_RGB(name.c_str());
		if (origin.width_byte == 0) {
			cout << "文件不存在，请重新输入文件名：" << endl;
		}
		else break;
	}
	BYTE* OriginData = origin.data;
	cout << "键入需要执行的操作。" << endl;
	cout << "请注意：重复执行同类型操作将覆盖上一次得到的文件。" << endl;
	cout << "1)平移 2)镜像 3)错切 4)缩放 5)旋转 0)退出" << endl;
	int op;
	while (true) {
		scanf("%d", &op);
		//平移
		if (op == 1) {
			int dx, dy;
			cout << "请输入横向及纵向的移动距离（像素），中间以一个空格分隔。" << endl;
			cout << "提示：输入负值来向左 / 向下移动。" << endl;
			scanf("%d %d", &dx, &dy);
			RGBInfo translated = Translation(&origin, dx, dy);
			imwrite(translated, "Translated Image.bmp");
			delete[] translated.data;
			cout << "平移变换完成。" << endl;
		}
		//镜像
		else if (op == 2) {
			char axis;
			cout << "请输入镜像变换的方向（以X或Y表示）:" << endl;
			rewind(stdin);
			scanf("%c", &axis);
			RGBInfo mirrored = Mirror(&origin, axis);
			imwrite(mirrored, "Mirrored Imgae.bmp");
			delete[] mirrored.data;
			cout << "镜像变换完成。" << endl;
		}
		//错切
		else if (op == 3) {
			char axis;
			double Coef;
			cout << "请输入错切变换的方向（以X或Y表示）及错切系数，中间以一个空格分隔。" << endl;
			cout << "提示：输入负值来向左 / 向下偏移。" << endl;
			rewind(stdin);
			scanf("%c %lf", &axis, &Coef);
			RGBInfo sheared = Shear(&origin, axis, Coef);
			imwrite(sheared, "Sheared Image.bmp");
			delete[] sheared.data;
			cout << "错切变换完成。" << endl;
		}
		//缩放
		else if (op == 4) {
			double cx, cy;
			cout << "请分别输入横向及纵向的缩放系数，中间以一个空格分隔。"<<endl;
			cout << "提示：系数大于1表示拉伸，小于1表示压缩。" << endl;
			scanf("%lf %lf", &cx, &cy);
			RGBInfo scaled = Scale(&origin, 2, 1);
			imwrite(scaled, "Scaled Image.bmp");
			delete[] scaled.data;
			cout << "缩放变换完成。" << endl;
		}
		//旋转
		else if (op == 5) {
			double angle;
			cout << "请输入旋转角度（弧度制）。目前只支持纯数字输入。" << endl;
			scanf("%lf", &angle);
			RGBInfo rotated = Rotation(&origin, angle);
			imwrite(rotated, "rotated image.bmp");
			delete[] rotated.data;
			cout << "旋转变换完成。" << endl;
		}
		else if (op == 0) break;
		else {
			cout << "操作代号不合法，请重新输入：" << endl;
			continue;
		}
		cout << "键入需要执行的操作。" << endl;
		cout << "请注意：重复执行同类型操作将覆盖上一次得到的文件。" << endl;
		cout << "1)平移 2)镜像 3)错切 4)缩放 5)旋转 0)退出" << endl;
	}

	cout << "感谢您的使用：)" << endl;
	system("pause");
	return 0;
}

//取像素函数
//参数：接收RGB图像信息（图像信息应为对象，下同）指针，横、纵坐标以及层数作为参数
//返回值：返回指向所取像素指定层数据的指针
//注意：RGB值在外存中以B,G,R的顺序存储
//     函数认为数据区中第一个像素的坐标为(1,1)
BYTE* GetPixel(RGBInfo* info, int x, int y, int layer) {
	BYTE* data = info->data;
	int w_byte = info->width_byte;
	int index = (y - 1)*w_byte + (x - 1) * 3 + layer - 1;
	return &data[index];
}
//重载取像素函数，适用于数据区为整型的情况，用于旋转变换
int* GetPixel(int* data, int w_pixel, int x, int y, int layer) {
	int index = ((y - 1)*w_pixel + x - 1) * 3 + layer - 1;
	return &data[index];
}

//平移函数
//参数：接受原图信息指针，横向与纵向平移量作为参数
//返回值：返回平移图像信息
RGBInfo Translation(RGBInfo* Origin, int DeltaX, int DeltaY) {
	RGBInfo New(*Origin);
	int nw = New.InfoHeader.biWidth = Origin->InfoHeader.biWidth + abs(DeltaX);
	int nh = New.InfoHeader.biHeight = Origin->InfoHeader.biHeight + abs(DeltaY);
	//计算行字节数
	New.width_byte = (nw*New.InfoHeader.biBitCount + 31) / 32 * 4;
	int nw_b = New.width_byte;
	New.data = new BYTE[nh*nw_b];
	BYTE* NewData = New.data;
	memset(NewData, 0, nh*nw_b);
	int start_X, start_Y;
	if (DeltaX <= 0) start_X = 0;
	else start_X = DeltaX;
	if (DeltaY <= 0) start_Y = 0;
	else start_Y = DeltaY;
	int x, y, layer;
	for (y = 1; y <= Origin->InfoHeader.biHeight; y++) {
		for (x = 1; x <= Origin->InfoHeader.biWidth; x++) {
			for (layer = 1; layer <= 3; layer++) {
				*GetPixel(&New, start_X + x, start_Y + y, layer) = *GetPixel(Origin, x, y, layer);
			}
		}
	}
	//修改其他图像信息
	New.FileHeader.bfSize = 56 + nw_b * nh;
	New.InfoHeader.biSizeImage = nw_b * nh + 2;
	return New;
}

//旋转函数
//参数：接受原图信息指针，旋转角度（弧度制）作为参数
//返回值：返回旋转图像信息
RGBInfo Rotation(RGBInfo* Origin, double theta) {
	RGBInfo New(*Origin);
	int oh = Origin->InfoHeader.biHeight;
	int ow = Origin->InfoHeader.biWidth;
	int ow_b = Origin->width_byte;
	//计算新数据区大小
	int arr[4];
	arr[0] = 1;
	arr[1] = round(-(oh - 1)*sin(theta) + 1);
	arr[2] = round((ow - 1)*cos(theta) + 1);
	arr[3] = round((ow - 1)*cos(theta) - (oh - 1)*sin(theta) + 1);
	BubbleSort(arr, 4);
	int nw = New.InfoHeader.biWidth = arr[3] - arr[0] + 1;
	//计算行字节数
	int nw_b = New.width_byte = (nw*New.InfoHeader.biBitCount + 31) / 32 * 4;
	int deltaX = -arr[0] + 1; //依据三角函数计算出的过渡坐标，需要加上deltaX才能得到新画布下的横坐标
	arr[0] = 0;
	arr[1] = round((oh - 1) * cos(theta) + 1);
	arr[2] = round((ow - 1)*sin(theta) + 1);
	arr[3] = round((ow - 1)*sin(theta) + (oh - 1) * cos(theta) + 1);
	BubbleSort(arr, 4);
	int nh = New.InfoHeader.biHeight = arr[3] - arr[0] + 1;
	int deltaY = -arr[0] + 1; //依据三角函数计算出的过渡坐标，需要加上deltaY才能得到新画布下的纵坐标
	int *TmpData = new int[nw*nh * 3];
	memset(TmpData, -1, nw*nh * 3 * sizeof(int));
	//将原图投至临时数据区
	int ox, oy, layer;
	int nx, ny;
	for (oy = 1; oy <= oh; oy++) {
		for (ox = 1; ox <= ow; ox++) {
			for (layer = 1; layer <= 3; layer++) {
				nx = NewX_r(ox, oy, deltaX, theta);
				ny = NewY_r(ox, oy, deltaY, theta);
				*GetPixel(TmpData, nw, nx, ny, layer) = *GetPixel(Origin, ox, oy, layer);
			}
		}
	}
	//补齐空洞
	for (ny = 1; ny <= nh; ny++) {
		for (nx = 1; nx <= nw; nx++) {
			if (*GetPixel(TmpData, nw, nx, ny, 1) == -1 || *GetPixel(TmpData, nw, nx, ny, 2) == -1 || *GetPixel(TmpData, nw, nx, ny, 3) == -1) {
				if (isHole(Origin, TmpData, nw, nx, ny, deltaX, deltaY, theta)) {
					Bilinear(TmpData, nw, nh, nx, ny);
				}
				else {
					for (layer = 1; layer <= 3; layer++) {
						*GetPixel(TmpData, nw, nx, ny, layer) = 0;
					}
				}
			}
		}
	}
	//产生新数据区
	New.data = new BYTE[nw_b*nh];
	for (ny = 1; ny <= nh; ny++) {
		for (nx = 1; nx <= nw; nx++) {
			for (layer = 1; layer <= 3; layer++) {
				*GetPixel(&New, nx, ny, layer) = *GetPixel(TmpData, nw, nx, ny, layer);
			}
		}
		//多余字节补0
		for (nx = nw * 3 + 1; nx <= nw_b; nx++) {
			New.data[(ny - 1)*nw_b + nx - 1] = 0;
		}
	}
	delete[] TmpData;
	//修改其他图像信息
	New.FileHeader.bfSize = 56 + nw_b * nh;
	New.InfoHeader.biSizeImage = nw_b * nh + 2;
	return New;
}

//双线性插值函数
//参数：接收整型临时数据区，数据区宽/高，像素坐标作为参数
//返回值：无返回值
//注意：传入临时数据区应为已赋值的数据区
void Bilinear(int* data, int width, int height, int x, int y) {
	int layer;
	//(x,y)位于左下角
	if (x == 1 || y == 1) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = *GetPixel(data, width, x + 1, y + 1, layer);
		}
	}
	//(x,y)位于右下角
	else if (x == width && height == 1) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = *GetPixel(data, width, x - 1, y + 1, layer);
		}
	}
	//(x,y)位于左上角
	else if (x == 1 && y == height) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = *GetPixel(data, width, x + 1, y - 1, layer);
		}
	}
	//(x,y)位于右上角
	else if (x == width && y == height) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = *GetPixel(data, width, x - 1, y - 1, layer);
		}
	}
	//(x,y)位于左边
	else if (x == 1) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = (*GetPixel(data, width, x + 1, y + 1, layer) + *GetPixel(data, width, x + 1, y - 1, layer)) / 2.0 + 0.5;
		}
	}
	//(x,y)位于底边
	else if (y == 1) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = (*GetPixel(data, width, x - 1, y + 1, layer) + *GetPixel(data, width, x + 1, y + 1, layer)) / 2.0 + 0.5;
		}
	}
	//(x,y)位于右边
	else if (x == width) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = (*GetPixel(data, width, x - 1, y + 1, layer) + *GetPixel(data, width, x - 1, y - 1, layer)) / 2.0 + 0.5;
		}
	}
	//(x,y)位于顶边
	else if (y == height) {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = (*GetPixel(data, width, x - 1, y - 1, layer) + *GetPixel(data, width, x + 1, y - 1, layer)) / 2.0 + 0.5;
		}
	}
	//(x,y)位于内部
	else {
		for (layer = 1; layer <= 3; layer++) {
			*GetPixel(data, width, x, y, layer) = (*GetPixel(data, width, x - 1, y - 1, layer) + *GetPixel(data, width, x - 1, y + 1, layer) + *GetPixel(data, width, x + 1, y - 1, layer) + *GetPixel(data, width, x + 1, y + 1, layer)) / 4.0 + 0.5;
		}
	}
}

//冒泡排序函数（升序）
//参数：接受整型数组，数组尺寸作为参数
//返回值：无返回值
void BubbleSort(int arr[], int size) {
	int i, j;
	int temp;
	for (i = 1; i <= size - 1; i++) {
		for (j = 0; j <= size - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
}

//用于计算旋转变换中，像素点在新画布下横/纵坐标的函数
//参数：接受原始坐标，坐标系平移量，以及旋转角度作为参数
//返回值：返回新横/纵坐标
int NewX_r(int OriginX, int OriginY, int deltaX, double theta) {
	return round((OriginX - 1)*cos(theta) - (OriginY - 1) * sin(theta)) + 1 + deltaX;
}
int NewY_r(int OriginX, int OriginY, int deltaY, double theta) {
	return round((OriginX - 1)*sin(theta) + (OriginY - 1) * cos(theta)) + 1 + deltaY;
}

//取原坐标函数（旋转情形）
//参数：接受新坐标，坐标系平移量，以及旋转角度作为参数
//返回值：返回原始横/纵坐标
int OriginX_r(int NewX, int NewY, int deltaX, int deltaY, double theta) {
	int tranX = NewX - deltaX;
	int tranY = NewY - deltaY;
	return round((tranX - 1)*cos(theta) + (tranY - 1) * sin(theta)) + 1;
}
int OriginY_r(int NewX, int NewY, int deltaX, int deltaY, double theta){
	int tranX = NewX - deltaX;
	int tranY = NewY - deltaY;
	return round(-(tranX - 1) * sin(theta) + (tranY - 1) * cos(theta)) + 1;
}

//判断旋转变换得到的临时数据区中的某像素是否是图像内部空洞
//参数：接受原图信息指针，临时数据区，临时数据区宽度，新坐标，坐标系平移量，旋转角度作为参数
//返回值：若是图像内部空洞则返回true，否则返回false
bool isHole(RGBInfo *info, int *data, int NewW, int NewX, int NewY, int deltaX, int deltaY, double theta) {
	int ox = OriginX_r(NewX, NewY, deltaX, deltaY, theta);
	int oy = OriginY_r(NewX, NewY, deltaX, deltaY, theta);
	int layer;
	if (ox<1 || ox>info->InfoHeader.biWidth || oy<1 || oy>info->InfoHeader.biHeight) return false;
	else return true;
}

//缩放函数
//参数：接受图像信息指针，x方向/y方向缩放倍数作为参数
//返回值：返回缩放图像信息
RGBInfo Scale(RGBInfo *origin, double CoefX, double CoefY) {
	RGBInfo New(*origin);
	int ow = origin->InfoHeader.biWidth;
	int oh = origin->InfoHeader.biHeight;
	int ow_b = origin->width_byte;
	int nw = New.InfoHeader.biWidth = round(ow*CoefX);
	int nh = New.InfoHeader.biHeight = round(oh*CoefY);
	int nw_b = New.width_byte = (nw*New.InfoHeader.biBitCount + 31) / 32 * 4;
	New.data = new BYTE[nw_b*nh];
	memset(New.data, 0, nw_b*nh * sizeof(BYTE));
	int nx, ny, layer;
	int ox, oy;
	for (ny = 1; ny <= nh; ny++) {
		oy = round(ny / CoefY);
		for (nx = 1; nx <= nw; nx++) {
			ox = round(nx / CoefX);
			for (layer = 1; layer <= 3; layer++) {
				*GetPixel(&New, nx, ny, layer) = *GetPixel(origin, ox, oy, layer);
			}
		}
	}
	//修改其他图像信息
	New.FileHeader.bfSize = 56 + nw_b * nh;
	New.InfoHeader.biSizeImage = nw_b * nh + 2;
	return New;
}

//错切函数
//参数：接受图像信息指针，错切方向，错切系数作为参数
//返回值：返回错切图像信息。传入错切方向出错将返回原图像信息
RGBInfo Shear(RGBInfo *origin, char axis, double Coef) {
	RGBInfo New(*origin);
	int ow = origin->InfoHeader.biWidth;
	int oh = origin->InfoHeader.biHeight;
	int ow_b = origin->width_byte;
	int nw, nh, nw_b;
	int ox, oy, layer;
	int nx, ny;
	//横向错切
	if (axis == 'X' || axis == 'x') {
		int deltaX;
		nh = New.InfoHeader.biHeight = oh;
		if (Coef < 0) {
			deltaX = -round(1 + oh * Coef) + 1;
			nw = New.InfoHeader.biWidth = ow + deltaX;
		}
		else {
			deltaX = 0;
			nw = New.InfoHeader.biWidth = round(ow + oh * Coef);
		}
		nw_b = New.width_byte = (nw*New.InfoHeader.biBitCount + 31) / 32 * 4;
		New.data = new BYTE[nw_b*nh];
		memset(New.data, 0, nw_b*nh * sizeof(BYTE));
		for (oy = 1; oy <= oh; oy++) {
			ny = oy;
			for (ox = 1; ox <= ow; ox++) {
				nx = round(ox + oy * Coef) + deltaX;
				for (layer = 1; layer <= 3; layer++) {
					*GetPixel(&New, nx, ny, layer) = *GetPixel(origin, ox, oy, layer);
				}
			}
		}
	}
	//纵向错切
	else if (axis == 'Y' || axis == 'y') {
		int deltaY;
		if (Coef < 0) {
			deltaY = -round(1 + ow * Coef) + 1;
			nh = New.InfoHeader.biHeight = oh + deltaY;
		}
		else {
			deltaY = 0;
			nh = New.InfoHeader.biHeight = round(oh + ow * Coef);
		}
		nw = New.InfoHeader.biWidth = ow;
		nw_b = origin->width_byte;
		New.data = new BYTE[nw_b*nh];
		memset(New.data, 0, nw_b*nh * sizeof(BYTE));
		for (ox = 1; ox <= ow; ox++) {
			nx = ox;
			for (oy = 1; oy <= oh; oy++) {
				ny = round(oy + Coef * ox) + deltaY;
				for (layer = 1; layer <= 3; layer++) {
					*GetPixel(&New, nx, ny, layer) = *GetPixel(origin, ox, oy, layer);
				}
			}
		}
	}
	//修改其他图像信息
	New.FileHeader.bfSize = 56 + nw_b * nh;
	New.InfoHeader.biSizeImage = nw_b * nh + 2;
	return New;
}

//镜像函数
//参数：接受图像信息指针，镜像方向作为参数
//返回值：返回镜像图像信息。传入镜像方向出错将返回原始图像信息
RGBInfo Mirror(RGBInfo *origin, char axis) {
	RGBInfo New(*origin);
	int w = New.InfoHeader.biWidth;
	int h = New.InfoHeader.biHeight;
	int w_b = New.width_byte;
	int ox, oy, nx, ny, layer;
	if (axis == 'X' || axis == 'x') {
		New.data = new BYTE[h*w_b];
		for (oy = 1; oy <= h; oy++) {
			ny = oy;
			for (ox = 1; ox <= w; ox++) {
				nx = w - ox + 1;
				for (layer = 1; layer <= 3; layer++) {
					*GetPixel(&New, nx, ny, layer) = *GetPixel(origin, ox, oy, layer);
				}
			}
		}
	}
	else if (axis == 'Y' || axis == 'y') {
		New.data = new BYTE[h*w_b];
		for (oy = 1; oy <= h; oy++) {
			ny = h - oy + 1;
			for (ox = 1; ox <= w; ox++) {
				nx = ox;
				for (layer = 1; layer <= 3; layer++) {
					*GetPixel(&New, nx, ny, layer) = *GetPixel(origin, ox, oy, layer);
				}
			}
		}
	}
	return New;
}