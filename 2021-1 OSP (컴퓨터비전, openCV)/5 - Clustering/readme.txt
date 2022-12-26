This codes are image filtering tools based on OpenCV.
OpenCV library is needed to run this code. 
_____________________________________________________________________________

<LoG.cpp>

This code is for applying LoG ( Laplacian of Gaussian) 
LoG_Gray is for grayscale image, and LoG_RGB is for color(RGB) image.

1. put the input image in line 22
2. run

______________________________________________________________________________

<Canny.cpp>

This code is for Canny Edge Detection

1. put the input image in line 13
2. run
_______________________________________________________________________________

<Harris_corner.cpp>

This code is for Harris Corner Detection

1. put the input image in line 25

2. Choose whether to use non-Maximum Suppression or not. (line 44)
   If you do not change anything, it is true in default.
   true -> use       false -> do not use
  
3. Choose whether to use subpixel accuracy or not. (line 44)
   If you do not change anything, it is true in default.
   true -> use       false -> do not use

4. run

______________________________________________________________________________

  