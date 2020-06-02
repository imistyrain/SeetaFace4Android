/*
 *
 * This file is part of the open-source SeetaFace engine, which includes three modules:
 * SeetaFace Detection, SeetaFace Alignment, and SeetaFace Identification.
 *
 * This file is an example of how to use SeetaFace engine for face alignment, the
 * face alignment method described in the following paper:
 *
 *
 *   Coarse-to-Fine Auto-Encoder Networks (CFAN) for Real-Time Face Alignment, 
 *   Jie Zhang, Shiguang Shan, Meina Kan, Xilin Chen. In Proceeding of the
 *   European Conference on Computer Vision (ECCV), 2014
 *
 *
 * Copyright (C) 2016, Visual Information Processing and Learning (VIPL) group,
 * Institute of Computing Technology, Chinese Academy of Sciences, Beijing, China.
 *
 * The codes are mainly developed by Jie Zhang (a Ph.D supervised by Prof. Shiguang Shan)
 *
 * As an open-source face recognition engine: you can redistribute SeetaFace source codes
 * and/or modify it under the terms of the BSD 2-Clause License.
 *
 * You should have received a copy of the BSD 2-Clause License along with the software.
 * If not, see < https://opensource.org/licenses/BSD-2-Clause>.
 *
 * Contact Info: you can send an email to SeetaFace@vipl.ict.ac.cn for any problems.
 *
 * Note: the above information must be kept whenever or wherever the codes are used.
 *
 */

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "mropencv.h"
#include "face_detection.h"
#define  SEETA_ALIGNMENT_LANDMARK 0
#include "face_alignment.h"

class CSeetaFaceDetector
{
public:
	static CSeetaFaceDetector *getInstance()
	{
		static CSeetaFaceDetector instance;;
		return &instance;
	}
	cv::Mat detectSingleImage(cv::Mat &img)
	{
		cv::Mat img_gray;
		if (img.channels() != 1)
			cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img;
		seeta::ImageData image_data;
		image_data.data = img_gray.data;
		image_data.width = img_gray.cols;
		image_data.height = img_gray.rows;
		image_data.num_channels = 1;
		std::vector<seeta::FaceInfo> faces = pfd->Detect(image_data);
		int32_t face_num = static_cast<int32_t>(faces.size());

		if (face_num == 0)
		{
			std::cout << "No Faces" << std::endl;
			return img;
		}
#if SEETA_ALIGNMENT_LANDMARK
		seeta::FacialLandmark points[5];
		pfa->PointDetectLandmarks(image_data, faces[0], points);
#endif
		cv::Mat img_color = img.clone();
		cv::rectangle(img_color, cv::Point(faces[0].bbox.x, faces[0].bbox.y), cv::Point(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0));
#if SEETA_ALIGNMENT_LANDMARK		
		for (int i = 0; i < pts_num; i++)
		{
			cv::circle(img_color, cv::Point(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
		}
#endif
		return img_color;
	}
private:
	CSeetaFaceDetector()
	{
		pfd = new seeta::FaceDetection("seeta_fd_frontal_v1.0.bin");
		pfd->SetMinFaceSize(40);
		pfd->SetScoreThresh(2.f);
		pfd->SetImagePyramidScaleFactor(0.8f);
		pfd->SetWindowStep(4, 4);
#if SEETA_ALIGNMENT_LANDMARK
		pfa=new seeta::FaceAlignment("seeta_fa_v1.0.bin");
#endif
	}
	~CSeetaFaceDetector()
	{
		delete pfd;
#if SEETA_ALIGNMENT_LANDMARK
		delete pfa;
#endif
	}
	seeta::FaceDetection *pfd;
#if SEETA_ALIGNMENT_LANDMARK
	seeta::FaceAlignment *pfa;
	const int pts_num = 5;
#endif
};

int fromvideo(int argc, char** argv)
{
	cv::VideoCapture capture(0);
	cv::Mat img;
	while (1)
	{
		capture >> img;
		if (!img.data)
		{
			return -1;
		}
		cv::Mat detected=CSeetaFaceDetector::getInstance()->detectSingleImage(img);
		cv::imshow("img", detected);
		cv::waitKey(1);
	}
	return 0;
}

int main(int argc, char** argv)
{
	fromvideo(argc, argv);
	return 0;
}

int fromimg(int argc, char** argv)
{
	// Initialize face detection model
	seeta::FaceDetection detector("seeta_fd_frontal_v1.0.bin");
	detector.SetMinFaceSize(40);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	// Initialize face alignment model 
	seeta::FaceAlignment point_detector("seeta_fa_v1.0.bin");


	cv::Mat img_color = cv::imread("image_0001.png");
	int pts_num = 5;
	cv::Mat img_gray;
	if (img_color.channels() != 1)
		cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);
	else
		img_gray = img_color;
	seeta::ImageData image_data;
	image_data.data = img_gray.data;
	image_data.width = img_gray.cols;
	image_data.height = img_gray.rows;
	image_data.num_channels = 1;

	// Detect faces
	std::vector<seeta::FaceInfo> faces = detector.Detect(image_data);
	int32_t face_num = static_cast<int32_t>(faces.size());

	if (face_num == 0)
	{
		std::cout << "No Faces" << std::endl;
		return 0;
	}

	// Detect 5 facial landmarks
	seeta::FacialLandmark points[5];
	point_detector.PointDetectLandmarks(image_data, faces[0], points);
	cv::rectangle(img_color, cv::Point(faces[0].bbox.x, faces[0].bbox.y), cv::Point(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0));
	for (int i = 0; i < pts_num; i++)
	{
		cv::circle(img_color, cv::Point(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
	}

	cv::imwrite("result.jpg", img_color);
	cv::imshow("img", img_color);
	cv::waitKey();

	return 0;
}

int fromvideo0(int argc, char** argv)
{
	// Initialize face detection model
	seeta::FaceDetection detector("seeta_fd_frontal_v1.0.bin");
	detector.SetMinFaceSize(80);
	detector.SetScoreThresh(2.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

	// Initialize face alignment model 
	seeta::FaceAlignment point_detector("seeta_fa_v1.0.bin");

	cv::VideoCapture capture(0);
	cv::Mat img_color;
	const int pts_num = 5;
	while (1)
	{
		capture >> img_color;
		if (!img_color.data)
			return -1;
		cv::TickMeter tm;
		tm.start();
		cv::Mat img_gray;
		if (img_color.channels() != 1)
			cv::cvtColor(img_color, img_gray, cv::COLOR_BGR2GRAY);
		else
			img_gray = img_color;
		seeta::ImageData image_data;
		image_data.data = img_gray.data;
		image_data.width = img_gray.cols;
		image_data.height = img_gray.rows;
		image_data.num_channels = 1;

		// Detect faces
		std::vector<seeta::FaceInfo> faces = detector.Detect(image_data);
		int32_t face_num = static_cast<int32_t>(faces.size());

		if (face_num == 0)
		{
			std::cout << "No Faces" << std::endl;
			return 0;
		}

		// Detect 5 facial landmarks
		seeta::FacialLandmark points[5];
		point_detector.PointDetectLandmarks(image_data, faces[0], points);
		tm.stop();
		std::cout << "cost " << tm.getTimeMilli() << "ms" << std::endl;
		cv::rectangle(img_color, cv::Point(faces[0].bbox.x, faces[0].bbox.y), cv::Point(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0));
		for (int i = 0; i < pts_num; i++)
		{
			cv::circle(img_color, cv::Point(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
		}
		cv::imshow("img", img_color);
		cv::waitKey(1);
	}
	return 0;
}
