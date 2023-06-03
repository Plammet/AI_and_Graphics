This codes are image filtering tools based on OpenCV.
OpenCV library and "hist_func.h" is needed to run this code. 
__________________________________________________________________________________________

<MeanFilterGray & MeanFilterRGB>
This code appllies uniform mean filter on image. 
MeanFilterGray is for grayscaled output, and MeanFilterRGB is for colored output.

●Mat meanfilter(const Mat input, int n, const char* opt)

Mat input : input image that you want to apply filter on it. 
          You can easily read image and make it into Mat format by using imread function on line 32

int n : size of filter(n>1)
         Since this represents the distance from the target pixel, so actual kernel size will be (2n+1)(2n+1) 
         If you don't change anything, it is 3 in default. 

char* opt : There are 3 boundary processes that you can choose.
	  ①zero-paddle     ②mirroring     ③adjustkernel

__________________________________________________________________________________________

<GaussianGray & GaussianRGB> 
This code applies Gaussian filter on image.
GaussianGray is for grayscaled output, and GaussianRGB is for colored output.

●Mat gaussianfilter(const Mat input, int n, float sigmaT, float sigmaS, const char* opt)

Mat input : input image that you want to apply filter on it. 
          You can easily read image and make it into Mat format by using imread function on line 32

int n : size of filter(n>1)
         Since this represents the distance from the target pixel, so actual kernel size will be (2n+1)(2n+1) 
         If you don't change anything, it is 3 in default. 
 
float sigmaT, : standard deviation of the filtering function.
float sigmaS   (sigmaS -> vertical, sigma T -> horizontal)
	        bigger value makes image more blurry.
	       If you don't change anything, they are both 3 in default. 

char* opt : There are 3 boundary processes that you can choose.
	  ①zero-paddle     ②mirroring     ③adjustkernel

________________________________________________________________________________________

<GaussianGray_sep & GaussianGrayRGB_sep>
Faster version of gaussian filtering.
It apply gaussian filter seperatly.
Everything else is same as GaussianGray & GaussianGrayRGB

________________________________________________________________________________________

<SobelGray & SobelRGB>
This code applies Sobel filter on image.
SobelGray is for grayscaled output, and SobelRGB is for colored output.

●Mat sobelfilter(const Mat input)

Mat input : input image that you want to apply filter on it. 
               You can easily read image and make it into Mat format by using imread function on line 32

_______________________________________________________________________________________

<LaplacianGray & LaplacianRGB>
This code applies Laplacian filter on image.
LaplacianGray is for grayscaled output, and LaplacianRGB is for colored output.

●Mat Laplacianfilter(const Mat input)

Mat input : input image that you want to apply filter on it. 
               You can easily read image and make it into Mat format by using imread function on line 32

_________________________________________________________________________________________

<unsharpGray & unsharpRGB>
This code applies unsharp filter on image. 
Since this function uses Gaussian filter to generate low-pass filtered image, 
all parameter of Gaussian filter is needed. 

unsharpGray is for grayscaled output, and unsharpRGB is for colored output.

●Mat unsharpMask(const Mat input, int n, float sigmaT, float sigmaS, const char* opt, float k);

Mat input : input image that you want to apply filter on it. 
                You can easily read image and make it into Mat format by using imread function on line 32

****(these are for gaussian filter)************************
	int n : size of filter(n>1)
         	         Since this represents the distance from the target pixel, so actual kernel size will be (2n+1)(2n+1) 
         	         If you don't change anything, it is 3 in default. 
 
	float sigmaT, : standard deviation of the filtering function.
	float sigmaS   (sigmaS -> vertical, sigma T -> horizontal)
		       bigger value makes image more blurry.
	       	       If you don't change anything, they are both 3 in default. 

	char* opt : There are 3 boundary processes that you can choose.
		  ①zero-paddle     ②mirroring     ③adjustkernel
**********************************************************

float k : scaling constant. (0<k<1) 
           bigger value makes sharper image.
           value between 0.1 and 0.5 is recommended.
           If you don't change anything, it is 0.4 in default.

______________________________________________________________________________________