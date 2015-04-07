#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <exception>
#include <string>

using namespace std;
using namespace cv;

RNG rng(12345);

void ScreenShotBB(const Mat& src, const Mat& processed_image)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(processed_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    Mat drawing = src;
    for( int i = 0; i < contours.size(); i++ ) {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        // drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 1, 1, 0 );
    }
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow("Contours", drawing);
    waitKey(0);
}

void LoadMinecraftFontImg(const std::string& imgName)
{
    Mat src = imread(imgName, 1);
    Mat drawing = src;
    for(int i = 4; i < src.rows - 16; i += 16) {
        for (int j = 4; j < src.cols - 16; j += 16) {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            // drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            rectangle(drawing, Rect(i, j, 16, 16).tl(), Rect(i, j, 16, 16).br(), color, 0, 0, 0);
        }
    }
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    waitKey(0);
}

int main(int argc, const char** argv)
{
    cout << "Hi!" << endl;
    LoadMinecraftFontImg("minecraft_fixedwidth_font.png");

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
    ScreenShotBB(image, processed_image);

    //namedWindow("Color image", CV_WINDOW_AUTOSIZE);
    //namedWindow("Processed image", CV_WINDOW_AUTOSIZE);

    //imshow("Color image", image);
    //imshow("Processed image", processed_image);

    //waitKey(0);
}
