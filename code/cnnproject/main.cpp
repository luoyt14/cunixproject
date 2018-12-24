#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

using Eigen::MatrixXd;

int main() {
	MatrixXd m(2, 2);
	m(0, 0) = 3;
	m(1, 0) = 2.5;
	m(0, 1) = -1;
	m(1, 1) = m(1, 0) + m(0, 1);
	std::cout << m << std::endl;

	cv::Mat srcImage = cv::imread("test.jpg");
	cv::imshow("show", srcImage);
	cv::waitKey(6000);
}