//Authors:
//Pierre VANDEL
//Adrien MAZEL
//Sans qui rien aurai été possible
//Colin OLIVIER

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <iostream>
using namespace cv;
using namespace std;
void main()
{
	int iLastX = -1;
	int iLastY = -1;
	Mat frame;
	Mat frameInverse;

	VideoCapture cap;
	//Les valeurs par défaut sont pour un style de couleur bleu
	int hmin = 0, smin = 151, vmin = 104;
	int hmax = 108, smax = 255, vmax = 255;
	Mat imgHSV, mask;
	cap.open(0);
	cap.read(frame);
	Mat imgLines = Mat::zeros(frame.size(), CV_8UC3);;
	RNG rng(12345);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Trackbars
	namedWindow("Trackbars", (640, 200));
	createTrackbar("Lower Blue", "Trackbars", &hmin, 179);
	createTrackbar("Lower Green", "Trackbars", &smin, 255);
	createTrackbar("Lower Red", "Trackbars", &vmin, 255);
	createTrackbar("Upper Blue", "Trackbars", &hmax, 179);
	createTrackbar("Upper Green", "Trackbars", &smax, 255);
	createTrackbar("Upper Red", "Trackbars", &vmax, 255);

	while (cap.isOpened() && cap.read(frameInverse)) {
		flip(frameInverse, frame, 1);
		if (waitKey(10) == 27) break;
		cvtColor(frame, imgHSV, COLOR_BGR2HSV);
		Scalar lower(hmin, smin, vmin);
		Scalar upper(hmax, smax, vmax);
		inRange(imgHSV, lower, upper, mask);

		findContours(mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		//Permet de clear le mask
		erode(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(mask, mask, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		//Calcule le moment de thresholded de l'image
		Moments oMoments = moments(mask);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// si darea <= 10000, Je considère qu'il n'y a pas d'objet dans l'image
		if (dArea > 10000)
		{
			//Calcule la position du stylo
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				//Dessine un trait bleu
				line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(255, 0, 0), 10);
			}

			iLastX = posX;
			iLastY = posY;
		}

		imshow("Thresholded Image", mask);

		frame = frame + imgLines;
		imshow("Final", frame); //Affiche l'image final

		waitKey(1);
	}
}


