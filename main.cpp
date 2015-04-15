#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <exception>
#include <string>
#include <algorithm>

using namespace std;
using namespace cv;

RNG rng(12345);

typedef vector<Mat> MinecraftFont;

void printChar(const Mat& c)
{
    cout << "CHAR!" << endl;
    cout << c.rows << "/" << c.cols << endl;
    for (int i = 0; i < c.rows; ++i) {
        for (int j = 0; j < c.cols; ++j) {
            cout << bool(int(c.at<uchar>(i, j))) << " ";
        }
        cout << endl;
    }
}

char recognizeChar(const MinecraftFont& mf, const Mat& whiteOnBlacImg)
{
    Mat candidate(whiteOnBlacImg.size(), whiteOnBlacImg.type());
    cv::threshold(whiteOnBlacImg, candidate, 0.1, 255, CV_THRESH_BINARY_INV);

    double similarity = 1.0/0.0;
    char result = 0;
    for (int f = 32; f < mf.size(); ++f) {
        if ((candidate.rows > mf[f].rows) || (candidate.cols > mf[f].cols)) {
            // TODO: resize here
            continue;
        }
        for (int si = 0; si < mf[f].rows - candidate.rows + 1; si++) {
            for (int sj = 0; sj < mf[f].cols - candidate.cols + 1; sj++) {
                //cout << "ij " << si << " " << sj << endl;
                Rect charRect = Rect(sj, si, candidate.cols, candidate.rows);
                Mat windowNonBinary(mf[f], charRect);
                //Mat window(windowNonBinary.size(), windowNonBinary.type());
                Mat window = windowNonBinary;
                /*if (f == '1') {
                    cout << si << " " << sj << endl;
                    printChar(window);
                    printChar(candidate);
                    //printChar(whiteOnBlacImg);
                }*/
                double current_similarity = norm(candidate, window);
                //printChar(window);
                if (current_similarity < similarity) {
                    //cout << current_similarity << " vs " << similarity << " " << int(f) << endl; 
                    /*cout << "~~~~~~" << endl;
                    printChar(window);
                    cout << "------" << endl;
                    printChar(candidate);
                    cout << "~~~~~~" << endl;*/
                    result = f;
                    similarity = current_similarity;
                }
            }
        }
    }
    return result;
}

struct contour_sorter // 'less' for contours
{
    bool operator ()( const vector<Point>& a, const vector<Point> & b )
    {
        Rect ra(boundingRect(a));
        Rect rb(boundingRect(b));
        //return (ra.y < rb.y) || ((ra.y == rb.y) && (ra.x < rb.x));
        return (ra.br().y < rb.br().y) || ((ra.br().y == rb.br().y) && (ra.br().x < ra.br().x));
    }
};

void ScreenShotBB(const Mat& src, const Mat& processed_image, const MinecraftFont& mf)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(processed_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    sort(contours.begin(), contours.end(), contour_sorter());
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    // TODO: copy src to drawing
    Mat drawing = src;
    /*Mat drawing;
    cvtColor(processed_image, drawing, CV_GRAY2RGB);*/
    for( int i = 0; i < contours.size(); i++ ) {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
         //drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 1, 1, 0 );
        Mat c(processed_image, boundRect[i]);
        printChar(c);

        char rc = recognizeChar(mf, c);

        //cout << "~~~~~~~~~~" << endl;
        if (rc > 31) {
            //cout << "OCR char: " << rc << " " << int(rc) << endl;
            cout << rc;
        } else {
            cout << "Char not recognized!" << endl;
        }
        // cout << "~~~~~~~~~~" << endl;
        cout << endl;
    }
    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    imshow("Contours", drawing);
    waitKey(0);
}

MinecraftFont LoadMinecraftFontImg(const std::string& imgName)
{
    MinecraftFont font;

    Mat src = imread(imgName, 1);
    Mat srcGray;
    cvtColor(src, srcGray, CV_BGR2GRAY);
    Mat drawing = src;
    for(int i = 4; i < src.cols - 16; i += 16) {
        for (int j = 4; j < src.rows - 16; j += 16) {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            Rect charRect = Rect(j, i, 16, 16);
            rectangle(drawing, charRect.tl(), charRect.br(), color, 0, 0, 0);
            Mat c(srcGray, charRect);
            Mat cc;
            c.copyTo(cc);
            font.push_back(c);
        }
    }
    namedWindow("Font", CV_WINDOW_AUTOSIZE);
    imshow("Font", src);
    //waitKey(0);
    return font;
}

int main(int argc, const char** argv)
{
    cout << "Hi!" << endl;
    auto font = LoadMinecraftFontImg("minecraft_fixedwidth_font.png");
    /*for (int f = 0; f < font.size(); ++f) {
        cout << "NEW CHAR " << char(f) << endl;
        printChar(font[f]);
    }*/

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
    ScreenShotBB(image, processed_image, font);

    //namedWindow("Color image", CV_WINDOW_AUTOSIZE);
    //namedWindow("Processed image", CV_WINDOW_AUTOSIZE);

    //imshow("Color image", image);
    //imshow("Processed image", processed_image);
}
