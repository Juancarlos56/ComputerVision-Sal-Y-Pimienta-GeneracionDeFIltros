

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <cmath>
#include <random>

#include <time.h>
// Librerías de OpenCV

//#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp> // Contiene los elementos base de OpenCV (matrices, arreglos, etc.)
#include <opencv2/highgui/highgui.hpp> // Contiene estructuras para crear la interfaz gráfica
#include <opencv2/imgproc/imgproc.hpp> // Procesamiento de imágenes
#include <opencv2/imgcodecs/imgcodecs.hpp> // Códecs para leer determinados formatos de imágenes
#include <opencv2/video/video.hpp> // Procesamiento y escritura
#include <opencv2/videoio/videoio.hpp> // de video

using namespace std;

using namespace cv; // El espacio de nombres donde están definidas algunas funciones de OpenCV

Mat frameSalGauss = Mat::eye(5, 5, CV_32F);
Mat framePimientaGauss = Mat::eye(5, 5, CV_32F);
Mat frameSalMedian = Mat::eye(5, 5, CV_32F);
Mat framePimientaMedian = Mat::eye(5, 5, CV_32F);
Mat frameSalBlur = Mat::eye(5, 5, CV_32F);
Mat framePimientaBlur = Mat::eye(5, 5, CV_32F);

Mat frameVideo;
Mat frameVideoMedian;

Mat frame;
Mat imgLaplace;
Mat laplaceEscala;
Mat canny;


Mat frameOriginal;
Mat frameSal;
Mat framePimienta;

int porcentajeSal = 0;
double porcentajeSalDecimal = 0.0;
int porcentajePimienta = 0;
double porcentajePimientaDecimal = 0.0;

int valorMascara = 3;
int opcion = 2;
int opcionSuavizado = 0;
int sigma = 1;
double sigmaDecimal = 0.1;


int thres = 150;
int ratio = 3;


Mat generarRuidoSal(Mat video, double porcentajeSalDecimal = 0.20){
    Mat videoRuido = video.clone();
    int total = (int) (porcentajeSalDecimal*((double)(video.rows))*((double)video.cols));
    int contador = 0;
    int fila = 0;
    int columna = 0;

    while(contador<total){
        fila = rand() % video.rows;
        columna = rand() % video.cols;
        videoRuido.at<uchar>(fila, columna) = 255;
        contador++;
    }
    return videoRuido;
}

Mat generarRuidoPimienta(Mat video, double porcentajePimientaDecimal = 0.20){
    Mat videoRuido = video.clone();
    int total = (int) (porcentajePimientaDecimal*((double)(video.rows))*((double)video.cols));
    int contador = 0;
    int fila = 0;
    int columna = 0;

    while(contador<total){
        fila = rand() % video.rows;
        columna = rand() % video.cols;
        videoRuido.at<uchar>(fila, columna) = 0;
        contador++;
    }
    return videoRuido;
}

void funcionTrackbarSal(int v, void *data){
    porcentajeSalDecimal = (double)(((double)porcentajeSal)/100);
}

void funcionTrackbarPimienta(int v, void *data){ 
    porcentajePimientaDecimal = (double)(((double)porcentajePimienta)/100);
}

void funcionTrackbarGauss(int v, void *data){
    sigmaDecimal = ((double) sigma)/10.0;
}

Mat filtoGaussBlur(Mat image, int valorMascara, double sigmaDecimal){
    Mat desImage;
    if(valorMascara%2!=0 && valorMascara>0){
        GaussianBlur(image,desImage,Size(valorMascara, valorMascara), sigmaDecimal, sigmaDecimal);
    }
    return desImage;
}

Mat filtoMedianBlur(Mat image, int valorMascara){
    Mat desImage;
    if(valorMascara%2!=0 && valorMascara>0){
        medianBlur(image, desImage, valorMascara);
    }
    return desImage;
}

Mat filtoBlur(Mat image, int valorMascara){
    Mat desImage;
    if(valorMascara%2!=0 && valorMascara>0){
        blur(image, desImage, Size(valorMascara, valorMascara), Point(-1,-1));
    }
    return desImage;
}

