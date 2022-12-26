#include "hist_func.h"

void hist_eq(Mat &input, Mat &equalized, G *trans_func,float *CDF);
void hist_matching(Mat &input, Mat &equalized, Mat reference, G *trans_func_matching);

int main() {

Mat input = imread("input.jpg", CV_LOAD_IMAGE_COLOR);
Mat input_gray;

Mat reference = imread("ref_bright.png", CV_LOAD_IMAGE_COLOR);
Mat ref_gray;

cvtColor(input, input_gray, CV_RGB2GRAY);	// convert RGB to Grayscale
cvtColor(reference, ref_gray, CV_RGB2GRAY);

Mat matched = input_gray.clone();

// PDF or transfer function txt files
FILE *f_PDF;
FILE *f_matched_PDF_gray;
FILE *f_trans_func_matching;

fopen_s(&f_PDF, "PDF.txt", "w+");
fopen_s(&f_matched_PDF_gray, "matched_PDF_gray.txt", "w+");
fopen_s(&f_trans_func_matching, "trans_func_matching.txt", "w+");

float *PDF = cal_PDF(input_gray);	// PDF of Input image(Grayscale) : [L]
float *CDF = cal_CDF(input_gray);	// CDF of Input image(Grayscale) : [L]

G trans_func_eq[L] = { 0 };	
G trans_func_matching[L] = { 0 }; //matching function

hist_eq(input_gray, matched, trans_func_eq, CDF); // histogram equalization on grayscale image

hist_matching(input_gray, matched, ref_gray, trans_func_matching);

float *matched_PDF_gray = cal_PDF(matched);							// equalized PDF (grayscale)

for (int i = 0; i < L; i++) {
	// write PDF
	fprintf(f_PDF, "%d\t%f\n", i, PDF[i]);
	fprintf(f_matched_PDF_gray, "%d\t%f\n", i, matched_PDF_gray[i]);

// write transfer functions
fprintf(f_trans_func_matching, "%d\t%d\n", i, trans_func_matching[i]);
}

// memory release
free(PDF);
free(CDF);
fclose(f_PDF);
fclose(f_matched_PDF_gray);
fclose(f_trans_func_matching);

////////////////////// Show each image ///////////////////////

namedWindow("Grayscale", WINDOW_AUTOSIZE);
imshow("Grayscale", input_gray);

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
		trans_func_matching[value] = trans_func_matching[value-1];
	
	for (int i = 0; i < input.rows; i++)
		for (int j = 0; j < input.cols; j++)
			matched.at<G>(i, j) = trans_func_matching[input.at<G>(i, j)];
}