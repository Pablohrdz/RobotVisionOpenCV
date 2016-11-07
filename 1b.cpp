#include <stdio.h>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;


//Callback function para el evento del mouse
void onMouse(int evt, int x, int y, int flags, void* param);

Mat img_A, img_B;

/** @function main */
int main( int argc, char** argv )
{

        std::vector<cv::Point2f> imA_points;
        std::vector<cv::Point2f> imB_points;

        //Cambiar el path para las imágenes deseadas
        String im1 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\img1.pgm";
        String im2 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\img2.pgm";

        img_A = imread( im1, IMREAD_GRAYSCALE );
        img_B = imread( im2, IMREAD_GRAYSCALE );


        cv::namedWindow("Output WindowA");
        imshow("Output WindowA", img_A );


        //El callback del mouse debe asociarse con la ventana de la imagen A
        cv::setMouseCallback("Output WindowA", onMouse, (void*)&imA_points);

        int x, y;


        cv::namedWindow("Output WindowB");
        imshow("Output WindowB", img_B );


        //Arrojar error si hay problema con las imágenes
        if( !img_A.data || !img_B.data )
        {
            std::cout<< " --(!) Error reading images " << std::endl;
            return -1;
        }

      //SURF para la detección de keypoints.
      int minHessian = 400;

      Ptr<SURF> detector = SURF::create( minHessian );

      std::vector<KeyPoint> keypoints_A, keypoints_B;
      Mat descriptors_A, descriptors_B;

      detector->detectAndCompute( img_A, Mat(), keypoints_A, descriptors_A );
      detector->detectAndCompute( img_B, Mat(), keypoints_B, descriptors_B );

      //Utilizar los vectores descriptores para hacer el Flann Matching
      FlannBasedMatcher matcher;
      std::vector< DMatch > matches;
      matcher.match( descriptors_A, descriptors_B, matches );

      double max_dist = 0; double min_dist = 100;

      //Cálculo de max y min de los keypoints
      for( int i = 0; i < descriptors_A.rows; i++ )
      {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
      }

      //Filtrar los puntos por los mejores
      std::vector< DMatch > good_matches;

      for( int i = 0; i < descriptors_A.rows; i++ )
      {
        if( matches[i].distance < 3*min_dist )
         {
            good_matches.push_back( matches[i]);
         }
      }

      Mat img_matches;
      drawMatches( img_A, keypoints_A, img_B, keypoints_B,
                   good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                   std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

      std::vector<Point2f> A;
      std::vector<Point2f> B;

      for( size_t i = 0; i < good_matches.size(); i++ )
      {
        //Extraer los keypoints de los resultados filtrados
        A.push_back( keypoints_A[ good_matches[i].queryIdx ].pt );
        B.push_back( keypoints_B[ good_matches[i].trainIdx ].pt );
      }

      //Encontrar la matriz de la homografía y guardarla en H.
      Mat H = findHomography( A, B, RANSAC );


      //El usuario da click en la imagen A, luego presiona una tecla y el mismo punto se dibuja en la imagen B.
      while(waitKey(0))
      {

          //Aplicar la transformación con la matriz homográfica encontrada.
          perspectiveTransform( imA_points, imB_points, H);

          //Mostrar los puntos de la imagen A
          cout << "imA_points: " << endl;

          for(int i = 0; i < imA_points.size(); i++)
          {
            cout << imA_points[i] << endl;
          }


          //Mostrar los puntos correspondientes en la imagen B
          cout << "imB_points: " << endl;

          //Dibujar los puntos en la imagen B
          for(int i = 0; i < imB_points.size(); i++)
          {
            cout << imB_points[i] << endl;

            circle(img_B,
               Point(imB_points[i].x, imB_points[i].y),
               3,
               Scalar( 255, 0, 255 ),
               CV_FILLED,
               8, 0 );
          }

          //Mostrar la imagen B con los puntos
          imshow("Output Window2", img_B );
      }

      return 0;

}

    //Función que hace que cada vez que el usuario dé click, el punto se guarde en el vector de puntos de la imagen A, y que también se dibuje
    //en ella. Dado que la imagen está en grayscale, el punto se dibuja en negro.
    void onMouse(int evt, int x, int y, int flags, void* param)
    {
        if(evt == CV_EVENT_LBUTTONDOWN)
        {
            std::vector<cv::Point2f>* ptPtr = (std::vector<cv::Point2f>*) param;

            ptPtr->push_back(cv::Point(x,y));

            cout << "Coordenadas X y Y: "<< endl;
            cout << x << '\t' << y << endl;

            circle(img_A,
             Point(x, y),
             3,
             Scalar( 255, 0, 255 ),
             -1,
             8 );

            imshow("Output WindowA", img_A );

        }
    }
