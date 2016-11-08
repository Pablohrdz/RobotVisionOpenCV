#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <string>

#define PI 3.14159265


using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
        Scalar colorTab[] =
        {
            Scalar(0, 0, 255),
            Scalar(0, 255, 0),
            Scalar(255, 0, 0),
            Scalar(255, 255, 0),
            Scalar(0,255,255)
        };
        Mat img(1000, 800, CV_8UC3);

        int cluster_count = 10;
        int i = 0;


        //Leer el archivo y sacar toda la información
        string line, line1, line2;
        ifstream myfile ("C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\dim2.txt");
        Point center;


        //Leer todas las lineas para saber la extensión del archivo y del arreglo a inicializar
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                i++;
            }

            myfile.close();
        }
        else    cout << "Unable to open file";

        cout << i << endl;

        float data[i][2];

        ifstream file ("C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\ex_2\\dim2.txt");


        int z = 0;

        if (file.is_open())
        {
            while ( getline (file, line) )
            {
                //cout << z << endl;

                istringstream iss(line);
                iss >> line1;
                iss >> line2;

                int x=atoi(line1.c_str());
                int y=atoi(line2.c_str());

                data[z][0] = x / 1200.0f;
                data[z][1] = y / 1500.0f;

                z++;
            }

            file.close();

        }else cout << "Unable to open file";

        Mat centers, points(i, 2, CV_32FC1 , data), labels;


         for( int j = 0; j < i; j++ )
        {
            //int clusterIdx = labels.at<int>(j);
            Point ipt = points.at<Point2f>(j);
            cout << ipt.x << ", " << ipt.y << endl;
            //circle( img, ipt, 2, colorTab[clusterIdx], FILLED, LINE_AA );
        }


        //Aplicar Kmeans
        kmeans(points, cluster_count, labels,
               TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
               3, KMEANS_PP_CENTERS, centers);
        //img = Scalar::all(0);

        //Dibujar los círculos
        for( int j = 0; j < i; j++ )
        {
            int clusterIdx = labels.at<int>(j);
            Point ipt = points.at<Point2f>(j);
            //cout << ipt.x << ", " << ipt.y << endl;
            circle( img, ipt, 2, colorTab[clusterIdx], FILLED, LINE_AA );
        }



        imshow("clusters", img);

        waitKey(0);

        return 0;
}
