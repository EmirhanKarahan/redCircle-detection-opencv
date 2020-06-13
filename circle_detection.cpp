#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace cv;

cv::VideoCapture cap(0);
int radius_trackbar;
int points_trackbar;

int main() {
	namedWindow("Trackbar Penceresi", CV_WINDOW_AUTOSIZE);
	createTrackbar("Daire �ap�", "Trackbar Penceresi", &radius_trackbar, 100);
	createTrackbar("De�me noktalar�", "Trackbar Penceresi", &points_trackbar, 100);
	//Sonsuz bir d�ng� ile kameradan gelen g�r�nt�y� her seferinde al�yoruz.
	while (true){
	cv::Mat bgr_image;
	cap >> bgr_image;

	cv::Mat orig_image = bgr_image.clone();

	cv::medianBlur(bgr_image, bgr_image, 3);

	// Giri� resmini BGR'den HSV'ye d�n��t�rd�k
	cv::Mat hsv_image;
	cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

	// K�rm�z� i�in e�ik de�erler olu�turduk
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
	cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

	// �st ve alt e�ik de�erli k�rm�z�lar� birle�tirip blur uygulad�k
	cv::Mat red_hue_image;
	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);

	// Birle�tirdi�imiz resimdeki daireleri bulduk
	std::vector<cv::Vec3f> circles;

	// HoughCircles negatif veya 0 parametre alm�yor onu engellemek i�in

	if (radius_trackbar < 5)
		radius_trackbar = 5;
	if (points_trackbar < 5)
		points_trackbar = 5;

	cv::HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows / 8, points_trackbar, radius_trackbar);

	//Daire'nin bulunamad��� durumlar i�in
	/*if (circles.size() == 0) std::exit(-1);*/
	
	//Orijinal resimin �st�ne daireleri ekledik
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 0), 5);
	}

	//G�r�nt�leme
	cv::imshow("Alt e�ik", lower_red_hue_range);
	
	cv::imshow("�st e�ik", upper_red_hue_range);

	cv::imshow("Birle�tirilmi� k�rm�z�lar", red_hue_image);
	
	cv::imshow("K�rm�z� ve daire olanlar", orig_image);
	
	//D�ng�den ��kmak i�in ESC tu�una bas�lmal�d�r
	if (cv::waitKey(30) == 27 || bgr_image.empty()) break;
		
	}

	return 0;
}