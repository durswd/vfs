#include <opencv2/opencv.hpp>
#include <stereotgv/stereotgv.h>

void showDepthJet(std::string windowName, cv::Mat image, float maxDepth, bool shouldWait = true) {
	cv::Mat u_norm, u_gray, u_color;
	u_norm = image * 256.0f / maxDepth;
	u_norm.convertTo(u_gray, CV_8UC1);
	cv::applyColorMap(u_gray, u_color, cv::COLORMAP_JET);

	cv::imshow(windowName, u_color);
	if (shouldWait) cv::waitKey();
}

int main() {
	cv::Mat im1 = cv::imread("fs1.png", cv::IMREAD_GRAYSCALE);
	cv::Mat im2 = cv::imread("fs2.png", cv::IMREAD_GRAYSCALE);

	StereoTgv* stereotgv = new StereoTgv();
	int width = 848;
	int height = 800;
	float stereoScaling = 1.0f; // Bugged, don't change
	int nLevel = 4;		// Limith such that minimum width > 50pix
	float fScale = 2.0f;	// Ideally 2.0
	int nWarpIters = 5;	// Change to reduce processing time
	int nSolverIters = 5;	// Change to reduce processing time
	float lambda = 5.0f;	// Change to increase data dependency
	float beta = 9.0f;		
	float gamma = 0.85f;
	float alpha0 = 17.0f;
	float alpha1 = 1.2f;
	float timeStepLambda = 1.0f;

	int stereoWidth = (int)((float)width / stereoScaling);
	int stereoHeight = (int)((float)height / stereoScaling);
	//stereotgv->baseline = 0.0642f; // Not used
	//stereotgv->focal = 285.8557f / stereoScaling;
	stereotgv->initialize(stereoWidth, stereoHeight, beta, gamma, alpha0, alpha1,
		timeStepLambda, lambda, nLevel, fScale, nWarpIters, nSolverIters);
	stereotgv->visualizeResults = true;

	// Load fisheye Mask
	cv::Mat fisheyeMask8 = cv::imread("mask.png", cv::IMREAD_GRAYSCALE);
	cv::Mat fisheyeMask;
	fisheyeMask8.convertTo(fisheyeMask, CV_32F, 1.0 / 255.0);
	stereotgv->copyMaskToDevice(fisheyeMask);

	// Load vector fields
	cv::Mat translationVector, calibrationVector;
	translationVector = cv::readOpticalFlow("translationVector.flo");
	calibrationVector = cv::readOpticalFlow("calibrationVector.flo");
	stereotgv->loadVectorFields(translationVector, calibrationVector);

	// Solve stereo depth
	cv::Mat equi1, equi2;
	cv::equalizeHist(im1, equi1);
	cv::equalizeHist(im2, equi2);

	clock_t start = clock();
	stereotgv->copyImagesToDevice(equi1, equi2);
	stereotgv->solveStereoForwardMasked();

	cv::Mat disparity = cv::Mat(stereoHeight, stereoWidth, CV_32FC2);
	stereotgv->copyDisparityToHost(disparity);
	clock_t timeElapsed = (clock() - start);
	std::cout << "time: " << timeElapsed << " ms ";
	cv::writeOpticalFlow("disparity.flo", disparity);

	std::vector<cv::Mat> planes;
	cv::split(disparity, planes);

	cv::imshow("opt1", planes[0] / 16.0f);
	cv::imshow("opt2", planes[1] / 16.0f);

	cv::waitKey();
	return 0;
}