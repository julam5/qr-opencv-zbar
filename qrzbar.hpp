#ifndef QRZBAR_H
#define QRZBAR_H

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <zbar.h>  

class QRZbar
{
    private:



    public:
        QRZbar(/* args */);
        ~QRZbar();
        void findIntersection(cv::Point2f* pts, cv::Point2f& center);
};

QRZbar::QRZbar(/* args */)
{
}

QRZbar::~QRZbar()
{
}

#endif