#include <opencv2/opencv.hpp>
#include <stdio.h>

#define IM_TYPE	CV_64FC3
#define Bilateral_amount 20;

using namespace cv;

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


Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);
Mat Bilateralfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt);
Mat Bilateralfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt);

int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_gray;

	// check for validation
	if (!input.data) {
		printf("Could not open\n");
		return -1;
	}

	cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale

												// 8-bit unsigned char -> 64-bit floating point
	input.convertTo(input, CV_64FC3, 1.0 / 255);
	input_gray.convertTo(input_gray, CV_64F, 1.0 / 255);

	// Add noise to original image
	Mat noise_Gray = Add_Gaussian_noise(input_gray, 0, 0.1);
	Mat noise_RGB = Add_Gaussian_noise(input, 0, 0.1);

	// Denoise, using gaussian filter
	Mat Denoised_Gray = Bilateralfilter_Gray(noise_Gray, 3, 3, 3, 5, "adjustkernel");
	Mat Denoised_RGB = Bilateralfilter_RGB(noise_RGB, 3, 3, 3, 5, "adjustkernel");

	namedWindow("Grayscale", WINDOW_AUTOSIZE);
	imshow("Grayscale", input_gray);

	namedWindow("RGB", WINDOW_AUTOSIZE);
	imshow("RGB", input);

	namedWindow("Gaussian Noise (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (Grayscale)", noise_Gray);

	namedWindow("Gaussian Noise (RGB)", WINDOW_AUTOSIZE);
	imshow("Gaussian Noise (RGB)", noise_RGB);

	namedWindow("Denoised (Grayscale)", WINDOW_AUTOSIZE);
	imshow("Denoised (Grayscale)", Denoised_Gray);

	namedWindow("Denoised (RGB)", WINDOW_AUTOSIZE);
	imshow("Denoised (RGB)", Denoised_RGB);

	waitKey(0);

	return 0;
}

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma) {

	Mat NoiseArr = Mat::zeros(input.rows, input.cols, input.type());
	RNG rng;
	rng.fill(NoiseArr, RNG::NORMAL, mean, sigma);

	add(input, NoiseArr, NoiseArr);

	return NoiseArr;
}

Mat Bilateralfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char* opt) {

	Mat kernel_s;
	Mat kernel_r;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float kernelvalue_s;
	float kernelvalue_r;

	// Initialiazing Kernel Matrix 
	kernel_s = Mat::zeros(kernel_size, kernel_size, CV_32F);
	kernel_r = Mat::zeros(kernel_size, kernel_size, CV_32F);

	Mat output = Mat::zeros(row, col, input.type());

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-padding")) {
				float sum1 = 0.0;
				float denom = 0.0;
				float I_sub;

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							I_sub = input.at<G>(i, j) - input.at<G>(i + a, j + b);
						}
						else {
							I_sub = input.at<G>(i, j);
						}
						I_sub *= Bilateral_amount;
						kernel_s.at<float>(a + n, b + n) = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
						kernel_r.at<float>(a + n, b + n) = exp(-(pow(I_sub, 2) / (2 * pow(sigma_r, 2))));
					}
				}

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernelvalue_s = kernel_s.at<float>(a + n, b + n);
						kernelvalue_r = kernel_r.at<float>(a + n, b + n);

						denom += kernelvalue_s * kernelvalue_r;

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							sum1 += kernelvalue_s * kernelvalue_r * input.at<G>(i + a, j + b);
						}

					}
				}
				output.at<G>(i, j) = (G)sum1 / denom;
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1 = 0.0;
				float denom = 0.0;
				float I_sub;
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
						I_sub = input.at<G>(i, j) - input.at<G>(tempa, tempb);
						I_sub *= Bilateral_amount;

						kernel_s.at<float>(a + n, b + n) = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
						kernel_r.at<float>(a + n, b + n) = exp(-(pow(I_sub, 2) / (2 * pow(sigma_r, 2))));
					}
				}

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						kernelvalue_s = kernel_s.at<float>(a + n, b + n);
						kernelvalue_r = kernel_r.at<float>(a + n, b + n);

						denom += kernelvalue_s * kernelvalue_r;

						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							sum1 += kernelvalue_s * kernelvalue_r * input.at<G>(i + a, j + b);
						}
					}
				}
				output.at<G>(i, j) = (G)sum1 / denom;
			}

			else if (!strcmp(opt, "adjustkernel")) {
				float sum1 = 0.0;
				float denom = 0.0;
				float I_sub;

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							I_sub = input.at<G>(i, j) - input.at<G>(i + a, j + b);
							I_sub *= Bilateral_amount;
							kernel_s.at<float>(a + n, b + n) = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
							kernel_r.at<float>(a + n, b + n) = exp(-(pow(I_sub, 2) / (2 * pow(sigma_r, 2))));
						}
					}
				}

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							kernelvalue_s = kernel_s.at<float>(a + n, b + n);
							kernelvalue_r = kernel_r.at<float>(a + n, b + n);

							denom += kernelvalue_s * kernelvalue_r;

							sum1 += kernelvalue_s * kernelvalue_r * input.at<G>(i + a, j + b);
						}

					}
				}
				output.at<G>(i, j) = (G)sum1 / denom;
			}
		}
	}
	return output;
}

