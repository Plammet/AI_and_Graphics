This codes are image filtering tools based on OpenCV.
OpenCV library and "hist_func.h" is needed to run this code. 
__________________________________________________________________________________________

<salt_and_pepper.cpp>
This code consist of two parts.
1. add salt_and_pepper noise to the image
2. remove salt_and_pepper noise from the image (using uniform mean filtering)

●Mat Add_salt_pepper_Noise(const Mat input, float ps, float pp);

Mat input : input image that you want to apply filter on it. 
                You can easily read image and make it into Mat format by using imread function on line 33

float ps : density of salt(white) noise (0~1)
float pp : density of pepper(black) noise (0~1)
             If you don't change anything, it is both 0.1 in default.
             you can change this in line 45(gray)~46(RGB) 

● Mat Salt_pepper_noise_removal_Gray(const Mat input, int n, const char *opt);
    Mat Salt_pepper_noise_removal_RGB(const Mat input, int n, const char *opt);

Mat input : input image that you want to apply filter on it. 
                You can easily read image and make it into Mat format by using imread function on line 33

int n : size of filter(n>1)
         Since this represents the distance from the target pixel, so actual kernel size will be (2n+1)(2n+1) 
         If you don't change anything, it is 3 in default. 

char* opt : There are 3 boundary processes that you can choose.
	  ①zero-padding     ②mirroring     ③adjustkernel

after run this code, you can see 6 windows. 
(original, grayscale, noised(rgb), noised(gray), denoised(RGB), denoised(gray) 
__________________________________________________________________________________________

<Gaussian.cpp> 
This code consist of two parts.
1. add Gaussian noise to the image
2. remove Gaussian noise from the image (using Gaussian filtering)

● Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);

Mat input : input image that you want to apply filter on it. 
          You can easily read image and make it into Mat format by using imread function on line 14

double mean : mean of gaussian filtering. If you don't change anything, it is 0 in default.(recommended)

double sigma : sigma of gaussian filtering. If you don't change anything, it is 0.1 in default.
 
● Mat Gaussianfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);
    Mat Gaussianfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, const char *opt);
 
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
   
after run this code, you can see 6 windows. 
(original, grayscale, noised(rgb), noised(gray), denoised(RGB), denoised(gray) 
________________________________________________________________________________________

<bilateral.cpp>
This code consist of two parts.
1. add Gaussian noise to the image
2. remove Gaussian noise from the image (using bilateral filtering)

You can change Bilateral_amount defined in line 5. (default is 20)
bigger number -> influence of the distance is reduced, influence of the color increases.
	           output will be less blurry, but more noise remain.  
smaller number ->influence of the distance increases, influence of the color is reduced
                       noise will be removed, but image will be more blurry. 

● Mat Add_Gaussian_noise(const Mat input, double mean, double sigma);

Mat input : input image that you want to apply filter on it. 
               You can easily read image and make it into Mat format by using imread function on line 33

double mean : mean of gaussian filtering. If you don't change anything, it is 0 in default.(recommended)

double sigma : sigma of gaussian filtering. If you don't change anything, it is 0.1 in default.

● Mat Bilateralfilter_Gray(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt);
    Mat Bilateralfilter_RGB(const Mat input, int n, double sigma_t, double sigma_s, double sigma_r, const char *opt);

Mat input : Mat input : input image that you want to apply filter on it. 

int n : size of filter(n>1)
         Since this represents the distance from the target pixel, so actual kernel size will be (2n+1)(2n+1) 
         If you don't change anything, it is 3 in default. 

double sigma_t : sigma for spatial-based filtering
double sigma_s 

double sigma_r : sigma for intensity-based filtering

char* opt : There are 3 boundary processes that you can choose.
	  ①zero-paddle     ②mirroring     ③adjustkernel