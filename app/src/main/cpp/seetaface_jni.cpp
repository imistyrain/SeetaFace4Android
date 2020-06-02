#include "seetaface_jni.h"
#include "opencv2/opencv.hpp"
#include <string>
#include <vector>
#include "util.h"
#define  SEETA_ALIGNMENT_LANDMARK 0
#include "face_detection.h"
#include "face_alignment.h"
#include "fstream"
#include <dirent.h>
#include "thread"
using namespace std;
using namespace cv;

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
		cv::rectangle(img_color, cv::Point(faces[0].bbox.x, faces[0].bbox.y), cv::Point(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0),2);
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
		pfd = new seeta::FaceDetection("/sdcard/mrface/fd_frontal_v1.0.bin");
		pfd->SetMinFaceSize(160);
		pfd->SetScoreThresh(2.f);
		pfd->SetImagePyramidScaleFactor(0.8f);
		pfd->SetWindowStep(4, 4);
#if SEETA_ALIGNMENT_LANDMARK
		pfa=new seeta::FaceAlignment("/sdcard/mrface/fa_v1.0.bin");
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

class CSeetaFace
{
public:
	static CSeetaFace *getInstance()
	{
		static CSeetaFace instance;
		return &instance;
	}
	seeta::FaceInfo faceinfo;
private:
	CSeetaFace(){
		pfd = new seeta::FaceDetection("/sdcard/mrface/fd_frontal_v1.0.bin");
		pfd->SetMinFaceSize(200);
		pfd->SetScoreThresh(2.f);
		pfd->SetImagePyramidScaleFactor(0.8f);
		pfd->SetWindowStep(4, 4);
		pfa = new seeta::FaceAlignment("/sdcard/mrface/fa_v1.0.bin");
	}
	~CSeetaFace()
	{
		delete pfd;
		delete pfa;
	}
	seeta::FaceDetection *pfd;
	seeta::FaceAlignment *pfa;
	const int pts_num = 5;
};

extern "C" JNIEXPORT int JNICALL Java_yanyu_com_seetaface4android_MRSeetaFaceUtil_seetaFaceDetectFaces
        (JNIEnv *env, jclass obj,jlong imageRgba, jlong imageResult)
{
    try
    {
        Mat& img = *(Mat*)imageRgba;
        resize(img,img,Size(640,480));
        cv::flip(img,img,1);
        transpose(img,img);
        Mat &imgResult= *(Mat*)imageResult;
        img=CSeetaFaceDetector::getInstance()->detectSingleImage(img);
        transpose(img,img);
        imgResult=img;
    }
    catch(cv::Exception& e)
    {
        LOGD("seetaDetectFaces  caught cv::Exception: %s", e.what());
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je)
            je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("seetaDetectFaces  caught unknown exception");
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code seetaDetectFaces");
    }
    return 0;
}