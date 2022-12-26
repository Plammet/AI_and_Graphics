* Image Stitching Case 2) Mx = b + RANSAC 은 하지 못했습니다.
_____________________________________________________________________________
This codes are image stitching & Line fitting tools based on OpenCV.
OpenCV library is needed to run this code. 
_____________________________________________________________________________

<SIFT_and_Stitching.cpp>

This code is for SIFT feature matching

1. Put the input images on the line 22, 23
2. You can choose whether to use crosscheck meth0d(line 89) and ratio_thresholding(line 90)
3. You can change RATIO_THR value in line 8 (0~1) (only used when you use ratio_thresholding)
   Default value is 0.2
   Higher value means more accurate matching, and less matched points.
3. You can change distance_THR value in line 9
   Default value is 50
    Lower value means more accurate matching, and less matched points.
_______________________________________________________________________________

<Hough.cpp>

This code is for Hough transform 

1. put the input image on the line 19
2-1. If you want the result of HaughLines, set the value in line 32 to 1 (default)
2-2. If you want the result of HaughLinesP, set the value in line 32 to 0
3. run

________________________________________________________________________________