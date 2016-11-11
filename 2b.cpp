#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string>


using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
        //Leer imagen original
        String im1 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\hylian_night.jpg";

        Mat original = imread(im1, CV_LOAD_IMAGE_COLOR);


        //Desplegar imagen original para comparación
        imshow("Original", original);

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
          int k = 4;
          Mat labels, centers;

          kmeans(samples, k, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001), 10, KMEANS_PP_CENTERS, centers );

          Mat cluster_img( original.size(), original.type());

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

          imshow( "Clusters", cluster_img );
          waitKey(0);

        return 0;
}
