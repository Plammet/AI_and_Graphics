#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF);
void hist_matching(Mat &input, Mat &equalized, Mat reference, G *trans_func_matching);

int main() {

	Mat input = imread("input.jpg", CV_LOAD_IMAGE_COLOR);
	Mat input_YUV;

	Mat reference = imread("ref_bright.png", CV_LOAD_IMAGE_COLOR);
	Mat ref_YUV;

	Mat matched;

	cvtColor(input, input_YUV, CV_RGB2YUV);	// convert RGB to YUV
	cvtColor(reference, ref_YUV, CV_RGB2YUV);

	// split each channel(Y, U, V)
	Mat input_channels[3];
	split(input_YUV, input_channels);
	Mat input_Y = input_channels[0];

	// split each channel(Y, U, V)
	Mat ref_channels[3];
	split(ref_YUV, ref_channels);
	Mat ref_Y = ref_channels[0];

	// PDF or transfer function txt files
	FILE *f_PDF;
	FILE *f_matched_PDF_YUV;
	FILE *f_trans_func_matching_YUV;

	fopen_s(&f_PDF, "PDF.txt", "w+");
	fopen_s(&f_matched_PDF_YUV, "matched_PDF_YUV.txt", "w+");
	fopen_s(&f_trans_func_matching_YUV, "trans_func_matching_YUV.txt", "w+");

	float **PDF = cal_PDF_RGB(input);	
	float *CDF = cal_CDF(input_Y);	

	G trans_func_eq[L] = { 0 };
	G trans_func_matching[L] = { 0 }; //matching function

	hist_eq(input_Y, input_Y, trans_func_eq, CDF);
	hist_matching(input_Y, input_Y, ref_Y, trans_func_matching);

	// merge Y, U, V channels
	merge(input_channels, 3, input_YUV);

	// YUV -> RGB (use "CV_YUV2RGB" flag)
	cvtColor(input_YUV, matched, CV_YUV2RGB);

	float **matched_PDF_YUV = cal_PDF_RGB(matched);	

	for (int i = 0; i < L; i++) {
		// write PDF
		fprintf(f_PDF, "%d\t%f\t%f\t%f\n", i, PDF[i][0], PDF[i][1], PDF[i][2]);
		fprintf(f_matched_PDF_YUV, "%d\t%f\t%f\t%f\n", i, matched_PDF_YUV[i][0], matched_PDF_YUV[i][1], matched_PDF_YUV[i][2]);

		// write transfer functions
		fprintf(f_trans_func_matching_YUV, "%d\t%d\n", i, trans_func_matching[i]);
	}

	// memory release
	free(PDF);
	free(CDF);
	fclose(f_PDF);
	fclose(f_matched_PDF_YUV);
	fclose(f_trans_func_matching_YUV);

	////////////////////// Show each image ///////////////////////

	namedWindow("Input", WINDOW_AUTOSIZE);
	imshow("Input", input);

	namedWindow("matched", WINDOW_AUTOSIZE);
	imshow("matched", matched);

	//////////////////////////////////////////////////////////////

	waitKey(0);

	return 0;
}

// histogram equalization
void hist_eq(Mat &input, Mat &equalized, G *trans_func, float *CDF) {

	// compute transfer function
	for (int i = 0; i < L; i++)
		trans_func[i] = (G)((L - 1) * CDF[i]);

	// perform the transfer function
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			equalized.at<G>(i, j) = trans_func[input.at<G>(i, j)];
}

void hist_matching(Mat &input, Mat &matched, Mat reference, G *trans_func_matching) {
	Mat ref_eq = reference.clone();
	G trans_func_ref[L] = { 0 }; // transfer function of reference

	float *CDF_ref = cal_CDF(reference);
	hist_eq(reference, ref_eq, trans_func_ref, CDF_ref); // histogram equalization on reference

	int value = trans_func_ref[0];

	for (int i = 0; i < L; i++) {
		if (trans_func_ref[i] > value) {
			for (int j = 0; j < trans_func_ref[i] - value; j++) {
				trans_func_matching[value + j] = i - 1;
			}
			value = trans_func_ref[i];
		}
	}
	for (value; value < L; value++)
		trans_func_matching[value] = trans_func_matching[value - 1];

	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			matched.at<G>(i, j) = trans_func_matching[input.at<G>(i, j)];
}
