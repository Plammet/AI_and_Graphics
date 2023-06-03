#include <opencv2/opencv.hpp>
#include <stdio.h>

#define IM_TYPE	CV_64FC3

using namespace cv;

Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);
Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);
Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);

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
	Mat Denoised_Gray = Gaussianfilter_Gray(noise_Gray, 3, 10, 10, "adjustkernel");
	Mat Denoised_RGB = Gaussianfilter_RGB(noise_RGB, 3, 10, 10, "zero-padding");

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

Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt) {
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
		float value_s = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))));
		kernel_s.at<float>(a + n, 0) = value_s;
		denom_s += value_s;
	}
	for (int b = -n; b <= n; b++) {
		float value_t = exp(-(pow(b, 2) / (2 * pow(sigma_t, 2))));
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

	if (!strcmp(opt, "zero-padding")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int a = -n; a <= n; a++) {
					if ((i + a <= row - 1) && (i + a >= 0))
						sum1 += kernel_s.at<float>(a + n, 0) * (input.at<double>(i + a, j));
				}
				temp.at<double>(i, j) = sum1;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				for (int b = -n; b <= n; b++) {
					if ((j + b <= col - 1) && (j + b >= 0))
						sum1 += kernel_t.at<float>(0, b + n) * (float)(temp.at<double>(i, j + b));
				}
				output.at<double>(i, j) = sum1;
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
					sum1 += kernel_s.at<float>(a + n, 0) * (float)(input.at<double>(tempa, j));
				}
				temp.at<double>(i, j) = sum1;
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
					sum1 += kernel_t.at<float>(0, b + n) * (float)(temp.at<double>(i, tempb));
				}
				output.at<double>(i, j) = sum1;
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
						sum1 += kernel_s.at<float>(a + n, 0) * (float)(input.at<double>(i + a, j));
						sum2 += kernel_s.at<float>(a + n, 0);
					}
				}
				temp.at<double>(i, j) = sum1 / sum2;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum1 = 0.0;
				float sum2 = 0.0;
				for (int b = -n; b <= n; b++) {
					if ((j + b <= row - 1) && (j + b >= 0)) {
						sum1 += kernel_t.at<float>(0, b + n) * (float)(temp.at<double>(i, j + b));
						sum2 += kernel_t.at<float>(0, b + n);
					}
				}
				output.at<double>(i, j) = sum1 / sum2;
			}
		}
	}
	return output;
}

Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt) {
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
		float value_s = exp(-(pow(a, 2) / (2 * pow(sigma_s, 2))));
		kernel_s.at<float>(a + n, 0) = value_s;
		denom_s += value_s;
	}
	for (int b = -n; b <= n; b++) {
		float value_t = exp(-(pow(b, 2) / (2 * pow(sigma_t, 2))));
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

	if (!strcmp(opt, "zero-padding")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum_r = 0.0;
				float sum_g = 0.0;
				float sum_b = 0.0;
				for (int a = -n; a <= n; a++) {
					if ((i + a <= row - 1) && (i + a >= 0)) {
						sum_r += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(i + a, j)[0]);
						sum_g += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(i + a, j)[1]);
						sum_b += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(i + a, j)[2]);
					}
				}
				temp.at<Vec3d>(i, j)[0] = sum_r;
				temp.at<Vec3d>(i, j)[1] = sum_g;
				temp.at<Vec3d>(i, j)[2] = sum_b;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum_r = 0.0;
				float sum_g = 0.0;
				float sum_b = 0.0;
				for (int b = -n; b <= n; b++) {
					if ((j + b <= col - 1) && (j + b >= 0)) {
						sum_r += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, j + b)[0]);
						sum_g += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, j + b)[1]);
						sum_b += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, j + b)[2]);
					}
				}
				output.at<Vec3d>(i, j)[0] = sum_r;
				output.at<Vec3d>(i, j)[1] = sum_g;
				output.at<Vec3d>(i, j)[2] = sum_b;
			}
		}
	}

	else if (!strcmp(opt, "mirroring")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum_r = 0.0;
				float sum_g = 0.0;
				float sum_b = 0.0;
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
					sum_r += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(tempa, j)[0]);
					sum_g += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(tempa, j)[1]);
					sum_b += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(tempa, j)[2]);
				}
				temp.at<Vec3d>(i, j)[0] = sum_r;
				temp.at<Vec3d>(i, j)[1] = sum_g;
				temp.at<Vec3d>(i, j)[2] = sum_b;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum_r = 0.0;
				float sum_g = 0.0;
				float sum_b = 0.0;
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
					sum_r += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, tempb)[0]);
					sum_g += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, tempb)[1]);
					sum_b += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, tempb)[2]);

				}
				output.at<Vec3d>(i, j)[0] = sum_r;
				output.at<Vec3d>(i, j)[1] = sum_g;
				output.at<Vec3d>(i, j)[2] = sum_b;
			}
		}
	}

	else if (!strcmp(opt, "adjustkernel")) {
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum_r = 0.0;
				float sum_g = 0.0;
				float sum_b = 0.0;
				float sum2 = 0.0;
				for (int a = -n; a <= n; a++) {
					if ((i + a <= row - 1) && (i + a >= 0)) {
						sum_r += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(i + a, j)[0]);
						sum_g += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(i + a, j)[1]);
						sum_b += kernel_s.at<float>(a + n, 0) * (float)(input.at<Vec3d>(i + a, j)[2]);
						sum2 += kernel_s.at<float>(a + n, 0);
					}
				}
				temp.at<Vec3d>(i, j)[0] = sum_r / sum2;
				temp.at<Vec3d>(i, j)[1] = sum_g / sum2;
				temp.at<Vec3d>(i, j)[2] = sum_b / sum2;
			}
		}
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				float sum_r = 0.0;
				float sum_g = 0.0;
				float sum_b = 0.0;
				float sum2 = 0.0;
				for (int b = -n; b <= n; b++) {
					if ((j + b <= row - 1) && (j + b >= 0)) {
						sum_r += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, j + b)[0]);
						sum_g += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, j + b)[1]);
						sum_b += kernel_t.at<float>(0, b + n) * (float)(temp.at<Vec3d>(i, j + b)[2]);
						sum2 += kernel_t.at<float>(0, b + n);
					}
				}
				output.at<Vec3d>(i, j)[0] = sum_r / sum2;
				output.at<Vec3d>(i, j)[1] = sum_g / sum2;
				output.at<Vec3d>(i, j)[2] = sum_b / sum2;
			}
		}
	}
	return output;
}
