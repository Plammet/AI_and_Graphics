#include <iostream>
#include <opencv2/opencv.hpp>

#define IM_TYPE	CV_8UC3
#define color_type "RGB" // RGB or Grayscale
#define feature_space "intensity_and_position"  //intensity_only or intensity_and_position

using namespace cv;

// Note that this code is for the case when an input data is a color value.
int main() {

	Mat input = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
	Mat output;

	if (!input.data)
	{
		std::cout << "Could not open" << std::endl;
		return -1;
	}

	Mat samples;

	if (!strcmp(color_type, "Grayscale")) {
		cvtColor(input, input, CV_RGB2GRAY);

		if (!strcmp(feature_space, "intensity_only")) {
			samples = Mat(input.rows * input.cols, 1, CV_32F);
			for (int y = 0; y < input.rows; y++)
				for (int x = 0; x < input.cols; x++) 
					samples.at<float>(y + x * input.rows, 0) = input.at<uchar>(y, x);
		}

		else if (!strcmp(feature_space, "intensity_and_position")) {
			samples = Mat(input.rows * input.cols, 3, CV_32F);
			for (int y = 0; y < input.rows; y++) {
				for (int x = 0; x < input.cols; x++) {
					samples.at<float>(y + x * input.rows, 0) = (float)(input.at<uchar>(y, x)) / 255;
					samples.at<float>(y + x * input.rows, 1) = (float)y / input.rows;
					samples.at<float>(y + x * input.rows, 2) = (float)x / input.cols;
				}
			}
		}
	}

	if (!strcmp(color_type, "RGB")) {
		if (!strcmp(feature_space, "intensity_only")) {
			samples = Mat(input.rows * input.cols, 3, CV_32F);
			for (int y = 0; y < input.rows; y++)
				for (int x = 0; x < input.cols; x++)
					for (int z = 0; z < 3; z++)
						samples.at<float>(y + x * input.rows, z) = input.at<Vec3b>(y, x)[z];
		}

		else if (!strcmp(feature_space, "intensity_and_position")) {
			samples = Mat(input.rows * input.cols, 5, CV_32F);
			for (int y = 0; y < input.rows; y++) {
				for (int x = 0; x < input.cols; x++){
					for (int z = 0; z < 3; z++) {
						samples.at<float>(y + x * input.rows, z) = (float)(input.at<Vec3b>(y, x)[z]) / 255;
					}
					samples.at<float>(y + x * input.rows, 3) = (float)y / input.rows;
					samples.at<float>(y + x * input.rows, 4) = (float)x / input.cols;
				}
			}
		}
	}	

	namedWindow("Original", WINDOW_AUTOSIZE);
	imshow("Original", input);
	
	// Clustering is performed for each channel (RGB)
	// Note that the intensity value is not normalized here (0~1). You should normalize both intensity and position when using them simultaneously.
	int clusterCount = 10;
	Mat labels;
	int attempts = 5;
	Mat centers;
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

	Mat new_image(input.size(), input.type());
	for (int y = 0; y < input.rows; y++)
		for (int x = 0; x < input.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x*input.rows, 0);

			//Fill code that finds for each pixel of each channel of the output image the intensity of the cluster center.
			if (!strcmp(color_type, "Grayscale")) {
				if (!strcmp(feature_space, "intensity_only"))
					new_image.at<uchar>(y, x) = centers.at<float>(cluster_idx, 0);

				else if (!strcmp(feature_space, "intensity_and_position"))
					new_image.at<uchar>(y, x) = centers.at<float>(cluster_idx, 0) * 255;
			}

			else if (!strcmp(color_type, "RGB")) {
				if (!strcmp(feature_space, "intensity_only")) {
					new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0);
					new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1);
					new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2);
				}

				else if (!strcmp(feature_space, "intensity_and_position")) {
					new_image.at<Vec3b>(y, x)[0] = centers.at<float>(cluster_idx, 0) * 255;
					new_image.at<Vec3b>(y, x)[1] = centers.at<float>(cluster_idx, 1) * 255;
					new_image.at<Vec3b>(y, x)[2] = centers.at<float>(cluster_idx, 2) * 255;
				}
			}
		}
	imshow("clustered image", new_image);

	waitKey(0);

	return 0;
}

