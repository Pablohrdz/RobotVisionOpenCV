#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

/** Programa que encuentra los keypoints de dos imágenes.
	El método utilizado para la obtención de estos es SURF. */
int main( int argc, char** argv )
{

	//Ajustar el path a las imágenes deseadas
	String im1 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\img1.pgm";
	String im2 = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\img2.pgm";

	//Leer las imágenes y transformarlas a grises.
	Mat img_A = imread( im1, IMREAD_GRAYSCALE );
	Mat img_B = imread( im2, IMREAD_GRAYSCALE );

	//Arrojar error, si hay problema con las imágenes.
	if( !img_A.data || !img_B.data )
	{ std::cout<< " --(!) Error reading images " << std::endl; return -1; }

	//Primero se deben detectar los puntos clave, utilizando el detector de SURF.
	int minHessian = 400;

	Ptr<SURF> detector = SURF::create( minHessian );

	std::vector<KeyPoint> keypoints_A, keypoints_B;

	//Detectar keypoints y guardarlos en los vectores anteriores.
	detector->detect( img_A, keypoints_A );
	detector->detect( img_B, keypoints_B );

	//Matrices que contienen los puntos clave
	Mat img_keypoints_A;
	Mat img_keypoints_B;

	//Dibujar los puntos clave en las imágenes
	drawKeypoints( img_A, keypoints_A, img_keypoints_A, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
	drawKeypoints( img_B, keypoints_B, img_keypoints_B, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

	//Pintar las imágenes con los keypoints dibujados
	imshow("Keypoints A", img_keypoints_A );
	imshow("Keypoints B", img_keypoints_B );

	waitKey(0);

	return 0;
}