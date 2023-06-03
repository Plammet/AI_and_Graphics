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

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt);
Mat unsharpMask(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;
	Mat output;

	cvtColor(input, input_gray, CV_RGB2GRAY);

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);
	output =unsharpMask(input_gray, 3, 3, 3, "mirroring", 0.4); //Boundary process: zero-paddle, mirroring, adjustkernel

	namedWindow("Unsharp Masking", WINDOW_AUTOSIZE);
	imshow("Unsharp Masking", output);


	waitKey(0);

	return 0;
}

Mat unsharpMask(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k) {
	Mat L = gaussianfilter(input, n, sigmaT, sigmaS, opt);
	Mat output(input.rows, input.cols, input.type());
	float temp;

	for (int i = 0; i < input.rows; i++) {
		for (int j = 0; j < input.cols; j++) {
			temp = abs(input.at<G>(i, j) - (k * L.at<G>(i, j))) / (1 - k);
			if (temp > 255) temp = 255;

			output.at<G>(i, j) = temp;
		}
	}
	return output;
}

Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt) {

	Mat kernel_s;
	Mat kernel_t;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float denom_s = 0.0;
	float denom_t = 0.0;

	// Initialiazing Kernel Matrix 
	kernel_s = Mat::zeros(kernel_size, 1, CV_32F);
	kernel_t = Mat::zeros(1, kernel_size, CV_32F);

	for (int a = -n; a <= n; a++) {  // Denominator in m(s,t)
		float value_s = exp(-(pow(a, 2) / (2 * pow(sigmaS, 2))));
		kernel_s.at<float>(a + n, 0) = value_s;
		denom_s += value_s;
	}
	for (int b = -n; b <= n; b++) {
		float value_t = exp(-(pow(b, 2) / (2 * pow(sigmaT, 2))));
		kernel_t.at<float>(0, b + n) = value_t;
		denom_t += value_t;
	}

	for (int a = -n; a <= n; a++) {
		kernel_s.at<float>(a + n, 0) /= denom_s;
	}
	for (int b = -n; b <= n; b++) {
		kernel_t.at<float>(0, b + n) /= denom_t;
	}

	Mat output = Mat::zeros(row, col, input.type());
	Mat temp = input.clone();

	if (!strcmp(opt, "zero-paddle")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					if ((i + a <= row - 1) && (i + a >= 0))
						sum1 += kernel_s.at<float>(a + n, 0) * (float)(input.at<G>(i + a, j));
				}
				temp.at<G>(i, j) = sum1;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int b = -n; b <= n; b++) {
					if ((j + b <= col - 1) && (j + b >= 0))
						sum1 += kernel_t.at<float>(0, b + n) * (float)(temp.at<G>(i, j + b));
				}
				output.at<G>(i, j) = sum1;
			}
		}
	}

	else if (!strcmp(opt, "mirroring")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					if (i + a > row - 1) {
						tempa = i - a;
					}
					else if (i + a < 0) {
						tempa = -(i + a);
					}
					else {
						tempa = i + a;
					}
					sum1 += kernel_s.at<float>(a + n, 0) * (float)(input.at<G>(tempa, j));
				}
				temp.at<G>(i, j) = sum1;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int b = -n; b <= n; b++) {
					if (j + b > col - 1) {
						tempb = j - b;
					}
					else if (j + b < 0) {
						tempb = -(j + b);
					}
					else {
						tempb = j + b;
					}
					sum1 += kernel_t.at<float>(0, b + n) * (float)(temp.at<G>(i, tempb));
				}
				output.at<G>(i, j) = sum1;
			}
		}
	}

	else if (!strcmp(opt, "adjustkernel")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					if ((i + a <= row - 1) && (i + a >= 0)) {
						sum1 += kernel_s.at<float>(a + n, 0) * (float)(input.at<G>(i + a, j));
						sum2 += kernel_s.at<float>(a + n, 0);
					}
				}
				temp.at<G>(i, j) = sum1 / sum2;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int b = -n; b <= n; b++) {
					if ((j + b <= col - 1) && (j + b >= 0)) {
						sum1 += kernel_t.at<float>(0, b + n) * (float)(temp.at<G>(i, j + b));
						sum2 += kernel_t.at<float>(0, b + n);
					}
				}
				output.at<G>(i, j) = sum1 / sum2;
			}
		}
	}
	return output;
}