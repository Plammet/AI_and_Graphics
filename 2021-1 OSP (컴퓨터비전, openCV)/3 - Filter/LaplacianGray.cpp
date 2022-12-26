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

Mat Laplacianfilter(const Mat input);

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
	output = Laplacianfilter(input_gray); //Boundary process: zero-paddle, mirroring, adjustkernel

	namedWindow("Laplacian Filter", WINDOW_AUTOSIZE);
	imshow("Laplacian Filter", output);

	waitKey(0);

	return 0;
}


Mat Laplacianfilter(const Mat input) {
	Mat kernel;

	int row = input.rows;
	int col = input.cols;
	int tempa = 0;
	int tempb = 0;
	int n = 1;
	float result;

	int L[3][3] = {0, 1, 0, 1, -4, 1, 0, 1, 0};

	Mat output = Mat::zeros(row, col, input.type());

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			float sum = 0.0;
			// Use mirroring boundary process
			for (int a = -n; a <= n; a++) {
				for (int b = -n; b <= n; b++) {
					if (i + a > row - 1) {
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
					sum += L[a + n][b + n] * (float)(input.at<G>(tempa, tempb));
				}
			}
			sum = sum * 5;
			if (sum < 0) sum = 0;
			if (sum > 255) sum = 255;
			output.at<G>(i, j) = (G)sum;
		}
	}
	return output;
}