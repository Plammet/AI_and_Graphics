This codes are image filtering tools based on OpenCV.
OpenCV library is needed to run this code. 
_____________________________________________________________________________

<SIFT.cpp>

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