Mat Bilateralfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char* opt) {

	Mat kernel_s;
	Mat kernel_r;

	int row = input.rows;
	int col = input.cols;
	int kernel_size = (2 * n + 1);
	int tempa;
	int tempb;
	float kernelvalue_s;
	float kernelvalue_r;

	// Initialiazing Kernel Matrix 
	kernel_s = Mat::zeros(kernel_size, kernel_size, CV_32F);
	kernel_r = Mat::zeros(kernel_size, kernel_size, CV_32FC3);

	Mat output = Mat::zeros(row, col, input.type());

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (!strcmp(opt, "zero-padding")) {
				float sum1[3] = { 0, };
				float denom[3] = { 0, };
				float rgb_sub[3] = { 0, };

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							rgb_sub[0] = input.at<C>(i, j)[0] - input.at<C>(i + a, j + b)[0];
							rgb_sub[1] = input.at<C>(i, j)[1] - input.at<C>(i + a, j + b)[1];
							rgb_sub[2] = input.at<C>(i, j)[2] - input.at<C>(i + a, j + b)[2];
						}
						else {
							rgb_sub[0] = input.at<C>(i, j)[0];
							rgb_sub[1] = input.at<C>(i, j)[1];
							rgb_sub[2] = input.at<C>(i, j)[2];
						}
						rgb_sub[0] *= Bilateral_amount;
						rgb_sub[1] *= Bilateral_amount;
						rgb_sub[2] += Bilateral_amount;

						kernel_s.at<float>(a + n, b + n) = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
						kernel_r.at<Vec3f>(a + n, b + n)[0] = exp(-(pow(rgb_sub[0], 2) / (2 * pow(sigma_r, 2))));
						kernel_r.at<Vec3f>(a + n, b + n)[1] = exp(-(pow(rgb_sub[1], 2) / (2 * pow(sigma_r, 2))));
						kernel_r.at<Vec3f>(a + n, b + n)[2] = exp(-(pow(rgb_sub[2], 2) / (2 * pow(sigma_r, 2))));
					}
				}

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						for (int k = 0; k < 3; k++) {
							kernelvalue_s = kernel_s.at<float>(a + n, b + n);
							kernelvalue_r = kernel_r.at<Vec3f>(a + n, b + n)[k];

							denom[k] += kernelvalue_s * kernelvalue_r;

							if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
								sum1[k] += kernelvalue_s * kernelvalue_r * input.at<C>(i + a, j + b)[k];
							}
						}
					}
				}
				output.at<C>(i, j)[0] = sum1[0] / denom[0];
				output.at<C>(i, j)[1] = sum1[1] / denom[1];
				output.at<C>(i, j)[2] = sum1[2] / denom[2];
			}

			else if (!strcmp(opt, "mirroring")) {
				float sum1[3] = { 0, };
				float denom[3] = { 0, };
				float rgb_sub[3] = { 0, };

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
						rgb_sub[0] = (input.at<C>(i, j)[0] - input.at<C>(tempa, tempb)[0]) * Bilateral_amount;
						rgb_sub[1] = (input.at<C>(i, j)[1] - input.at<C>(tempa, tempb)[1]) * Bilateral_amount;
						rgb_sub[2] = (input.at<C>(i, j)[2] - input.at<C>(tempa, tempb)[2]) * Bilateral_amount;

						kernel_s.at<float>(a + n, b + n) = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
						kernel_r.at<Vec3f>(a + n, b + n)[0] = exp(-(pow(rgb_sub[0], 2) / (2 * pow(sigma_r, 2))));
						kernel_r.at<Vec3f>(a + n, b + n)[1] = exp(-(pow(rgb_sub[1], 2) / (2 * pow(sigma_r, 2))));
						kernel_r.at<Vec3f>(a + n, b + n)[2] = exp(-(pow(rgb_sub[2], 2) / (2 * pow(sigma_r, 2))));
					}
				}

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						for (int k = 0; k < 3; k++) {
							kernelvalue_s = kernel_s.at<float>(a + n, b + n);
							kernelvalue_r = kernel_r.at<Vec3f>(a + n, b + n)[k];

							denom[k] += kernelvalue_s * kernelvalue_r;

							if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
								sum1[k] += kernelvalue_s * kernelvalue_r * input.at<C>(i + a, j + b)[k];
							}
						}
					}
				}
				output.at<C>(i, j)[0] = sum1[0] / denom[0];
				output.at<C>(i, j)[1] = sum1[1] / denom[1];
				output.at<C>(i, j)[2] = sum1[2] / denom[2];
			}

			else if (!strcmp(opt, "adjustkernel")) {
				float sum1[3] = { 0, };
				float denom[3] = { 0, };
				float rgb_sub[3] = { 0, };

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) {
							rgb_sub[0] = (input.at<C>(i, j)[0] - input.at<C>(i + a, j + b)[0]) * Bilateral_amount;
							rgb_sub[1] = (input.at<C>(i, j)[1] - input.at<C>(i + a, j + b)[1]) * Bilateral_amount;
							rgb_sub[2] = (input.at<C>(i, j)[2] - input.at<C>(i + a, j + b)[2]) * Bilateral_amount;

							kernel_s.at<float>(a + n, b + n) = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))) - (pow(b, 2) / (2 * pow(sigma_t, 2))));
							kernel_r.at<Vec3f>(a + n, b + n)[0] = exp(-(pow(rgb_sub[0], 2) / (2 * pow(sigma_r, 2))));
							kernel_r.at<Vec3f>(a + n, b + n)[1] = exp(-(pow(rgb_sub[1], 2) / (2 * pow(sigma_r, 2))));
							kernel_r.at<Vec3f>(a + n, b + n)[2] = exp(-(pow(rgb_sub[2], 2) / (2 * pow(sigma_r, 2))));;
						}
					}
				}

				for (int a = -n; a <= n; a++) {
					for (int b = -n; b <= n; b++) {
						if ((i + a <= row - 1) && (i + a >= 0) && (j + b <= col - 1) && (j + b >= 0)) { //if the pixel is not a border pixel
							for (int k = 0; k < 3; k++) {
								kernelvalue_s = kernel_s.at<float>(a + n, b + n);
								kernelvalue_r = kernel_r.at<Vec3f>(a + n, b + n)[k];

								denom[k] += kernelvalue_s * kernelvalue_r;

								sum1[k] += kernelvalue_s * kernelvalue_r * input.at<C>(i + a, j + b)[k];
							}

						}

					}
				}
				output.at<C>(i, j)[0] = sum1[0] / denom[0];
				output.at<C>(i, j)[1] = sum1[1] / denom[1];
				output.at<C>(i, j)[2] = sum1[2] / denom[2];
			}
		}
	}
	return output;
}