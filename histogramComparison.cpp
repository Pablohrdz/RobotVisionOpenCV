#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>
#include <algorithm>
#include <fstream>

//Pablo Hernández Cepeda
//A01335563


using namespace cv;
using namespace std;

#define COMP_CORREL 0
#define COMP_CHISQR 1
#define COMP_INTERSECT 2
#define COMP_BHATTACHARYYA 3

//Estructura que contiene el nombre de una imagen y los scores de los distintos métodos de comparación de histogramas.
struct Imagen
{
    String nombre;
    double chi_square;
    double bhatt;
    double correlation;
    double intersection;
    double emd;
};

/******* Funciones de comparaciòn para ordenar el vector de imágenes, dependiendo del método que se utilice para calcular el score. *****/
bool compareChi(Imagen& img1, Imagen& img2)
{
    return img1.chi_square < img2.chi_square;
}

bool compareBhatt(Imagen& img1, Imagen& img2)
{
    return img1.bhatt < img2.bhatt;
}

bool compareCorrelation(Imagen& img1, Imagen& img2)
{
    return img1.correlation > img2.correlation;
}

bool compareIntersection(Imagen& img1, Imagen& img2)
{
    return img1.intersection > img2.intersection;
}

bool compareEMD(Imagen& img1, Imagen& img2)
{
    return img1.emd < img2.emd;
}

//Prototipos
void sortAndWriteToFile(ofstream &file, vector<Imagen> &imagenes);
void prepareEMD(Mat &sig, MatND &hist_img, int h_bins, int s_bins);

int main(int argv, char** argc)
{
    //Argument check
    if(argv != 2)
    {
        cerr << "Only one input argument required" << endl;
        return -1;
    }

    //Image variables
    vector<String> filenames;
    Mat imagen_principal_src  = imread(argc[1]);
    Mat imagen_principal_hsv;
    Mat imagen_principal_emd_hist;

    //Histogram variables
    vector<MatND> histogramas;
    MatND hist_principal;

    //Histogram parameters
    int h_bins = 5;
    int s_bins = 6;
    int histSize[] ={h_bins, s_bins};
    float h_ranges[] = {0, 180};
    float s_ranges[] = {0, 256};

    const float* ranges[] = {h_ranges, s_ranges};

    int channels[] = {0, 1};

    //Directorio de la base de datos de imágenes
    String dir = "C:\\Users\\Pablo\\Documents\\TEC\\VisionRobots\\DataBase_1\\DataBase_1";
    String imagen = argc[1];

    //Vectores con los scores de los 4 distintos métodos
    vector<double> correlation, chi_square, intersection, bhatta;

    vector<Imagen> imagenes;

    //Leer todo del directorio dado
    glob(dir, filenames);

    //Calcular histograma de la imagen principal
    cvtColor(imagen_principal_src, imagen_principal_hsv, CV_BGR2HSV);
    calcHist(&imagen_principal_hsv, 1, channels, Mat(), hist_principal, 2, histSize, ranges, true, false);
    normalize(hist_principal, hist_principal, 0, 1, NORM_MINMAX, -1, Mat());

    //Preparar
    prepareEMD(imagen_principal_emd_hist, hist_principal, h_bins, s_bins);

    cout << "Calculando..." << endl;

    for(int i = 0; i < filenames.size(); i++)
    {
        Imagen imagen;
        Mat img_src = imread(filenames[i]);
        Mat img_hsv;
        Mat hist_emd_mat;

        MatND hist_img_db;

        cvtColor(img_src, img_hsv, CV_BGR2HSV);

        if(!img_src.data)
        {
            cerr << "Problem loading image from given folder" << endl;
            return -1;
        }

        //Calcular el histograma de la imagen actual de la base de datos y normalizarlo.
        calcHist(&img_hsv, 1, channels, Mat(), hist_img_db, 2, histSize, ranges, true, false);
        normalize(hist_img_db, hist_img_db, 0, 1, NORM_MINMAX, -1, Mat());

        //Guardar el nombre de la imagen
        //Calcular los scores de la imagen y compararlos con la imagen actual de la base de datos.
        //Guardar el resultado en el struct de la imagen actual
        imagen.nombre = filenames[i];

        imagen.correlation = compareHist(hist_principal, hist_img_db, COMP_CORREL);
        imagen.chi_square = compareHist(hist_principal, hist_img_db, COMP_CHISQR);
        imagen.bhatt = compareHist(hist_principal, hist_img_db, COMP_BHATTACHARYYA);
        imagen.intersection = compareHist(hist_principal, hist_img_db, COMP_INTERSECT);

        //Normalizar el histograma actual y preparar la matriz para el EMD.
        normalize(hist_img_db, hist_img_db, 0, 1, NORM_MINMAX, -1, Mat());
        prepareEMD(hist_emd_mat, hist_img_db, h_bins, s_bins);

        //Calcular EMD utilizando el histograma de la imagen principal y la imagen actual
        //El EMD debería ser 0 para un match exacto.
        //Si una imagen se compara con otra parecida pero con distinta iluminación, el algoritmo de EMD
        //tendrá un mejor score que los otros cuatro métodos, dado que la forma del histograma no cambiará, sino que ocurrirá un
        //desplazamiento de ésta. Como el EMD calcula la distancia del desplazamiento, es una mejor opción para analizar imágenes de
        //este tipo.
        imagen.emd = EMD(imagen_principal_emd_hist, hist_emd_mat, DIST_L2);

        cout << "Imagen #" << i << " procesada. Cálculo exitoso de sus scores." << endl;

        imagenes.push_back(imagen);

    }

    //Archivo en el que se guardaràn los resultados. Modificar el string del constructor para escribir el archivo en otro directorio.
    ofstream myFile("resultados_scores.txt");

    //Ordenar por mètodo de correlación y escribir el resultado en el archivo.
    sortAndWriteToFile(myFile, imagenes);

    return 0;

}

