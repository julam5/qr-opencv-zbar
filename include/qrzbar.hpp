#ifndef QRZBAR_H
#define QRZBAR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <zbar.h>  

class QRZbar
{
    private:
        zbar::ImageScanner scanner;  

        void findIntersection(cv::Point2f* pts, cv::Point2f& center);

    public:
        QRZbar(/* args */);
        ~QRZbar();
        void init();
        bool FindQRCenter(cv::Mat& frame, cv::Point2f& center);
        
};

#endif