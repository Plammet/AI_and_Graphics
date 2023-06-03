hist_matching.cpp & hist_matching_YUV.cpp
---------------------------------------------------------------------------------------------------------
This is an image tool based on OpenCV that performs histogram matching.
OpenCV library and "hist_func.h" is needed to run this code. 

input : Input image, Reference image
output : Output image, transfer function(txt), histogram of the Input image(txt), histogram of the output image(txt)

1. hist_matching.cpp is for grayscale output
   Input image color does not matter since the code will automatically transfer it into grayscale image. 

2. hist_matching.cpp is for color output
   Input image will be transfered into YUV format instead of RGB
   Performs histogram matching on Y channel
   Output image will be transfered into RGB format.

 If completed successfully, you will be able to see the input & output image in window. 
------------------------------------------------------------------------------------------------------

To use
1. Prepare target image and reference
2. Put target image in line 8, and reference in line 11
3. Run

----------------------------------------------------------------------------------------------------------