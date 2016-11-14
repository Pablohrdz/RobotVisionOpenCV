#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/calib3d.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string>

using namespace cv::xfeatures2d;
using namespace std;
using namespace cv;

void detectKeypointsSURF(Mat& keypoints_img, const Mat& original);
void clusterKMeans(Mat& cluster_img, Mat& original, int k);
void cannyThreshold(int, void*);

int edgeThresh = 1;
int lowThreshold = 2913;
int const max_lowThreshold = 5250;
int ratio = 3;
int kernel_size = 5;
char* window_name = "Edge Map";
Mat src_gray, dst;
Mat detected_edges;

int main(int argc, char** argv)
{
        //Leer imagen original
        String im1 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\plate_1.jpg";
        String im2 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\plate_2.jpg";

        namedWindow( window_name, WINDOW_NORMAL );

        Mat original = imread(im1, CV_LOAD_IMAGE_COLOR);
        //Mat src_gray = imread(im1, IMREAD_GRAYSCALE);
        Mat cluster_img( original.size(), original.type());
        Mat keypoints, edges;


        /// Convert the image to grayscale
        cvtColor( original, src_gray, CV_BGR2GRAY );

        dst.create(src_gray.size(), src_gray.type());

        //clusterKMeans(cluster_img, original, 3);
        //detectKeypointsSURF(keypoints, cluster_img);

        /// Create a Trackbar for user to enter threshold
        createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, cannyThreshold );

        cannyThreshold(0, 0);

        //Desplegar imagen original para comparación
        //imshow("Original", original);


        //imshow("edges", edges);
        //imshow("keypoints", keypoints);
        //imshow( "Clusters", cluster_img );
        waitKey(0);

        return 0;
}

//Taken from http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/canny_detector/canny_detector.html
void cannyThreshold(int, void*)
{

  /// Reduce noise with a kernel 3x3
  //blur( src_gray, detected_edges, Size(3,3) );

  Mat src_gray2;
  src_gray2.create(src_gray.size(), src_gray.type());

  erode(src_gray, src_gray2, getStructuringElement(MORPH_RECT, Size(8, 3)));
  medianBlur(src_gray2, detected_edges, kernel_size);


  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src_gray.copyTo( dst, detected_edges);

  //blur(dst, dst, Size(2, 2));

  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 120, 0, 0 );


  Mat cdst;

  cvtColor(dst, cdst, CV_GRAY2BGR);

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }

  //dilate(dst, dst, getStructuringElement(MORPH_RECT, Size(8, 3)));
  imshow("edges", cdst);
 }



void detectKeypointsSURF(Mat& keypoints_img, const Mat& original)
{
    //Primero se deben detectar los puntos clave, utilizando el detector de SURF.
	int minHessian = 400;

	Ptr<SURF> detector = SURF::create( minHessian );

	std::vector<KeyPoint> keypoints_A;

	//Detectar keypoints y guardarlos en los vectores anteriores.
	detector->detect( original, keypoints_A );

	//Dibujar los puntos clave en las imágenes
	drawKeypoints( original, keypoints_A, keypoints_img, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
}



void clusterKMeans(Mat& cluster_img, Mat& original, int k)
{
    Mat samples(original.rows * original.cols, 3, CV_32F);

          for( int y = 0; y < original.rows; y++ )
          {
              for( int x = 0; x < original.cols; x++ )
              {
                  for( int z = 0; z < 3; z++)
                  {
                      samples.at<float>(y + (x * original.rows), z) = original.at<Vec3b> (y, x)[z];
                  }
              }
          }

          //Aplica k-means con k clusters
          //int k = 3;
          Mat labels, centers;

          kmeans(samples, k, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), 10, KMEANS_PP_CENTERS, centers );



          for( int y = 0; y < original.rows; y++ )
          {
            for( int x = 0; x < original.cols; x++ )
            {
              int cluster_idx = labels.at<int>(y + (x * original.rows), 0);

              cluster_img.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
              cluster_img.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
              cluster_img.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
            }
          }
}
