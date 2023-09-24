#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <ctime>
using namespace std;

int CHECKERBOARD[2]{7, 8};
cv::VideoCapture cap(0);

int main()
{
    cv::Mat cameraMatrix, distCoeffs, R, T;

    // calibration(&cameraMatrix, &distCoeffs, &R, &T);

    readCamera();
    return 0;
}

void readCamera()
{
    cv::Mat frame;
    int i = 0;
    while (cap.isOpened())
    {
        cap.read(frame);
        if (!frame.empty())
        {
            cv::imshow("Image", frame);
            cout << std::time(0) << endl;
            if (std::time(0) % 5000 == 0)
            {
                cv::imwrite("./checkerboard_img/checkerboard" + std::to_string(i) + ".jpg", frame);
                i++;
            }
            char c = std::cin.get();
            if (cv::waitKey(0) && c == 'q')
            {
                break;
            }
        };
    }
}

void calibration(cv::Mat *add_cameraMatrix, cv::Mat *add_distCoeffs, cv::Mat *add_R, cv::Mat *add_T)
{
    vector<vector<cv::Point3f>> obj_points; // storing vectors of 3d real world points
    vector<vector<cv::Point2f>> img_points; // sotring vectors of 2d points in image space

    // real world coordinates
    vector<cv::Point3f> obj_coords;
    for (int i = 0; i < CHECKERBOARD[1]; i++)
    {
        for (int j = 0; i < CHECKERBOARD[0]; j++)
        {
            obj_coords.push_back(cv::Point3f(j, i, 0));
        }
    }

    vector<cv::String> images;
    string path = "./images/*.jpg";
    cv::glob(path, images);

    cv::Mat frame, gray;
    vector<cv::Point2f> corner_pts;
    bool success;

    for (int i = 0; i < images.size(); i++)
    {
        frame = cv::imread(images[i]);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

        if (success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);
            // refining pixel coordinates for given 2d points.
            cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

            // Displaying the detected corner points on the checker board
            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

            obj_points.push_back(obj_coords);
            img_points.push_back(corner_pts);
        }

        cv::imshow("Image", frame);
        cv::waitKey(0);
    }

    cv::destroyAllWindows();

    cv::Mat cameraMatrix, distCoeffs, R, T;

    cv::calibrateCamera(obj_points, img_points, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

    *add_cameraMatrix = cameraMatrix;
    *add_distCoeffs = distCoeffs;
    *add_R = R;
    *add_T = T;
}