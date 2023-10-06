#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/video.hpp>
#include <sys/time.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <numeric>

cv::VideoCapture cap(0);

cv::Mat ReadMatFromTxt(std::string filename, int rows,int cols)
{
    double m;
    cv::Mat out = cv::Mat::zeros(rows, cols, CV_64FC1);//Matrix to store values

    std::ifstream fileStream(filename);
    int cnt = 0;//index starts from 0
    while (fileStream >> m)
    {
        int temprow = cnt / cols;
        int tempcol = cnt % cols;
        out.at<double>(temprow, tempcol) = m;
        cnt++;
    }
    return out;
}

bool compare(const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
    return cv::contourArea(a) > cv::contourArea(b);
}

void findTagCorners(cv::Mat frame, int kernelSize, std::vector<std::vector<cv::Point>> *majorContours) {
    cv::Mat gray, blurred, thresh;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> heirarchy;

    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurred, cv::Size(kernelSize, kernelSize), 0);

    cv::adaptiveThreshold(blurred, thresh, 255, cv::AdaptiveThresholdTypes::ADAPTIVE_THRESH_MEAN_C, cv::ThresholdTypes::THRESH_BINARY_INV, 3, 0);
    cv::findContours(thresh, contours, heirarchy, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    std::vector<int> indices(contours.size());
    iota(indices.begin(), indices.end(), 0);

    std::sort(contours.begin(), contours.end(), compare);

    int N = 4; // set number of largest contours
    N = std::min(N, int(contours.size()));
    for(int i = 1; i <= N; i++) {
        cv::drawContours(frame, contours, indices[i], cv::Scalar(0, 255, 0), 2);
        majorContours->push_back(contours[indices[i]]);
    }

    cv::imshow("Contours", frame);
}

int main()
{
    cv::Mat cameraMatrix, distCoeffs;
    cameraMatrix = ReadMatFromTxt("undistort.txt", 3, 3);
    distCoeffs = ReadMatFromTxt("distcoeffs.txt", 1, 5);

    cv::Mat frame, undistorted;
    std::vector<std::vector<cv::Point>> majorContours;
    while (cap.isOpened())
    {
        cap.read(frame);
        if (!frame.empty())
        {
            cv::undistort(frame, undistorted, cameraMatrix, distCoeffs);
            findTagCorners(undistorted, 7, &majorContours);
            int k = cv::waitKey(1) & 0XFF;
            if (k == 27)
                break;
        }
    }
    return 0;
}