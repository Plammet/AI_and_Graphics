#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>       // exp 

#define IM_TYPE	CV_8UC3

using namespace cv;

// Image Type
// "G" for GrayScale Image, "C" for Color Image
#if (IM_TYPE == CV_8UC3)
typedef uchar G;
typedef cv::Vec3b C;
#elif (IM_TYPE == CV_16SC3)
typedef short G;
typedef Vec3s C;
#elif (IM_TYPE == CV_32SC3)
typedef int G;
typedef Vec3i C;
#elif (IM_TYPE == CV_32FC3)
typedef float G;
typedef Vec3f C;
#elif (IM_TYPE == CV_64FC3)
typedef double G;
typedef Vec3d C;
#endif

Mat sobelfilter(const Mat input);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);
	output = sobelfilter(input);

	namedWindow("Sobel Filter", WINDOW_AUTOSIZE);
	imshow("Sobel Filter", output);


	waitKey(0);

	return 0;
}

Mat sobelfilter(const Mat input) {
	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int tempa = 0;
	int tempb = 0;
	int n = 1; // Sobel Filter Kernel N
	float result;

	// Initialiazing 2 Kernel Matrix with 3x3 size for Sx and Sy
	//Fill code to initialize Sobel filter kernel matrix for Sx and Sy (Given in the lecture notes)
	int Sx[3][3] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	int Sy[3][3] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

	Mat output = Mat::zeros(row, col, input.type());

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			float sumX_r = 0.0;
			float sumX_g = 0.0;
			float sumX_b = 0.0;
			float sumY_r = 0.0;
			float sumY_g = 0.0;
			float sumY_b = 0.0;
			// Use mirroring boundary process
			for (int a = -n; a <= n; a++) { // for each kernel window
				for (int b = -n; b <= n; b++) {
					if (i + a > row - 1) {  //mirroring for the border pixels
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					if (j + b > col - 1) {
						tempb = j - b;
					}
					else if (j + b < 0) {
						tempb = -(j + b);
					}
					else {
						tempb = j + b;
					}
					sumX_r += Sx[a + n][b + n] * (float)(input.at<C>(tempa, tempb)[0]);
					sumX_g += Sx[a + n][b + n] * (float)(input.at<C>(tempa, tempb)[1]);
					sumX_b += Sx[a + n][b + n] * (float)(input.at<C>(tempa, tempb)[2]);
					sumY_r += Sy[a + n][b + n] * (float)(input.at<C>(tempa, tempb)[0]);
					sumY_g += Sy[a + n][b + n] * (float)(input.at<C>(tempa, tempb)[1]);
					sumY_b += Sy[a + n][b + n] * (float)(input.at<C>(tempa, tempb)[2]);
				}
			}

			float result_r = sqrt(pow(sumX_r, 2) + pow(sumY_r, 2));
			if (result_r < 0) result_r = 0;
			if (result_r > 255) result_r = 255;
			
			float result_g = sqrt(pow(sumX_g, 2) + pow(sumY_g, 2));
			if (result_g < 0) result_g = 0;
			if (result_g > 255) result_g = 255;

			float result_b = sqrt(pow(sumX_b, 2) + pow(sumY_b, 2));
			if (result_b < 0) result_b = 0;
			if (result_b > 255) result_b = 255;

			//ver1
			result = (result_r + result_g + result_b)/3;
			output.at<C>(i, j)[0] = result;
			output.at<C>(i, j)[1] = result;
			output.at<C>(i, j)[2] = result;

			//ver2
			//output.at<C>(i, j)[0] = result_r;
			//output.at<C>(i, j)[1] = result_g;
			//output.at<C>(i, j)[2] = result_b;
			
		}
	}
	return output;
}