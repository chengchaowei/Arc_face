//#include "stdafx.h"
#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <direct.h> 
#include <iostream>  
#include <crtdbg.h>
#include <stdarg.h>
#include <opencv.hpp>
#include <io.h>   
#include <vector> 
#include<fstream>  
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include<string.h>
#include <sstream>
#include <exception>
#include <windows.h>
#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;
extern long long couting = 0;

#pragma comment(lib, "libarcsoft_face_engine.lib")

//#define APPID "3oHkvk7Wh9N3aneiXxyBzR71umuBYxUW2L69DQRuJtBF"
//#define SDKKey "7bscFFKX1vHhWYmWzdkPpkkj8x1YpwtoYKoMD9tvZnfG"	
#define APPID "Dtn3MMQyDytDWNnPzkEHbhbdbQsSSqgzeuP4YKf68cEg"
#define SDKKey "CVobQtULgo1m6m5Bwc98KuoKLiQx6B1vLpyCrC4kxeVT"

#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 

MHandle init() {
	//激活SDK
	MRESULT res = ASFActivation(APPID, SDKKey);
	if (MOK != res && MERR_ASF_ALREADY_ACTIVATED != res)
		printf("ALActivation fail: %d\n", res);
	else
		printf("ALActivation sucess: %d\n", res);

	//初始化引擎
	MHandle handle = NULL;
	MInt32 mask = ASF_FACE_DETECT | ASF_FACERECOGNITION | ASF_AGE | ASF_GENDER | ASF_FACE3DANGLE;
	res = ASFInitEngine(ASF_DETECT_MODE_IMAGE, ASF_OP_0_ONLY, 16, 5, mask, &handle);
	if (res != MOK)
		printf("ALInitEngine fail: %d\n", res);
	else
		printf("ALInitEngine sucess: %d\n", res);

	return handle;
}

int face_match(const char* image_path1, const char* image_path2, MHandle handle)
{
	int score = 0;
	MRESULT res;
	// 人脸检测
	IplImage* img = cvLoadImage(image_path1);

	IplImage* img1 = cvLoadImage(image_path2);

	if (img && img1)
	{
		ASF_MultiFaceInfo detectedFaces1 = { 0 };
		ASF_SingleFaceInfo SingleDetectedFaces1 = { 0 };
		ASF_FaceFeature feature1 = { 0 };
		ASF_FaceFeature copyfeature1 = { 0 };
		res = ASFDetectFaces(handle, img->width, img->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)img->imageData, &detectedFaces1);
		if (MOK == res)
		{
			SingleDetectedFaces1.faceRect.left = detectedFaces1.faceRect[0].left;
			SingleDetectedFaces1.faceRect.top = detectedFaces1.faceRect[0].top;
			SingleDetectedFaces1.faceRect.right = detectedFaces1.faceRect[0].right;
			SingleDetectedFaces1.faceRect.bottom = detectedFaces1.faceRect[0].bottom;
			SingleDetectedFaces1.faceOrient = detectedFaces1.faceOrient[0];

			res = ASFFaceFeatureExtract(handle, img->width, img->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)img->imageData, &SingleDetectedFaces1, &feature1);
			if (res == MOK)
			{
				//拷贝feature
				copyfeature1.featureSize = feature1.featureSize;
				copyfeature1.feature = (MByte *)malloc(feature1.featureSize);
				memset(copyfeature1.feature, 0, feature1.featureSize);
				memcpy(copyfeature1.feature, feature1.feature, feature1.featureSize);
			}
			else {
				printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
			}

		}
		else {
			printf("ASFDetectFaces 1 fail: %d\n", res);
		}
		//第二张人脸提取特征
		ASF_MultiFaceInfo	detectedFaces2 = { 0 };
		ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
		ASF_FaceFeature feature2 = { 0 };
		res = ASFDetectFaces(handle, img1->width, img1->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)img1->imageData, &detectedFaces2);
		if (MOK == res)
		{
			SingleDetectedFaces2.faceRect.left = detectedFaces2.faceRect[0].left;
			SingleDetectedFaces2.faceRect.top = detectedFaces2.faceRect[0].top;
			SingleDetectedFaces2.faceRect.right = detectedFaces2.faceRect[0].right;
			SingleDetectedFaces2.faceRect.bottom = detectedFaces2.faceRect[0].bottom;
			SingleDetectedFaces2.faceOrient = detectedFaces2.faceOrient[0];

			res = ASFFaceFeatureExtract(handle, img1->width, img1->height, ASVL_PAF_RGB24_B8G8R8, (MUInt8*)img1->imageData, &SingleDetectedFaces2, &feature2);
			if (MOK != res) {
				printf("ASFFaceFeatureExtract 2 fail: %d\n", res);
				SafeFree(copyfeature1.feature);
				cvReleaseImage(&img);
				cvReleaseImage(&img1);
				return -1;
			}
		}
		else
			printf("ASFDetectFaces 2 fail: %d\n", res);

		// 单人脸特征比对
		MFloat confidenceLevel;
		res = ASFFaceFeatureCompare(handle, &copyfeature1, &feature2, &confidenceLevel);
		if (res != MOK) {
			printf("ASFFaceFeatureCompare fail: %d\n", res);
		}
		else
			score = int(confidenceLevel*100.0);
		printf("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);
		SafeFree(copyfeature1.feature);		//释放内存
	}
	cvReleaseImage(&img);
	cvReleaseImage(&img1);
	return score;
}