int main(int argc, char *argv[]){
    // Carga y reproducción de vídeos
    // VideoCapture video(0);
    // VideoCapture video("/dev/video0");

    //Carga de video para ser procesado, se almacena en video. 
    VideoCapture video("./video.mkv");
    
    //Ingreso para ver si existe el video. 
    if(video.isOpened()){
        
        //Creacion de ventanas para visualizacion de video, bordes y filtros. 
        //Ventana de BORDES
        namedWindow("Video agregado filtro de Sal", WINDOW_AUTOSIZE);
        namedWindow("Laplace", WINDOW_AUTOSIZE);
        namedWindow("Canny", WINDOW_AUTOSIZE);
        //Ventana de Suavizados
        namedWindow("Video Original", WINDOW_AUTOSIZE);
        namedWindow("Video Sal", WINDOW_AUTOSIZE);
        namedWindow("Video Pimienta", WINDOW_AUTOSIZE);
        //Ventana de Filtros
        namedWindow("Imagen Sal Median", WINDOW_AUTOSIZE);
        namedWindow("Imagen Pimienta Median", WINDOW_AUTOSIZE);
        namedWindow("Imagen Sal Filtro Gaussiano", WINDOW_AUTOSIZE);
        namedWindow("Imagen Pimienta Filtro Gaussiano", WINDOW_AUTOSIZE);
        namedWindow("Imagen Sal Blur", WINDOW_AUTOSIZE);
        namedWindow("Imagen Pimienta Blur", WINDOW_AUTOSIZE);
        
        //Bucle infinito, teclear esc para salir
        while(3==3){
            
            //paso de video a tipo Mat
            video >> frame;
            if (frame.empty()){
                break;
            }
            //Cambio de color a escala de grises para trabajar con suavizados y bordes
            cvtColor(frame, frameOriginal, COLOR_BGR2GRAY);
            
            //Creacion de Semilla para generacion de numeros aleatorios. 
            srand(time(NULL));
            //Modificacion de tamanio del video al 50%
            cv::resize(frameOriginal, frameOriginal, Size(), 0.5, 0.5);
            
            //Trackbar para colorcar porcentaje de pimienta de 1-100
            createTrackbar("Pimienta Pixel 0","Video Original",&porcentajePimienta, 100, funcionTrackbarPimienta, NULL);
            //Trackbar para colorcar porcentaje de sal 1-100
            createTrackbar("Sal Pixel 255","Video Original",&porcentajeSal, 100, funcionTrackbarSal, NULL);
            //Trackbar para tamanio de mascara del 1-30
            createTrackbar("Macara","Video Original",&valorMascara, 30, NULL);
            //Trackbar para sigma para suavizado Gauss 1-70
            createTrackbar("Sigma para Gauss","Video Original",&sigma, 70, funcionTrackbarGauss, NULL);
            //Trackbar de selecion de metodo de suavizado 1-3
            //1.Gauss para aplicar suavizado en el video
            //2.Median para aplicar suavizado en el video
            //3.Blur para aplicar suavizado en el video
            createTrackbar("Video: 1. Gauss 2. Median 3. Blur ","Video Original",&opcion, 3, NULL);
            //Trackbar de seleccion  
            //1. Aplicar Sal en la imagen
            //2. Aplicar Pimienta en la imagen
            createTrackbar("Video: 1. Sal 2. Pimienta","Video Original",&opcionSuavizado, 2, NULL);

            //Verificacion de valor de mascara debe ser impar y mayor a 0
            if(valorMascara%2!=0 && valorMascara>0){
                //Generacion de imagen con Sal
                frameSal = generarRuidoSal(frameOriginal, porcentajeSalDecimal);
                //Generacion de imagen con Pimienta
                framePimienta = generarRuidoPimienta(frameOriginal, porcentajePimientaDecimal);
                
                //Aplicacion de suavizado de media para imagenes de sal y pimienta
                frameSalMedian = filtoMedianBlur(frameSal, valorMascara);
                framePimientaMedian = filtoMedianBlur(framePimienta, valorMascara);
                //Aplicacion de suavizado de gauss para imagenes de sal y pimienta
                frameSalGauss = filtoGaussBlur(frameSal, valorMascara, sigmaDecimal);
                framePimientaGauss = filtoGaussBlur(framePimienta, valorMascara, sigmaDecimal);
                //Aplicacion de suavizado de blur para imagenes de sal y pimienta
                frameSalBlur = filtoBlur(frameSal, valorMascara);
                framePimientaBlur = filtoBlur(framePimienta, valorMascara);

                //Opcion  
                //1. para video con sal
                //2. para video con pimienta
                if (opcionSuavizado == 1){
                    frameVideo = generarRuidoSal(frameOriginal, porcentajeSalDecimal);
                }else{
                    frameVideo = generarRuidoPimienta(frameOriginal, porcentajePimientaDecimal);
                }
                
                //Opcion de suavizado 
                //1. Gauss ---Laplace - Canny
                //2. Median ---Laplace - Canny
                //3. Blur ---Laplace - Canny
                if (opcion==1){
                    
                    GaussianBlur(frameVideo,frameVideoMedian,Size(valorMascara, valorMascara), sigmaDecimal, sigmaDecimal);
                    Laplacian(frameVideoMedian, imgLaplace, CV_16S, 3);
                    convertScaleAbs(imgLaplace, laplaceEscala);
                    Canny(frameVideoMedian, canny, thres, thres*3, 5);
                }else if (opcion == 2)
                {
                    medianBlur(frameVideo, frameVideoMedian, valorMascara);
                    Laplacian(frameVideoMedian, imgLaplace, CV_16S, 3);
                    convertScaleAbs(imgLaplace, laplaceEscala);
                    Canny(frameVideoMedian, canny, thres, thres*3, 5);

                }else if (opcion == 3)
                {
                    blur(frameVideo, frameVideoMedian, Size(valorMascara, valorMascara));
                    Laplacian(frameVideoMedian, imgLaplace, CV_16S, 3);
                    convertScaleAbs(imgLaplace, laplaceEscala);
                    Canny(frameVideoMedian, canny, thres, thres*3, 5);
                }
                
                //Muestra de video 
                imshow("Video agregado filtro de Sal", frameVideo);
                imshow("Laplace", laplaceEscala);
                imshow("Canny", canny);

                //Muestra de filtros 
                imshow("Video Original", frameOriginal);
                imshow("Video Sal", frameSal);
                imshow("Video Pimienta", framePimienta);
                imshow("Imagen Sal Median", frameSalMedian);
                imshow("Imagen Pimienta Median", framePimientaMedian);
                imshow("Imagen Sal Filtro Gaussiano", frameSalGauss);
                imshow("Imagen Pimienta Filtro Gaussiano", framePimientaGauss);
                imshow("Imagen Sal Blur", frameSalBlur);
                imshow("Imagen Pimienta Blur", framePimientaBlur);

            }

            
            if(waitKey(23)==27){
                break;
            }
        }

        video.release();
        destroyAllWindows();

    }
    return 0;
}





