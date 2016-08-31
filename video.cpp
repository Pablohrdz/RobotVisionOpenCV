#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

Point stringToPoint(string coordinateString);
void drawRectOverImage(Mat3b& img, const Point& uL, const Point& lR, int r, int g, int b);


int main()
{
    string filename = "C:\\Users\\Pablo\\Pictures\\test.mp4";
    VideoCapture video(filename);

    if(!video.isOpened())       return -1;

    float dataGaussian1[9] = {.0625, .125, .0625, .125, .250, .125, .0625, .125, .0625};
    float dataGaussian2[9] = {0.088874, 0.12037, 0.088874, 0.12037, 0.163027, 0.12037, 0.088874, 0.12037, 0.088874};
    float dataUniform1[9] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
    Mat frame, kernel(3, 3, CV_32F, dataUniform1);


    //int kernel_size = 3;
    //kernel = Mat::ones( kernel_size, kernel_size, CV_32F )/ (float)(kernel_size*kernel_size);

    cout << kernel << endl;

    while(true)
    {
        Mat blurGauss, blurFilter;
        video >> frame;

        GaussianBlur(frame, blurGauss, Size(7, 7), 1.5, 1.5);
        filter2D(frame, blurFilter, -1, kernel, Point(-1, -1), 0, BORDER_DEFAULT);

        imshow("normalFrame", frame);
        imshow("GaussianBlur", blurGauss);
        imshow("filter2Dblur", blurFilter);

        if(waitKey(30) >= 0)    break;
    }

    return 0;

}