void uinit(MHandle handle) {
	//获取版本信息
	MRESULT res;
	const ASF_VERSION* pVersionInfo = ASFGetVersion(handle);

	//反初始化
	res = ASFUninitEngine(handle);
	if (res != MOK)
		printf("ALUninitEngine fail: %d\n", res);
	else
		printf("ALUninitEngine sucess: %d\n", res);
}

int Resize(const char* filename_path) {
	const char* filename = filename_path;

	cv::Mat src, dst;

	int scaleW = 4;
	//定义图像的大小，宽度缩小80%
	int scaleH = scaleW;
	//定义图像的大小，高度缩小80%

	src = cv::imread(filename);
	if (src.empty()) {
		throw("Faild open file.");
	}

	int width = static_cast<int>((int)(src.cols / scaleW)*scaleW);
	//定义想要扩大或者缩小后的宽度，4的倍数
	int height = static_cast<int>((int)(src.rows / scaleH)*(scaleH));
	//定义想要扩大或者缩小后的高度，4的倍数

	resize(src, dst, cv::Size(width, height), 0, 0, INTER_CUBIC);
	//重新定义大小的函数
	imwrite(filename_path, dst);
	dst.release();
	src.release();
	return 0;
}

int copy_file_function(const char* path1_sorse, char* path2_sorse)
{
	const char* path = path1_sorse;
	char* path2 = path2_sorse;
	ifstream infile(path, ios::in | ios::binary);
	ofstream outfile(path2, ios::out | ios::binary);
	char szBuff[1025] = { 0 };
	while (!infile.eof())
	{
		infile.read(szBuff, sizeof(szBuff));
		outfile.write(szBuff, infile.gcount());
	}

	//int iFilesize = 0;
	//infile.seekg(0, ios::end);
	//iFilesize = infile.tellg();
	//infile.seekg(0);

	//cout << iFilesize << endl;

	infile.close();
	outfile.close();
	return 0;
}