//Funciòn que prepara la matriz necesaria para hacer el cálculo del EMD.
void prepareEMD(Mat &sig, MatND &hist_img, int h_bins, int s_bins)
{
    vector<Vec3f> sigv;

    for(int h = 0; h < h_bins; h++)
    {
            for(int s = 0; s < s_bins; s++)
            {
                float bin_val = hist_img.at<float>(h, s);

                if(bin_val != 0)
                {
                    sigv.push_back(Vec3f(bin_val, (float) h, (float) s));
                }
            }
    }

    sig = Mat(sigv).clone().reshape(1);
}


//Método que ordena el vector de imagenes dependiendo del método de comparación, y que escribe el resultado al archivo final.
void sortAndWriteToFile(ofstream &file, vector<Imagen> &imagenes)
{
    if(file.is_open())
    {
        //Ordenar los scores de cada método, usando las funciones de comparación para cada uno.
        std::sort(imagenes.begin(), imagenes.end(), compareChi);

        file << "*********************** Chi Square Scores ***********************" << endl;
        for(int i = 0; i < imagenes.size(); i++)
        {
            file << imagenes[i].nombre << " -> " << imagenes[i].chi_square << endl;
        }

        std::sort(imagenes.begin(), imagenes.end(), compareBhatt);

        file << endl;
        file << "*********************** Bhattacharyya Scores ***********************" << endl;
        for(int i = 0; i < imagenes.size(); i++)
        {
            file << imagenes[i].nombre << " -> " << imagenes[i].bhatt << endl;
        }

        std::sort(imagenes.begin(), imagenes.end(), compareCorrelation);

        file << endl;
        file << "*********************** Correlation Scores ***********************" << endl;
        for(int i = 0; i < imagenes.size(); i++)
        {
            file << imagenes[i].nombre << " -> " << imagenes[i].correlation << endl;
        }

        std::sort(imagenes.begin(), imagenes.end(), compareIntersection);

        file << endl;
        file << "*********************** Intersection Scores ***********************" << endl;
        for(int i = 0; i < imagenes.size(); i++)
        {
            file << imagenes[i].nombre << " -> " << imagenes[i].intersection << endl;
        }

        std::sort(imagenes.begin(), imagenes.end(), compareEMD);

        file << endl;
        file << "*********************** EMD Scores ***********************" << endl;
        for(int i = 0; i < imagenes.size(); i++)
        {
            file << imagenes[i].nombre << " -> " << imagenes[i].emd << endl;
        }

        file.close();
    }

    else
    {
        cout << "No se pudo abrir el archivo" << endl;
    }

}

