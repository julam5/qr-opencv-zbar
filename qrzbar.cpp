#include "qrzbar.hpp"

void QRZbar::findIntersection(cv::Point2f* pts, cv::Point2f& center)
{
    for(int i=0;i<4;i++){
                std::cout << "x: " << i << std::endl;   
                std::cout << "pts[x]: " << pts[i] << "; pts[(x+1)%4]:" << pts[(i+1)%4]  << std::endl; 
	}

    float x1,y1,x2,y2,x3,y3,x4,y4,u;
    x1 = pts[0].x;
    y1 = pts[0].y;
    x2 = pts[2].x;
    y2 = pts[2].y;
    x3 = pts[1].x;
    y3 = pts[1].y;
    x4 = pts[3].x;
    y4 = pts[3].y;
    std::cout << "x1: " << x1 << std::endl; 
    std::cout << "y1: " << y1 << std::endl; 
    std::cout << "x2: " << x2 << std::endl; 
    std::cout << "y2: " << y2 << std::endl;
    std::cout << "x3: " << x3 << std::endl; 
    std::cout << "y3: " << y3 << std::endl;
    std::cout << "x4: " << x4 << std::endl; 
    std::cout << "y4: " << y4 << std::endl;

    u = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    std::cout << "u: " << u << std::endl;   
    center.x = x1 + u * (x2-x1);
    center.y = y1 + u * (y2-y1);

}