void read_txt_and_face_match(MHandle handle) {
	string data_s;
	ifstream data_inf;
	string sample;
	data_inf.open("E://ArcFace_pro//data9.txt");
	char my_dir_eight[] = "0.8_abo//";	//通过strcat将文件夹信息组合成可寻址文件夹
	char my_dir_seven[] = "0.7_0.8_amo//";
	char my_dir_six[] = "0.6_0.7_amo//";
	char my_dir_five[] = "0.55_0.6_amo//";
	char last_dir[100] = "E://ArcFace_pro//last//";
	char* file_path_name = { 0 };
	const char* image_path2;
	while (getline(data_inf, data_s))//getline(inf,s)是逐行读取inf中的文件信息
	{
		couting++;
		cout << "couting:-------------------------------------------------" << couting << endl;
		image_path2 = data_s.data();
		Resize(image_path2);
		int lens_path2 = strlen(image_path2);
		int counts = 0;
		int flag = 1;
		char img_filename[41] = { 0 };//照片名字
		for (int i = 26; i < lens_path2; i++) {
			img_filename[counts] = image_path2[i];
			counts++;
		}//获取文件名字
		cout << img_filename << endl;
		int flags = 0;
		ifstream sample_test;
		sample_test.open("E://ArcFace_pro//sample.txt");
		while (getline(sample_test, sample)) {
			const char* image_path1 = sample.data();//打开样本文件
			cout << image_path1 << endl;
			int lens = strlen(image_path1);
			int count = 1;
			int score = 0;
			//制作文件夹路径
			char* image_dir_path_one = {};
			char* image_dir_path_two = {};
			char* image_dir_path_thre = {};
			char* image_dir_path_four = {};
			char image_sample_path[5][50] = { 0 };
			for (int j = 0; j < 4; j++) {
				for (int i = 0; i < lens - 5; i++) {
					image_sample_path[j][i] = image_path1[i];
				}
				if (count == 1) {
					image_dir_path_one = strcat(image_sample_path[j], my_dir_eight);//获取sample文件夹路径				
				}
				if (count == 2) {
					image_dir_path_two = strcat(image_sample_path[j], my_dir_seven);
				}
				if (count == 3) {
					image_dir_path_thre = strcat(image_sample_path[j], my_dir_six);
				}
				if (count == 4) {
					image_dir_path_four = strcat(image_sample_path[j], my_dir_five);
				}
				count++;
			}
			score = face_match(image_path1, image_path2, handle);
			cout << "score:" << score << endl;
			if (score == -1) {
				break;
			}
			if (score >= 80) {
				if (flag == 1) {
					file_path_name = strcat(image_dir_path_one, img_filename);
					flag = 0;
				}
				copy_file_function(image_path2, file_path_name);
				cout << file_path_name << " 移入80-100文件夹" << endl;
				int lens = strlen(file_path_name);
				for (int i = 38; i < lens; i++) {
					file_path_name[i] = { 0 };
				}
				break;
				//利用复制函数将文件复制到指定的文件夹
			}
			else if (score >= 70 && score<80) {
				if (flag == 1) {
					file_path_name = strcat(image_dir_path_two, img_filename);
					flag = 0;
				}
				copy_file_function(image_path2, file_path_name);
				cout << file_path_name << " 移入70-80文件夹" << endl;
				int lens = strlen(file_path_name);
				for (int i = 42; i < lens; i++) {
					file_path_name[i] = { 0 };
				}
				break;
				//利用复制函数将文件复制到指定的文件夹
			}
			else if (score >= 60 && score<70) {
				if (flag == 1) {
					file_path_name = strcat(image_dir_path_thre, img_filename);
					flag = 0;
				}
				copy_file_function(image_path2, file_path_name);
				cout << file_path_name << " 移入60-70文件夹" << endl;
				int lens = strlen(file_path_name);
				for (int i = 42; i < lens; i++) {
					file_path_name[i] = { 0 };
				}
				break;
				//利用复制函数将文件复制到指定的文件夹
			}
			else if (score >= 55 && score<60) {
				if (flag == 1) {
					file_path_name = strcat(image_dir_path_four, img_filename);
					flag = 0;
				}
				copy_file_function(image_path2, file_path_name);
				cout << file_path_name << " 移入55-60文件夹" << endl;
				int lens = strlen(file_path_name);
				for (int i = 43; i < lens; i++) {
					file_path_name[i] = { 0 };
				}
				break;
				//利用复制函数将文件复制到指定的文件夹
			}
			flags++;
			while (flags == 67) {
				if (flag == 1) {
					//cout << "融合路径" << endl;
					file_path_name = strcat(last_dir, img_filename);
					flag = 0;
					//cout << "融合完成"<< file_path_name << endl;
				}
				copy_file_function(image_path2, file_path_name);
				cout << file_path_name << " 移入last文件夹" << endl;
				int lens = strlen(file_path_name);
				for (int i = 23; i < lens; i++) {
					file_path_name[i] = { 0 };
				}
				//cout << "file_path_name" << file_path_name << endl;
				flags = 0;
			}
		}
		sample_test.close();
	}
	data_inf.close();
}


int main() {
	MHandle handle = init();
	read_txt_and_face_match(handle);
	uinit(handle);
	system("pause");
	return 0;
}
