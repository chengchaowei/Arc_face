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
#pragma comment(lib, "libarcsoft_face_engine.lib")

#define APPID "Dtn3MMQyDytDWNnPzkEHbhbdbQsSSqgzeuP4YKf68cEg"
#define SDKKey "CVobQtULgo1m6m5Bwc98KuoKLiQx6B1vLpyCrC4kxeVT"	

#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 
extern int flags = 83;
extern ASF_FaceFeature copyfeature1[83] = { 0 };
extern int count = 0;
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

void face_match(MHandle handle)
{
	long long num_image = 0;
	MRESULT res;
	// 人脸检测
	ifstream sample_test;
	string sample;
	int feature_count = 0;
	sample_test.open("E://image_processing//sample.txt");
	while (getline(sample_test, sample) && flags)
	{
		const char* image_path1 = sample.data();
		IplImage* img = cvLoadImage(image_path1);
		ASF_MultiFaceInfo detectedFaces1 = { 0 };
		ASF_SingleFaceInfo SingleDetectedFaces1 = { 0 };
		ASF_FaceFeature feature1 = { 0 };
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
				copyfeature1[feature_count].featureSize = feature1.featureSize;
				copyfeature1[feature_count].feature = (MByte *)malloc(feature1.featureSize);
				memset(copyfeature1[feature_count].feature, 0, feature1.featureSize);
				memcpy(copyfeature1[feature_count].feature, feature1.feature, feature1.featureSize);
			}
			else {
				printf("ASFFaceFeatureExtract 1 fail: %d\n", res);
			}
		}
		else {
			printf("ASFDetectFaces 1 fail: %d\n", res);
		}
		cout << "第 " << feature_count << " 张图片特征拷贝成功" << endl;
		feature_count++;
		flags--;
		cvReleaseImage(&img);
	}
	string data_s;
	ifstream data_inf;
	data_inf.open("E://image_processing//data1.txt");
	const char* image_path2;
	while (getline(data_inf, data_s)) {
		num_image++;
		cout << "第 " << num_image << " 张图片" << endl;
		image_path2 = data_s.data();
		int lens_path2 = strlen(image_path2);
		int counts = 0;
		int flag = 1;
		char* file_path_name = { 0 };
		char img_filename[40] = { 0 };//照片名字
		for (int i = 34; i < lens_path2; i++) {
			img_filename[counts] = image_path2[i];
			counts++;
		}//获取文件名字
		try
		{
			// 保护代码
			Resize(image_path2);
		}
		catch (exception)
		{
			// catch 块
			cout << "出现异常！" << endl;
			continue;
		}
		IplImage* img1 = cvLoadImage(image_path2);
		ASF_FaceFeature feature2 = { 0 };
		if (img1)
		{
			//第二张人脸提取特征
			ASF_MultiFaceInfo	detectedFaces2 = { 0 };
			ASF_SingleFaceInfo SingleDetectedFaces2 = { 0 };
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
					cvReleaseImage(&img1);
					continue;
				}
			}
			else
				printf("ASFDetectFaces 2 fail: %d\n", res);
		}
		// 单人脸特征比对
		MFloat confidenceLevel;
		int flags_num = 1;
		for (int i = 0; i < 83; i++) {
			try {
				res = ASFFaceFeatureCompare(handle, &copyfeature1[i], &feature2, &confidenceLevel);
			}
			catch (exception) {
				cout << "特征对比失败！" << endl;
				continue;
			}
			if (res != MOK) {
				printf("ASFFaceFeatureCompare fail: %d\n", res);
				continue;
			}
			else;
			//printf("ASFFaceFeatureCompare sucess: %lf\n", confidenceLevel);
			if (confidenceLevel >= 0.55 && confidenceLevel < 0.6) {
				flags_num = 0;
				switch (i)
				{
				case 0:
				{char path_name0[100] = "E://image_processing//sample//00//0.55_0.6_amo//";
				file_path_name = strcat(path_name0, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//00//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 1:
				{char path_name1[100] = "E://image_processing//sample//01//0.55_0.6_amo//";
				file_path_name = strcat(path_name1, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//01//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 2:    
				{char path_name2[100] = "E://image_processing//sample//02//0.55_0.6_amo//";
				file_path_name = strcat(path_name2, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E:/     /image_processing//sample//02//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 3:
				{char path_name3[100] = "E://image_processing//sample//03//0.55_0.6_amo//";
				file_path_name = strcat(path_name3, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//03//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 4:
				{char path_name4[100] = "E://image_processing//sample//04//0.55_0.6_amo//";
				file_path_name = strcat(path_name4, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//04//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 5:
				{char path_name5[100] = "E://image_processing//sample//05//0.55_0.6_amo//";
				file_path_name = strcat(path_name5, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//05//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 6:
				{char path_name6[100] = "E://image_processing//sample//06//0.55_0.6_amo//";
				file_path_name = strcat(path_name6, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//06//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 7:
				{char path_name7[100] = "E://image_processing//sample//07//0.55_0.6_amo//";
				file_path_name = strcat(path_name7, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//07//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 8:
				{char path_name8[100] = "E://image_processing//sample//08//0.55_0.6_amo//";
				file_path_name = strcat(path_name8, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//08//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 9:
				{char path_name9[100] = "E://image_processing//sample//09//0.55_0.6_amo//";
				file_path_name = strcat(path_name9, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//09//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 10:
				{char path_name10[100] = "E://image_processing//sample//10//0.55_0.6_amo//";
				file_path_name = strcat(path_name10, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//10//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 11:
				{char path_name11[100] = "E://image_processing//sample//11//0.55_0.6_amo//";
				file_path_name = strcat(path_name11, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//11//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 12:
				{char path_name12[100] = "E://image_processing//sample//12//0.55_0.6_amo//";
				file_path_name = strcat(path_name12, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//12//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 13:
				{char path_name13[100] = "E://image_processing//sample//13//0.55_0.6_amo//";
				file_path_name = strcat(path_name13, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//13//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 14:
				{char path_name14[100] = "E://image_processing//sample//14//0.55_0.6_amo//";
				file_path_name = strcat(path_name14, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//14//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 15:
				{char path_name15[100] = "E://image_processing//sample//15//0.55_0.6_amo//";
				file_path_name = strcat(path_name15, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//15//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 16:
				{char path_name16[100] = "E://image_processing//sample//16//0.55_0.6_amo//";
				file_path_name = strcat(path_name16, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//16//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 17:
				{char path_name17[100] = "E://image_processing//sample//17//0.55_0.6_amo//";
				file_path_name = strcat(path_name17, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//17//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 18:
				{char path_name18[100] = "E://image_processing//sample//18//0.55_0.6_amo//";
				file_path_name = strcat(path_name18, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//18//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 19:
				{char path_name19[100] = "E://image_processing//sample//19//0.55_0.6_amo//";
				file_path_name = strcat(path_name19, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//19//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 20:
				{char path_name20[100] = "E://image_processing//sample//20//0.55_0.6_amo//";
				file_path_name = strcat(path_name20, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//20//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 21:
				{char path_name21[100] = "E://image_processing//sample//21//0.55_0.6_amo//";
				file_path_name = strcat(path_name21, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//21//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 22:
				{char path_name22[100] = "E://image_processing//sample//22//0.55_0.6_amo//";
				file_path_name = strcat(path_name22, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//22//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 23:
				{char path_name23[100] = "E://image_processing//sample//23//0.55_0.6_amo//";
				file_path_name = strcat(path_name23, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//23//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 24:
				{char path_name24[100] = "E://image_processing//sample//24//0.55_0.6_amo//";
				file_path_name = strcat(path_name24, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//24//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 25:
				{char path_name25[100] = "E://image_processing//sample//25//0.55_0.6_amo//";
				file_path_name = strcat(path_name25, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//25//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 26:
				{char path_name26[100] = "E://image_processing//sample//26//0.55_0.6_amo//";
				file_path_name = strcat(path_name26, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//26//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 27:
				{char path_name27[100] = "E://image_processing//sample//27//0.55_0.6_amo//";
				file_path_name = strcat(path_name27, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//27//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 28:
				{char path_name28[100] = "E://image_processing//sample//28//0.55_0.6_amo//";
				file_path_name = strcat(path_name28, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//28//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 29:
				{char path_name29[100] = "E://image_processing//sample//29//0.55_0.6_amo//";
				file_path_name = strcat(path_name29, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//29//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 30:
				{char path_name30[100] = "E://image_processing//sample//30//0.55_0.6_amo//";
				file_path_name = strcat(path_name30, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//30//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 31:
				{char path_name31[100] = "E://image_processing//sample//31//0.55_0.6_amo//";
				file_path_name = strcat(path_name31, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//31//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 32:
				{char path_name32[100] = "E://image_processing//sample//32//0.55_0.6_amo//";
				file_path_name = strcat(path_name32, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//32//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 33:
				{char path_name33[100] = "E://image_processing//sample//33//0.55_0.6_amo//";
				file_path_name = strcat(path_name33, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//33//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 34:
				{char path_name34[100] = "E://image_processing//sample//34//0.55_0.6_amo//";
				file_path_name = strcat(path_name34, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//34//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 35:
				{char path_name35[100] = "E://image_processing//sample//35//0.55_0.6_amo//";
				file_path_name = strcat(path_name35, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//35//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 36:
				{char path_name36[100] = "E://image_processing//sample//36//0.55_0.6_amo//";
				file_path_name = strcat(path_name36, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//36//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 37:
				{char path_name37[100] = "E://image_processing//sample//37//0.55_0.6_amo//";
				file_path_name = strcat(path_name37, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//37//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 38:
				{char path_name38[100] = "E://image_processing//sample//38//0.55_0.6_amo//";
				file_path_name = strcat(path_name38, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//38//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 39:
				{char path_name39[100] = "E://image_processing//sample//39//0.55_0.6_amo//";
				file_path_name = strcat(path_name39, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//39//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 40:
				{char path_name40[100] = "E://image_processing//sample//40//0.55_0.6_amo//";
				file_path_name = strcat(path_name40, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//40//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 41:
				{char path_name41[100] = "E://image_processing//sample//41//0.55_0.6_amo//";
				file_path_name = strcat(path_name41, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//41//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 42:
				{char path_name42[100] = "E://image_processing//sample//42//0.55_0.6_amo//";
				file_path_name = strcat(path_name42, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//42//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 43:
				{char path_name43[100] = "E://image_processing//sample//43//0.55_0.6_amo//";
				file_path_name = strcat(path_name43, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//43//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 44:
				{char path_name44[100] = "E://image_processing//sample//44//0.55_0.6_amo//";
				file_path_name = strcat(path_name44, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//44//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 45:
				{char path_name45[100] = "E://image_processing//sample//45//0.55_0.6_amo//";
				file_path_name = strcat(path_name45, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//45//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 46:
				{char path_name46[100] = "E://image_processing//sample//46//0.55_0.6_amo//";
				file_path_name = strcat(path_name46, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//46//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 47:
				{char path_name47[100] = "E://image_processing//sample//47//0.55_0.6_amo//";
				file_path_name = strcat(path_name47, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//47//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 48:
				{char path_name48[100] = "E://image_processing//sample//48//0.55_0.6_amo//";
				file_path_name = strcat(path_name48, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//48//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 49:
				{char path_name49[100] = "E://image_processing//sample//49//0.55_0.6_amo//";
				file_path_name = strcat(path_name49, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//49//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 50:
				{char path_name50[100] = "E://image_processing//sample//50//0.55_0.6_amo//";
				file_path_name = strcat(path_name50, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//50//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 51:
				{char path_name51[100] = "E://image_processing//sample//51//0.55_0.6_amo//";
				file_path_name = strcat(path_name51, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//51//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 52:
				{char path_name52[100] = "E://image_processing//sample//52//0.55_0.6_amo//";
				file_path_name = strcat(path_name52, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//52//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 53:
				{char path_name53[100] = "E://image_processing//sample//53//0.55_0.6_amo//";
				file_path_name = strcat(path_name53, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//53//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 54:
				{char path_name54[100] = "E://image_processing//sample//54//0.55_0.6_amo//";
				file_path_name = strcat(path_name54, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//54//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 55:
				{char path_name55[100] = "E://image_processing//sample//55//0.55_0.6_amo//";
				file_path_name = strcat(path_name55, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//55//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 56:
				{char path_name56[100] = "E://image_processing//sample//56//0.55_0.6_amo//";
				file_path_name = strcat(path_name56, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//56//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 57:
				{char path_name57[100] = "E://image_processing//sample//57//0.55_0.6_amo//";
				file_path_name = strcat(path_name57, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//57//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 58:
				{char path_name58[100] = "E://image_processing//sample//58//0.55_0.6_amo//";
				file_path_name = strcat(path_name58, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//58//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 59:
				{char path_name59[100] = "E://image_processing//sample//59//0.55_0.6_amo//";
				file_path_name = strcat(path_name59, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//59//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 60:
				{char path_name60[100] = "E://image_processing//sample//60//0.55_0.6_amo//";
				file_path_name = strcat(path_name60, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//60//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 61:
				{char path_name61[100] = "E://image_processing//sample//61//0.55_0.6_amo//";
				file_path_name = strcat(path_name61, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 62:
				{char path_name62[100] = "E://image_processing//sample//62//0.55_0.6_amo//";
				file_path_name = strcat(path_name62, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 63:
				{char path_name63[100] = "E://image_processing//sample//63//0.55_0.6_amo//";
				file_path_name = strcat(path_name63, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//63//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 64:
				{char path_name64[100] = "E://image_processing//sample//64//0.55_0.6_amo//";
				file_path_name = strcat(path_name64, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//64//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 65:
				{char path_name65[100] = "E://image_processing//sample//65//0.55_0.6_amo//";
				file_path_name = strcat(path_name65, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//65//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 66:
				{char path_name66[100] = "E://image_processing//sample//66//0.55_0.6_amo//";
				file_path_name = strcat(path_name66, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//66//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 67:
				{char path_name67[100] = "E://image_processing//sample//67//0.55_0.6_amo//";
				file_path_name = strcat(path_name67, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//67//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 68:
				{char path_name68[100] = "E://image_processing//sample//68//0.55_0.6_amo//";
				file_path_name = strcat(path_name68, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//68//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 69:
				{char path_name69[100] = "E://image_processing//sample//69//0.55_0.6_amo//";
				file_path_name = strcat(path_name69, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//69//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 70:
				{char path_name70[100] = "E://image_processing//sample//70//0.55_0.6_amo//";
				file_path_name = strcat(path_name70, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//70//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 71:
				{char path_name71[100] = "E://image_processing//sample//71//0.55_0.6_amo//";
				file_path_name = strcat(path_name71, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//71//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 72:
				{char path_name72[100] = "E://image_processing//sample//72//0.55_0.6_amo//";
				file_path_name = strcat(path_name72, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//72//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 73:
				{char path_name73[100] = "E://image_processing//sample//73//0.55_0.6_amo//";
				file_path_name = strcat(path_name73, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//73//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 74:
				{char path_name74[100] = "E://image_processing//sample//74//0.55_0.6_amo//";
				file_path_name = strcat(path_name74, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//74//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 75:
				{char path_name75[100] = "E://image_processing//sample//75//0.55_0.6_amo//";
				file_path_name = strcat(path_name75, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//75//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 76:
				{char path_name76[100] = "E://image_processing//sample//76//0.55_0.6_amo//";
				file_path_name = strcat(path_name76, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//76//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 77:
				{char path_name77[100] = "E://image_processing//sample//77//0.55_0.6_amo//";
				file_path_name = strcat(path_name77, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//77//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 78:
				{char path_name78[100] = "E://image_processing//sample//78//0.55_0.6_amo//";
				file_path_name = strcat(path_name78, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//78//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 79:
				{char path_name79[100] = "E://image_processing//sample//79//0.55_0.6_amo//";
				file_path_name = strcat(path_name79, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//79//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 80:
				{char path_name80[100] = "E://image_processing//sample//80//0.55_0.6_amo//";
				file_path_name = strcat(path_name80, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//80//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 81:
				{char path_name81[100] = "E://image_processing//sample//81//0.55_0.6_amo//";
				file_path_name = strcat(path_name81, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//81//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				case 82:
				{char path_name82[100] = "E://image_processing//sample//82//0.55_0.6_amo//";
				file_path_name = strcat(path_name82, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//82//0.55_0.6_amo//---文件夹！" << endl; }
				break;
				}
				break;
			}
			if (confidenceLevel >= 0.6 && confidenceLevel < 0.7) {
				flags_num = 0;
				switch (i)
				{
				case 0:
				{char path_name0[100] = "E://image_processing//sample//00//0.6_0.7_amo//";
				file_path_name = strcat(path_name0, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//00//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 1:
				{char path_name1[100] = "E://image_processing//sample//01//0.6_0.7_amo//";
				file_path_name = strcat(path_name1, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//01//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 2:
				{char path_name2[100] = "E://image_processing//sample//02//0.6_0.7_amo//";
				file_path_name = strcat(path_name2, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//02//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 3:
				{char path_name3[100] = "E://image_processing//sample//03//0.6_0.7_amo//";
				file_path_name = strcat(path_name3, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//03//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 4:
				{char path_name4[100] = "E://image_processing//sample//04//0.6_0.7_amo//";
				file_path_name = strcat(path_name4, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//04//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 5:
				{char path_name5[100] = "E://image_processing//sample//05//0.6_0.7_amo//";
				file_path_name = strcat(path_name5, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//05//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 6:
				{char path_name6[100] = "E://image_processing//sample//06//0.6_0.7_amo//";
				file_path_name = strcat(path_name6, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//06//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 7:
				{char path_name7[100] = "E://image_processing//sample//07//0.6_0.7_amo//";
				file_path_name = strcat(path_name7, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//07//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 8:
				{char path_name8[100] = "E://image_processing//sample//08//0.6_0.7_amo//";
				file_path_name = strcat(path_name8, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//08//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 9:
				{char path_name9[100] = "E://image_processing//sample//09//0.6_0.7_amo//";
				file_path_name = strcat(path_name9, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//09//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 10:
				{char path_name10[100] = "E://image_processing//sample//10//0.6_0.7_amo//";
				file_path_name = strcat(path_name10, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//10//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 11:
				{char path_name11[100] = "E://image_processing//sample//11//0.6_0.7_amo//";
				file_path_name = strcat(path_name11, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//11//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 12:
				{char path_name12[100] = "E://image_processing//sample//12//0.6_0.7_amo//";
				file_path_name = strcat(path_name12, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//12//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 13:
				{char path_name13[100] = "E://image_processing//sample//13//0.6_0.7_amo//";
				file_path_name = strcat(path_name13, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//13//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 14:
				{char path_name14[100] = "E://image_processing//sample//14//0.6_0.7_amo//";
				file_path_name = strcat(path_name14, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//14//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 15:
				{char path_name15[100] = "E://image_processing//sample//15//0.6_0.7_amo//";
				file_path_name = strcat(path_name15, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//15//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 16:
				{char path_name16[100] = "E://image_processing//sample//16//0.6_0.7_amo//";
				file_path_name = strcat(path_name16, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//16//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 17:
				{char path_name17[100] = "E://image_processing//sample//17//0.6_0.7_amo//";
				file_path_name = strcat(path_name17, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//17//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 18:
				{char path_name18[100] = "E://image_processing//sample//18//0.6_0.7_amo//";
				file_path_name = strcat(path_name18, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//18//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 19:
				{char path_name19[100] = "E://image_processing//sample//19//0.6_0.7_amo//";
				file_path_name = strcat(path_name19, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//19//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 20:
				{char path_name20[100] = "E://image_processing//sample//20//0.6_0.7_amo//";
				file_path_name = strcat(path_name20, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//20//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 21:
				{char path_name21[100] = "E://image_processing//sample//21//0.6_0.7_amo//";
				file_path_name = strcat(path_name21, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//21//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 22:
				{char path_name22[100] = "E://image_processing//sample//22//0.6_0.7_amo//";
				file_path_name = strcat(path_name22, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//22//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 23:
				{char path_name23[100] = "E://image_processing//sample//23//0.6_0.7_amo//";
				file_path_name = strcat(path_name23, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//23//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 24:
				{char path_name24[100] = "E://image_processing//sample//24//0.6_0.7_amo//";
				file_path_name = strcat(path_name24, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//24//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 25:
				{char path_name25[100] = "E://image_processing//sample//25//0.6_0.7_amo//";
				file_path_name = strcat(path_name25, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//25//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 26:
				{char path_name26[100] = "E://image_processing//sample//26//0.6_0.7_amo//";
				file_path_name = strcat(path_name26, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//26//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 27:
				{char path_name27[100] = "E://image_processing//sample//27//0.6_0.7_amo//";
				file_path_name = strcat(path_name27, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//27//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 28:
				{char path_name28[100] = "E://image_processing//sample//28//0.6_0.7_amo//";
				file_path_name = strcat(path_name28, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//28//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 29:
				{char path_name29[100] = "E://image_processing//sample//29//0.6_0.7_amo//";
				file_path_name = strcat(path_name29, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//29//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 30:
				{char path_name30[100] = "E://image_processing//sample//30//0.6_0.7_amo//";
				file_path_name = strcat(path_name30, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//30//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 31:
				{char path_name31[100] = "E://image_processing//sample//31//0.6_0.7_amo//";
				file_path_name = strcat(path_name31, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//31//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 32:
				{char path_name32[100] = "E://image_processing//sample//32//0.6_0.7_amo//";
				file_path_name = strcat(path_name32, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//32//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 33:
				{char path_name33[100] = "E://image_processing//sample//33//0.6_0.7_amo//";
				file_path_name = strcat(path_name33, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//33//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 34:
				{char path_name34[100] = "E://image_processing//sample//34//0.6_0.7_amo//";
				file_path_name = strcat(path_name34, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//34//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 35:
				{char path_name35[100] = "E://image_processing//sample//35//0.6_0.7_amo//";
				file_path_name = strcat(path_name35, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//35//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 36:
				{char path_name36[100] = "E://image_processing//sample//36//0.6_0.7_amo//";
				file_path_name = strcat(path_name36, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//36//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 37:
				{char path_name37[100] = "E://image_processing//sample//37//0.6_0.7_amo//";
				file_path_name = strcat(path_name37, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//37//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 38:
				{char path_name38[100] = "E://image_processing//sample//38//0.6_0.7_amo//";
				file_path_name = strcat(path_name38, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//38//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 39:
				{char path_name39[100] = "E://image_processing//sample//39//0.6_0.7_amo//";
				file_path_name = strcat(path_name39, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//39//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 40:
				{char path_name40[100] = "E://image_processing//sample//40//0.6_0.7_amo//";
				file_path_name = strcat(path_name40, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//40//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 41:
				{char path_name41[100] = "E://image_processing//sample//41//0.6_0.7_amo//";
				file_path_name = strcat(path_name41, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//41//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 42:
				{char path_name42[100] = "E://image_processing//sample//42//0.6_0.7_amo//";
				file_path_name = strcat(path_name42, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//42//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 43:
				{char path_name43[100] = "E://image_processing//sample//43//0.6_0.7_amo//";
				file_path_name = strcat(path_name43, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//43//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 44:
				{char path_name44[100] = "E://image_processing//sample//44//0.6_0.7_amo//";
				file_path_name = strcat(path_name44, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//44//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 45:
				{char path_name45[100] = "E://image_processing//sample//45//0.6_0.7_amo//";
				file_path_name = strcat(path_name45, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//45//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 46:
				{char path_name46[100] = "E://image_processing//sample//46//0.6_0.7_amo//";
				file_path_name = strcat(path_name46, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//46//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 47:
				{char path_name47[100] = "E://image_processing//sample//47//0.6_0.7_amo//";
				file_path_name = strcat(path_name47, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//47//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 48:
				{char path_name48[100] = "E://image_processing//sample//48//0.6_0.7_amo//";
				file_path_name = strcat(path_name48, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//48//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 49:
				{char path_name49[100] = "E://image_processing//sample//49//0.6_0.7_amo//";
				file_path_name = strcat(path_name49, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//49//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 50:
				{char path_name50[100] = "E://image_processing//sample//50//0.6_0.7_amo//";
				file_path_name = strcat(path_name50, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//50//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 51:
				{char path_name51[100] = "E://image_processing//sample//51//0.6_0.7_amo//";
				file_path_name = strcat(path_name51, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//51//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 52:
				{char path_name52[100] = "E://image_processing//sample//52//0.6_0.7_amo//";
				file_path_name = strcat(path_name52, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//52//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 53:
				{char path_name53[100] = "E://image_processing//sample//53//0.6_0.7_amo//";
				file_path_name = strcat(path_name53, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//53//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 54:
				{char path_name54[100] = "E://image_processing//sample//54//0.6_0.7_amo//";
				file_path_name = strcat(path_name54, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//54//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 55:
				{char path_name55[100] = "E://image_processing//sample//55//0.6_0.7_amo//";
				file_path_name = strcat(path_name55, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//55//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 56:
				{char path_name56[100] = "E://image_processing//sample//56//0.6_0.7_amo//";
				file_path_name = strcat(path_name56, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//56//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 57:
				{char path_name57[100] = "E://image_processing//sample//57//0.6_0.7_amo//";
				file_path_name = strcat(path_name57, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//57//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 58:
				{char path_name58[100] = "E://image_processing//sample//58//0.6_0.7_amo//";
				file_path_name = strcat(path_name58, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//58//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 59:
				{char path_name59[100] = "E://image_processing//sample//59//0.6_0.7_amo//";
				file_path_name = strcat(path_name59, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//59//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 60:
				{char path_name60[100] = "E://image_processing//sample//60//0.6_0.7_amo//";
				file_path_name = strcat(path_name60, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//60//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 61:
				{char path_name61[100] = "E://image_processing//sample//61//0.6_0.7_amo//";
				file_path_name = strcat(path_name61, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 62:
				{char path_name62[100] = "E://image_processing//sample//62//0.6_0.7_amo//";
				file_path_name = strcat(path_name62, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 63:
				{char path_name63[100] = "E://image_processing//sample//63//0.6_0.7_amo//";
				file_path_name = strcat(path_name63, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//63//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 64:
				{char path_name64[100] = "E://image_processing//sample//64//0.6_0.7_amo//";
				file_path_name = strcat(path_name64, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//64//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 65:
				{char path_name65[100] = "E://image_processing//sample//65//0.6_0.7_amo//";
				file_path_name = strcat(path_name65, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//65//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 66:
				{char path_name66[100] = "E://image_processing//sample//66//0.6_0.7_amo//";
				file_path_name = strcat(path_name66, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//66//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 67:
				{char path_name67[100] = "E://image_processing//sample//67//0.6_0.7_amo//";
				file_path_name = strcat(path_name67, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//67//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 68:
				{char path_name68[100] = "E://image_processing//sample//68//0.6_0.7_amo//";
				file_path_name = strcat(path_name68, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//68//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 69:
				{char path_name69[100] = "E://image_processing//sample//69//0.6_0.7_amo//";
				file_path_name = strcat(path_name69, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//69//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 70:
				{char path_name70[100] = "E://image_processing//sample//70//0.6_0.7_amo//";
				file_path_name = strcat(path_name70, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//70//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 71:
				{char path_name71[100] = "E://image_processing//sample//71//0.6_0.7_amo//";
				file_path_name = strcat(path_name71, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//71//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 72:
				{char path_name72[100] = "E://image_processing//sample//72//0.6_0.7_amo//";
				file_path_name = strcat(path_name72, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//72//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 73:
				{char path_name73[100] = "E://image_processing//sample//73//0.6_0.7_amo//";
				file_path_name = strcat(path_name73, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//73//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 74:
				{char path_name74[100] = "E://image_processing//sample//74//0.6_0.7_amo//";
				file_path_name = strcat(path_name74, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//74//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 75:
				{char path_name75[100] = "E://image_processing//sample//75//0.6_0.7_amo//";
				file_path_name = strcat(path_name75, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//75//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 76:
				{char path_name76[100] = "E://image_processing//sample//76//0.6_0.7_amo//";
				file_path_name = strcat(path_name76, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//76//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 77:
				{char path_name77[100] = "E://image_processing//sample//77//0.6_0.7_amo//";
				file_path_name = strcat(path_name77, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//77//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 78:
				{char path_name78[100] = "E://image_processing//sample//78//0.6_0.7_amo//";
				file_path_name = strcat(path_name78, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//78//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 79:
				{char path_name79[100] = "E://image_processing//sample//79//0.6_0.7_amo//";
				file_path_name = strcat(path_name79, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//79//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 80:
				{char path_name80[100] = "E://image_processing//sample//80//0.6_0.7_amo//";
				file_path_name = strcat(path_name80, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//80//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 81:
				{char path_name81[100] = "E://image_processing//sample//81//0.6_0.7_amo//";
				file_path_name = strcat(path_name81, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//81//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				case 82:
				{char path_name82[100] = "E://image_processing//sample//82//0.6_0.7_amo//";
				file_path_name = strcat(path_name82, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//82//0.6_0.7_amo//---文件夹！" << endl; }
				break;
				}
				break;
			}
			if (confidenceLevel >= 0.7 && confidenceLevel < 0.8) {
				flags_num = 0;
				switch (i)
				{
				case 0:
				{char path_name0[100] = "E://image_processing//sample//00//0.7_0.8_amo//";
				file_path_name = strcat(path_name0, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//00//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 1:
				{char path_name1[100] = "E://image_processing//sample//01//0.7_0.8_amo//";
				file_path_name = strcat(path_name1, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//01//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 2:
				{char path_name2[100] = "E://image_processing//sample//02//0.7_0.8_amo//";
				file_path_name = strcat(path_name2, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//02//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 3:
				{char path_name3[100] = "E://image_processing//sample//03//0.7_0.8_amo//";
				file_path_name = strcat(path_name3, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//03//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 4:
				{char path_name4[100] = "E://image_processing//sample//04//0.7_0.8_amo//";
				file_path_name = strcat(path_name4, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//04//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 5:
				{char path_name5[100] = "E://image_processing//sample//05//0.7_0.8_amo//";
				file_path_name = strcat(path_name5, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//05//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 6:
				{char path_name6[100] = "E://image_processing//sample//06//0.7_0.8_amo//";
				file_path_name = strcat(path_name6, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//06//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 7:
				{char path_name7[100] = "E://image_processing//sample//07//0.7_0.8_amo//";
				file_path_name = strcat(path_name7, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//07//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 8:
				{char path_name8[100] = "E://image_processing//sample//08//0.7_0.8_amo//";
				file_path_name = strcat(path_name8, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//08//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 9:
				{char path_name9[100] = "E://image_processing//sample//09//0.7_0.8_amo//";
				file_path_name = strcat(path_name9, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//09//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 10:
				{char path_name10[100] = "E://image_processing//sample//10//0.7_0.8_amo//";
				file_path_name = strcat(path_name10, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//10//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 11:
				{char path_name11[100] = "E://image_processing//sample//11//0.7_0.8_amo//";
				file_path_name = strcat(path_name11, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//11//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 12:
				{char path_name12[100] = "E://image_processing//sample//12//0.7_0.8_amo//";
				file_path_name = strcat(path_name12, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//12//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 13:
				{char path_name13[100] = "E://image_processing//sample//13//0.7_0.8_amo//";
				file_path_name = strcat(path_name13, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//13//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 14:
				{char path_name14[100] = "E://image_processing//sample//14//0.7_0.8_amo//";
				file_path_name = strcat(path_name14, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//14//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 15:
				{char path_name15[100] = "E://image_processing//sample//15//0.7_0.8_amo//";
				file_path_name = strcat(path_name15, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//15//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 16:
				{char path_name16[100] = "E://image_processing//sample//16//0.7_0.8_amo//";
				file_path_name = strcat(path_name16, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//16//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 17:
				{char path_name17[100] = "E://image_processing//sample//17//0.7_0.8_amo//";
				file_path_name = strcat(path_name17, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//17//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 18:
				{char path_name18[100] = "E://image_processing//sample//18//0.7_0.8_amo//";
				file_path_name = strcat(path_name18, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//18//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 19:
				{char path_name19[100] = "E://image_processing//sample//19//0.7_0.8_amo//";
				file_path_name = strcat(path_name19, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//19//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 20:
				{char path_name20[100] = "E://image_processing//sample//20//0.7_0.8_amo//";
				file_path_name = strcat(path_name20, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//20//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 21:
				{char path_name21[100] = "E://image_processing//sample//21//0.7_0.8_amo//";
				file_path_name = strcat(path_name21, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//21//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 22:
				{char path_name22[100] = "E://image_processing//sample//22//0.7_0.8_amo//";
				file_path_name = strcat(path_name22, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//22//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 23:
				{char path_name23[100] = "E://image_processing//sample//23//0.7_0.8_amo//";
				file_path_name = strcat(path_name23, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//23//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 24:
				{char path_name24[100] = "E://image_processing//sample//24//0.7_0.8_amo//";
				file_path_name = strcat(path_name24, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//24//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 25:
				{char path_name25[100] = "E://image_processing//sample//25//0.7_0.8_amo//";
				file_path_name = strcat(path_name25, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//25//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 26:
				{char path_name26[100] = "E://image_processing//sample//26//0.7_0.8_amo//";
				file_path_name = strcat(path_name26, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//26//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 27:
				{char path_name27[100] = "E://image_processing//sample//27//0.7_0.8_amo//";
				file_path_name = strcat(path_name27, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//27//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 28:
				{char path_name28[100] = "E://image_processing//sample//28//0.7_0.8_amo//";
				file_path_name = strcat(path_name28, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//28//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 29:
				{char path_name29[100] = "E://image_processing//sample//29//0.7_0.8_amo//";
				file_path_name = strcat(path_name29, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//29//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 30:
				{char path_name30[100] = "E://image_processing//sample//30//0.7_0.8_amo//";
				file_path_name = strcat(path_name30, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//30//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 31:
				{char path_name31[100] = "E://image_processing//sample//31//0.7_0.8_amo//";
				file_path_name = strcat(path_name31, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//31//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 32:
				{char path_name32[100] = "E://image_processing//sample//32//0.7_0.8_amo//";
				file_path_name = strcat(path_name32, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//32//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 33:
				{char path_name33[100] = "E://image_processing//sample//33//0.7_0.8_amo//";
				file_path_name = strcat(path_name33, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//33//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 34:
				{char path_name34[100] = "E://image_processing//sample//34//0.7_0.8_amo//";
				file_path_name = strcat(path_name34, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//34//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 35:
				{char path_name35[100] = "E://image_processing//sample//35//0.7_0.8_amo//";
				file_path_name = strcat(path_name35, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//35//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 36:
				{char path_name36[100] = "E://image_processing//sample//36//0.7_0.8_amo//";
				file_path_name = strcat(path_name36, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//36//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 37:
				{char path_name37[100] = "E://image_processing//sample//37//0.7_0.8_amo//";
				file_path_name = strcat(path_name37, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//37//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 38:
				{char path_name38[100] = "E://image_processing//sample//38//0.7_0.8_amo//";
				file_path_name = strcat(path_name38, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//38//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 39:
				{char path_name39[100] = "E://image_processing//sample//39//0.7_0.8_amo//";
				file_path_name = strcat(path_name39, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//39//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 40:
				{char path_name40[100] = "E://image_processing//sample//40//0.7_0.8_amo//";
				file_path_name = strcat(path_name40, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//40//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 41:
				{char path_name41[100] = "E://image_processing//sample//41//0.7_0.8_amo//";
				file_path_name = strcat(path_name41, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//41//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 42:
				{char path_name42[100] = "E://image_processing//sample//42//0.7_0.8_amo//";
				file_path_name = strcat(path_name42, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//42//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 43:
				{char path_name43[100] = "E://image_processing//sample//43//0.7_0.8_amo//";
				file_path_name = strcat(path_name43, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//43//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 44:
				{char path_name44[100] = "E://image_processing//sample//44//0.7_0.8_amo//";
				file_path_name = strcat(path_name44, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//44//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 45:
				{char path_name45[100] = "E://image_processing//sample//45//0.7_0.8_amo//";
				file_path_name = strcat(path_name45, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//45//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 46:
				{char path_name46[100] = "E://image_processing//sample//46//0.7_0.8_amo//";
				file_path_name = strcat(path_name46, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//46//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 47:
				{char path_name47[100] = "E://image_processing//sample//47//0.7_0.8_amo//";
				file_path_name = strcat(path_name47, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//47//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 48:
				{char path_name48[100] = "E://image_processing//sample//48//0.7_0.8_amo//";
				file_path_name = strcat(path_name48, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//48//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 49:
				{char path_name49[100] = "E://image_processing//sample//49//0.7_0.8_amo//";
				file_path_name = strcat(path_name49, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//49//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 50:
				{char path_name50[100] = "E://image_processing//sample//50//0.7_0.8_amo//";
				file_path_name = strcat(path_name50, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//50//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 51:
				{char path_name51[100] = "E://image_processing//sample//51//0.7_0.8_amo//";
				file_path_name = strcat(path_name51, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//51//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 52:
				{char path_name52[100] = "E://image_processing//sample//52//0.7_0.8_amo//";
				file_path_name = strcat(path_name52, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//52//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 53:
				{char path_name53[100] = "E://image_processing//sample//53//0.7_0.8_amo//";
				file_path_name = strcat(path_name53, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//53//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 54:
				{char path_name54[100] = "E://image_processing//sample//54//0.7_0.8_amo//";
				file_path_name = strcat(path_name54, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//54//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 55:
				{char path_name55[100] = "E://image_processing//sample//55//0.7_0.8_amo//";
				file_path_name = strcat(path_name55, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//55//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 56:
				{char path_name56[100] = "E://image_processing//sample//56//0.7_0.8_amo//";
				file_path_name = strcat(path_name56, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//56//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 57:
				{char path_name57[100] = "E://image_processing//sample//57//0.7_0.8_amo//";
				file_path_name = strcat(path_name57, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//57//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 58:
				{char path_name58[100] = "E://image_processing//sample//58//0.7_0.8_amo//";
				file_path_name = strcat(path_name58, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//58//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 59:
				{char path_name59[100] = "E://image_processing//sample//59//0.7_0.8_amo//";
				file_path_name = strcat(path_name59, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//59//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 60:
				{char path_name60[100] = "E://image_processing//sample//60//0.7_0.8_amo//";
				file_path_name = strcat(path_name60, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//60//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 61:
				{char path_name61[100] = "E://image_processing//sample//61//0.7_0.8_amo//";
				file_path_name = strcat(path_name61, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 62:
				{char path_name62[100] = "E://image_processing//sample//62//0.7_0.8_amo//";
				file_path_name = strcat(path_name62, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 63:
				{char path_name63[100] = "E://image_processing//sample//63//0.7_0.8_amo//";
				file_path_name = strcat(path_name63, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//63//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 64:
				{char path_name64[100] = "E://image_processing//sample//64//0.7_0.8_amo//";
				file_path_name = strcat(path_name64, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//64//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 65:
				{char path_name65[100] = "E://image_processing//sample//65//0.7_0.8_amo//";
				file_path_name = strcat(path_name65, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//65//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 66:
				{char path_name66[100] = "E://image_processing//sample//66//0.7_0.8_amo//";
				file_path_name = strcat(path_name66, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//66//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 67:
				{char path_name67[100] = "E://image_processing//sample//67//0.7_0.8_amo//";
				file_path_name = strcat(path_name67, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//67//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 68:
				{char path_name68[100] = "E://image_processing//sample//68//0.7_0.8_amo//";
				file_path_name = strcat(path_name68, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//68//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 69:
				{char path_name69[100] = "E://image_processing//sample//69//0.7_0.8_amo//";
				file_path_name = strcat(path_name69, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//69//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 70:
				{char path_name70[100] = "E://image_processing//sample//70//0.7_0.8_amo//";
				file_path_name = strcat(path_name70, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//70//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 71:
				{char path_name71[100] = "E://image_processing//sample//71//0.7_0.8_amo//";
				file_path_name = strcat(path_name71, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//71//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 72:
				{char path_name72[100] = "E://image_processing//sample//72//0.7_0.8_amo//";
				file_path_name = strcat(path_name72, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//72//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 73:
				{char path_name73[100] = "E://image_processing//sample//73//0.7_0.8_amo//";
				file_path_name = strcat(path_name73, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//73//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 74:
				{char path_name74[100] = "E://image_processing//sample//74//0.7_0.8_amo//";
				file_path_name = strcat(path_name74, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//74//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 75:
				{char path_name75[100] = "E://image_processing//sample//75//0.7_0.8_amo//";
				file_path_name = strcat(path_name75, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//75//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 76:
				{char path_name76[100] = "E://image_processing//sample//76//0.7_0.8_amo//";
				file_path_name = strcat(path_name76, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//76//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 77:
				{char path_name77[100] = "E://image_processing//sample//77//0.7_0.8_amo//";
				file_path_name = strcat(path_name77, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//77//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 78:
				{char path_name78[100] = "E://image_processing//sample//78//0.7_0.8_amo//";
				file_path_name = strcat(path_name78, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//78//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 79:
				{char path_name79[100] = "E://image_processing//sample//79//0.7_0.8_amo//";
				file_path_name = strcat(path_name79, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//79//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 80:
				{char path_name80[100] = "E://image_processing//sample//80//0.7_0.8_amo//";
				file_path_name = strcat(path_name80, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//80//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 81:
				{char path_name81[100] = "E://image_processing//sample//81//0.7_0.8_amo//";
				file_path_name = strcat(path_name81, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//81//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				case 82:
				{char path_name82[100] = "E://image_processing//sample//82//0.7_0.8_amo//";
				file_path_name = strcat(path_name82, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//82//0.7_0.8_amo//---文件夹！" << endl; }
				break;
				}
				break;
			}
			if (confidenceLevel >= 0.8) {
				flags_num = 0;
				switch (i)
				{
				case 0:
				{char path_name0[100] = "E://image_processing//sample//00//0.8_abo//";
				file_path_name = strcat(path_name0, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//00//0.8_abo//---文件夹！" << endl; }
				break;
				case 1:
				{char path_name1[100] = "E://image_processing//sample//01//0.8_abo//";
				file_path_name = strcat(path_name1, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//01//0.8_abo//---文件夹！" << endl; }
				break;
				case 2:
				{char path_name2[100] = "E://image_processing//sample//02//0.8_abo//";
				file_path_name = strcat(path_name2, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//02//0.8_abo//---文件夹！" << endl; }
				break;
				case 3:
				{char path_name3[100] = "E://image_processing//sample//03//0.8_abo//";
				file_path_name = strcat(path_name3, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//03//0.8_abo//---文件夹！" << endl; }
				break;
				case 4:
				{char path_name4[100] = "E://image_processing//sample//04//0.8_abo//";
				file_path_name = strcat(path_name4, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//04//0.8_abo//---文件夹！" << endl; }
				break;
				case 5:
				{char path_name5[100] = "E://image_processing//sample//05//0.8_abo//";
				file_path_name = strcat(path_name5, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//05//0.8_abo//---文件夹！" << endl; }
				break;
				case 6:
				{char path_name6[100] = "E://image_processing//sample//06//0.8_abo//";
				file_path_name = strcat(path_name6, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//06//0.8_abo//---文件夹！" << endl; }
				break;
				case 7:
				{char path_name7[100] = "E://image_processing//sample//07//0.8_abo//";
				file_path_name = strcat(path_name7, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//07//0.8_abo//---文件夹！" << endl; }
				break;
				case 8:
				{char path_name8[100] = "E://image_processing//sample//08//0.8_abo//";
				file_path_name = strcat(path_name8, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//08//0.8_abo//---文件夹！" << endl; }
				break;
				case 9:
				{char path_name9[100] = "E://image_processing//sample//09//0.8_abo//";
				file_path_name = strcat(path_name9, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//09//0.8_abo//---文件夹！" << endl; }
				break;
				case 10:
				{char path_name10[100] = "E://image_processing//sample//10//0.8_abo//";
				file_path_name = strcat(path_name10, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//10//0.8_abo//---文件夹！" << endl; }
				break;
				case 11:
				{char path_name11[100] = "E://image_processing//sample//11//0.8_abo//";
				file_path_name = strcat(path_name11, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//11//0.8_abo//---文件夹！" << endl; }
				break;
				case 12:
				{char path_name12[100] = "E://image_processing//sample//12//0.8_abo//";
				file_path_name = strcat(path_name12, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//12//0.8_abo//---文件夹！" << endl; }
				break;
				case 13:
				{char path_name13[100] = "E://image_processing//sample//13//0.8_abo//";
				file_path_name = strcat(path_name13, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//13//0.8_abo//---文件夹！" << endl; }
				break;
				case 14:
				{char path_name14[100] = "E://image_processing//sample//14//0.8_abo//";
				file_path_name = strcat(path_name14, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//14//0.8_abo//---文件夹！" << endl; }
				break;
				case 15:
				{char path_name15[100] = "E://image_processing//sample//15//0.8_abo//";
				file_path_name = strcat(path_name15, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//15//0.8_abo//---文件夹！" << endl; }
				break;
				case 16:
				{char path_name16[100] = "E://image_processing//sample//16//0.8_abo//";
				file_path_name = strcat(path_name16, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//16//0.8_abo//---文件夹！" << endl; }
				break;
				case 17:
				{char path_name17[100] = "E://image_processing//sample//17//0.8_abo//";
				file_path_name = strcat(path_name17, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//17//0.8_abo//---文件夹！" << endl; }
				break;
				case 18:
				{char path_name18[100] = "E://image_processing//sample//18//0.8_abo//";
				file_path_name = strcat(path_name18, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//18//0.8_abo//---文件夹！" << endl; }
				break;
				case 19:
				{char path_name19[100] = "E://image_processing//sample//19//0.8_abo//";
				file_path_name = strcat(path_name19, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//19//0.8_abo//---文件夹！" << endl; }
				break;
				case 20:
				{char path_name20[100] = "E://image_processing//sample//20//0.8_abo//";
				file_path_name = strcat(path_name20, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//20//0.8_abo//---文件夹！" << endl; }
				break;
				case 21:
				{char path_name21[100] = "E://image_processing//sample//21//0.8_abo//";
				file_path_name = strcat(path_name21, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//21//0.8_abo//---文件夹！" << endl; }
				break;
				case 22:
				{char path_name22[100] = "E://image_processing//sample//22//0.8_abo//";
				file_path_name = strcat(path_name22, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//22//0.8_abo//---文件夹！" << endl; }
				break;
				case 23:
				{char path_name23[100] = "E://image_processing//sample//23//0.8_abo//";
				file_path_name = strcat(path_name23, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//23//0.8_abo//---文件夹！" << endl; }
				break;
				case 24:
				{char path_name24[100] = "E://image_processing//sample//24//0.8_abo//";
				file_path_name = strcat(path_name24, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//24//0.8_abo//---文件夹！" << endl; }
				break;
				case 25:
				{char path_name25[100] = "E://image_processing//sample//25//0.8_abo//";
				file_path_name = strcat(path_name25, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//25//0.8_abo//---文件夹！" << endl; }
				break;
				case 26:
				{char path_name26[100] = "E://image_processing//sample//26//0.8_abo//";
				file_path_name = strcat(path_name26, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//26//0.8_abo//---文件夹！" << endl; }
				break;
				case 27:
				{char path_name27[100] = "E://image_processing//sample//27//0.8_abo//";
				file_path_name = strcat(path_name27, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//27//0.8_abo//---文件夹！" << endl; }
				break;
				case 28:
				{char path_name28[100] = "E://image_processing//sample//28//0.8_abo//";
				file_path_name = strcat(path_name28, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//28//0.8_abo//---文件夹！" << endl; }
				break;
				case 29:
				{char path_name29[100] = "E://image_processing//sample//29//0.8_abo//";
				file_path_name = strcat(path_name29, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//29//0.8_abo//---文件夹！" << endl; }
				break;
				case 30:
				{char path_name30[100] = "E://image_processing//sample//30//0.8_abo//";
				file_path_name = strcat(path_name30, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//30//0.8_abo//---文件夹！" << endl; }
				break;
				case 31:
				{char path_name31[100] = "E://image_processing//sample//31//0.8_abo//";
				file_path_name = strcat(path_name31, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//31//0.8_abo//---文件夹！" << endl; }
				break;
				case 32:
				{char path_name32[100] = "E://image_processing//sample//32//0.8_abo//";
				file_path_name = strcat(path_name32, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//32//0.8_abo//---文件夹！" << endl; }
				break;
				case 33:
				{char path_name33[100] = "E://image_processing//sample//33//0.8_abo//";
				file_path_name = strcat(path_name33, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//33//0.8_abo//---文件夹！" << endl; }
				break;
				case 34:
				{char path_name34[100] = "E://image_processing//sample//34//0.8_abo//";
				file_path_name = strcat(path_name34, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//34//0.8_abo//---文件夹！" << endl; }
				break;
				case 35:
				{char path_name35[100] = "E://image_processing//sample//35//0.8_abo//";
				file_path_name = strcat(path_name35, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//35//0.8_abo//---文件夹！" << endl; }
				break;
				case 36:
				{char path_name36[100] = "E://image_processing//sample//36//0.8_abo//";
				file_path_name = strcat(path_name36, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//36//0.8_abo//---文件夹！" << endl; }
				break;
				case 37:
				{char path_name37[100] = "E://image_processing//sample//37//0.8_abo//";
				file_path_name = strcat(path_name37, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//37//0.8_abo//---文件夹！" << endl; }
				break;
				case 38:
				{char path_name38[100] = "E://image_processing//sample//38//0.8_abo//";
				file_path_name = strcat(path_name38, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//38//0.8_abo//---文件夹！" << endl; }
				break;
				case 39:
				{char path_name39[100] = "E://image_processing//sample//39//0.8_abo//";
				file_path_name = strcat(path_name39, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//39//0.8_abo//---文件夹！" << endl; }
				break;
				case 40:
				{char path_name40[100] = "E://image_processing//sample//40//0.8_abo//";
				file_path_name = strcat(path_name40, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//40//0.8_abo//---文件夹！" << endl; }
				break;
				case 41:
				{char path_name41[100] = "E://image_processing//sample//41//0.8_abo//";
				file_path_name = strcat(path_name41, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//41//0.8_abo//---文件夹！" << endl; }
				break;
				case 42:
				{char path_name42[100] = "E://image_processing//sample//42//0.8_abo//";
				file_path_name = strcat(path_name42, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//42//0.8_abo//---文件夹！" << endl; }
				break;
				case 43:
				{char path_name43[100] = "E://image_processing//sample//43//0.8_abo//";
				file_path_name = strcat(path_name43, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//43//0.8_abo//---文件夹！" << endl; }
				break;
				case 44:
				{char path_name44[100] = "E://image_processing//sample//44//0.8_abo//";
				file_path_name = strcat(path_name44, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//44//0.8_abo//---文件夹！" << endl; }
				break;
				case 45:
				{char path_name45[100] = "E://image_processing//sample//45//0.8_abo//";
				file_path_name = strcat(path_name45, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//45//0.8_abo//---文件夹！" << endl; }
				break;
				case 46:
				{char path_name46[100] = "E://image_processing//sample//46//0.8_abo//";
				file_path_name = strcat(path_name46, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//46//0.8_abo//---文件夹！" << endl; }
				break;
				case 47:
				{char path_name47[100] = "E://image_processing//sample//47//0.8_abo//";
				file_path_name = strcat(path_name47, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//47//0.8_abo//---文件夹！" << endl; }
				break;
				case 48:
				{char path_name48[100] = "E://image_processing//sample//48//0.8_abo//";
				file_path_name = strcat(path_name48, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//48//0.8_abo//---文件夹！" << endl; }
				break;
				case 49:
				{char path_name49[100] = "E://image_processing//sample//49//0.8_abo//";
				file_path_name = strcat(path_name49, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//49//0.8_abo//---文件夹！" << endl; }
				break;
				case 50:
				{char path_name50[100] = "E://image_processing//sample//50//0.8_abo//";
				file_path_name = strcat(path_name50, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//50//0.8_abo//---文件夹！" << endl; }
				break;
				case 51:
				{char path_name51[100] = "E://image_processing//sample//51//0.8_abo//";
				file_path_name = strcat(path_name51, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//51//0.8_abo//---文件夹！" << endl; }
				break;
				case 52:
				{char path_name52[100] = "E://image_processing//sample//52//0.8_abo//";
				file_path_name = strcat(path_name52, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//52//0.8_abo//---文件夹！" << endl; }
				break;
				case 53:
				{char path_name53[100] = "E://image_processing//sample//53//0.8_abo//";
				file_path_name = strcat(path_name53, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//53//0.8_abo//---文件夹！" << endl; }
				break;
				case 54:
				{char path_name54[100] = "E://image_processing//sample//54//0.8_abo//";
				file_path_name = strcat(path_name54, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//54//0.8_abo//---文件夹！" << endl; }
				break;
				case 55:
				{char path_name55[100] = "E://image_processing//sample//55//0.8_abo//";
				file_path_name = strcat(path_name55, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//55//0.8_abo//---文件夹！" << endl; }
				break;
				case 56:
				{char path_name56[100] = "E://image_processing//sample//56//0.8_abo//";
				file_path_name = strcat(path_name56, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//56//0.8_abo//---文件夹！" << endl; }
				break;
				case 57:
				{char path_name57[100] = "E://image_processing//sample//57//0.8_abo//";
				file_path_name = strcat(path_name57, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//57//0.8_abo//---文件夹！" << endl; }
				break;
				case 58:
				{char path_name58[100] = "E://image_processing//sample//58//0.8_abo//";
				file_path_name = strcat(path_name58, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//58//0.8_abo//---文件夹！" << endl; }
				break;
				case 59:
				{char path_name59[100] = "E://image_processing//sample//59//0.8_abo//";
				file_path_name = strcat(path_name59, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//59//0.8_abo//---文件夹！" << endl; }
				break;
				case 60:
				{char path_name60[100] = "E://image_processing//sample//60//0.8_abo//";
				file_path_name = strcat(path_name60, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//60//0.8_abo//---文件夹！" << endl; }
				break;
				case 61:
				{char path_name61[100] = "E://image_processing//sample//61//0.8_abo//";
				file_path_name = strcat(path_name61, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.8_abo//---文件夹！" << endl; }
				break;
				case 62:
				{char path_name62[100] = "E://image_processing//sample//62//0.8_abo//";
				file_path_name = strcat(path_name62, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//61//0.8_abo//---文件夹！" << endl; }
				break;
				case 63:
				{char path_name63[100] = "E://image_processing//sample//63//0.8_abo//";
				file_path_name = strcat(path_name63, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//63//0.8_abo//---文件夹！" << endl; }
				break;
				case 64:
				{char path_name64[100] = "E://image_processing//sample//64//0.8_abo//";
				file_path_name = strcat(path_name64, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//64//0.8_abo//---文件夹！" << endl; }
				break;
				case 65:
				{char path_name65[100] = "E://image_processing//sample//65//0.8_abo//";
				file_path_name = strcat(path_name65, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//65//0.8_abo//---文件夹！" << endl; }
				break;
				case 66:
				{char path_name66[100] = "E://image_processing//sample//66//0.8_abo//";
				file_path_name = strcat(path_name66, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//66//0.8_abo//---文件夹！" << endl; }
				break;
				case 67:
				{char path_name67[100] = "E://image_processing//sample//67//0.8_abo//";
				file_path_name = strcat(path_name67, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//67//0.8_abo//---文件夹！" << endl; }
				break;
				case 68:
				{char path_name68[100] = "E://image_processing//sample//68//0.8_abo//";
				file_path_name = strcat(path_name68, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//68//0.8_abo//---文件夹！" << endl; }
				break;
				case 69:
				{char path_name69[100] = "E://image_processing//sample//69//0.8_abo//";
				file_path_name = strcat(path_name69, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//69//0.8_abo//---文件夹！" << endl; }
				break;
				case 70:
				{char path_name70[100] = "E://image_processing//sample//70//0.8_abo//";
				file_path_name = strcat(path_name70, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//70//0.8_abo//---文件夹！" << endl; }
				break;
				case 71:
				{char path_name71[100] = "E://image_processing//sample//71//0.8_abo//";
				file_path_name = strcat(path_name71, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//71//0.8_abo//---文件夹！" << endl; }
				break;
				case 72:
				{char path_name72[100] = "E://image_processing//sample//72//0.8_abo//";
				file_path_name = strcat(path_name72, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//72//0.8_abo//---文件夹！" << endl; }
				break;
				case 73:
				{char path_name73[100] = "E://image_processing//sample//73//0.8_abo//";
				file_path_name = strcat(path_name73, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//73//0.8_abo//---文件夹！" << endl; }
				break;
				case 74:
				{char path_name74[100] = "E://image_processing//sample//74//0.8_abo//";
				file_path_name = strcat(path_name74, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//74//0.8_abo//---文件夹！" << endl; }
				break;
				case 75:
				{char path_name75[100] = "E://image_processing//sample//75//0.8_abo//";
				file_path_name = strcat(path_name75, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//75//0.8_abo//---文件夹！" << endl; }
				break;
				case 76:
				{char path_name76[100] = "E://image_processing//sample//76//0.8_abo//";
				file_path_name = strcat(path_name76, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//76//0.8_abo//---文件夹！" << endl; }
				break;
				case 77:
				{char path_name77[100] = "E://image_processing//sample//77//0.8_abo//";
				file_path_name = strcat(path_name77, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//77//0.8_abo//---文件夹！" << endl; }
				break;
				case 78:
				{char path_name78[100] = "E://image_processing//sample//78//0.8_abo//";
				file_path_name = strcat(path_name78, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//78//0.8_abo//---文件夹！" << endl; }
				break;
				case 79:
				{char path_name79[100] = "E://image_processing//sample//79//0.8_abo//";
				file_path_name = strcat(path_name79, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//79//0.8_abo//---文件夹！" << endl; }
				break;
				case 80:
				{char path_name80[100] = "E://image_processing//sample//80//0.8_abo//";
				file_path_name = strcat(path_name80, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//80//0.8_abo//---文件夹！" << endl; }
				break;
				case 81:
				{char path_name81[100] = "E://image_processing//sample//81//0.8_abo//";
				file_path_name = strcat(path_name81, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//81//0.8_abo//---文件夹！" << endl; }
				break;
				case 82:
				{char path_name82[100] = "E://image_processing//sample//82//0.8_abo//";
				file_path_name = strcat(path_name82, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//sample//82//0.8_abo//---文件夹！" << endl; }
				break;
				}
				break;
			}
			else if (flags_num == 1 && i == 82) {
				char path_name_trash[100] = "E://image_processing//trash//";
				file_path_name = strcat(path_name_trash, img_filename);
				copy_file_function(image_path2, file_path_name);
				cout << image_path2 << "移入---E://image_processing//trash//---文件夹！" << endl;
			}
			MFloat confidenceLevel;
		}
		cvReleaseImage(&img1);
	}
	for (int i = 0; i < 83; i++) {
		SafeFree(copyfeature1[i].feature);		//释放内存
	}
	cout << "全部结束" << endl;
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

int main() {
	MHandle handle = init();
	face_match(handle);
	uinit(handle);
	system("pause");
	return 0;
}