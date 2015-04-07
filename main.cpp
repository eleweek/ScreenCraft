#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <exception>
#include <string>

using namespace std;
using namespace cv;

int main(int argc, const char** argv)
{
    cout << "Hi!" << endl;
    if (argc != 3) {
        cerr << "Bad args" << endl;
        throw std::exception();
    }
    string input_image_name(argv[1]);
    string output_image_name(argv[2]);
    Mat image;
    image = imread(input_image_name, 1);
    if (!image.data) {
        throw std::exception();
    }

    Mat processed_image;
    Mat gray_image;
    cvtColor(image, gray_image, CV_BGR2GRAY);
    threshold(gray_image, processed_image, 222, -1, THRESH_TOZERO);

    imwrite(output_image_name, processed_image);

    namedWindow("Color image", CV_WINDOW_AUTOSIZE);
    namedWindow("Processed image", CV_WINDOW_AUTOSIZE);

    imshow("Color image", image);
    imshow("Processed image", processed_image);

    waitKey(0);
}
