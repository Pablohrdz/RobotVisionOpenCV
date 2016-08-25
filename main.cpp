#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

Point stringToPoint(string coordinateString);
void drawRectOverImage(Mat3b& img, const Point& uL, const Point& lR, int r, int g, int b);

int main(int argv, char** argc)
{
    //cout << argv << endl;

    if(argv != 4)    return -1;

    string upperLeft = argc[1];
    string lowerRight = argc[2];
    string imgName = argc[3];

    cout << upperLeft << endl;
    cout << lowerRight << endl;
    cout << imgName << endl;

    Point uL = stringToPoint(upperLeft);
    Point lR = stringToPoint(lowerRight);

    cout << uL << endl;
    cout << lR << endl;

    Rect rect(uL, lR);

    Mat3b img = imread(imgName);
    Mat3b cropped_img, img_rect;

    drawRectOverImage(img, uL, lR, 255, 0, 0);

    //rectangle(img, uL, lR, Scalar(255, 0, 0), -1);

    img(rect).copyTo(cropped_img);
    //Mat3b img = imread("C:\\Users\\Pablo\\Pictures\\cat7.jpg");
    imshow("img", img);
    imshow("cropped_img", cropped_img);
    //imshow("rect_img", img_rect);
    waitKey();
    return 0;
}

void drawRectOverImage(Mat3b& img, const Point& uL, const Point& lR, int r, int g, int b)
{
    for(int i = uL.y; i < lR.y; i++)
    {
        for(int j = uL.x; j < lR.x; j++)
        {
            img(i, j)[0] = r;
            img(i, j)[1] = g;
            img(i, j)[2] = b;
        }
    }
}

Point stringToPoint(string coordinateString)
{
    assert(coordinateString.size() > 0);

    size_t comma = coordinateString.find(",");
    size_t closing_bracket = coordinateString.find(")");

    string str_x = "", str_y = "";

    str_x = coordinateString.substr(1, comma - 1);
    str_y = coordinateString.substr(comma + 1, closing_bracket - 1);

    cout << str_x << endl;
    cout << str_y << endl;

    stringstream convertX(str_x);
    stringstream convertY(str_y);

    int x, y;

    convertX >> x;
    convertY >> y;

    Point p(x, y);

    return p;
}
