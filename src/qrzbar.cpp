#include "qrzbar.hpp"

QRZbar::QRZbar(/* args */)
{
}

QRZbar::~QRZbar()
{
}


void QRZbar::init()
{
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);   

}

bool QRZbar::FindQRCenter(cv::Mat& frame, cv::Point2f& center)
{
    cv::Mat grey;  
    cv::cvtColor(frame,grey,CV_BGR2GRAY);  
    int width = frame.cols;   
    int height = frame.rows;   
    uchar *raw = (uchar *)grey.data;   
    // wrap image data   
    zbar::Image image(width, height, "Y800", raw, width * height);   
    // scan the image for barcodes   
    int n = scanner.scan(image);   
    // extract results   
    for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) 
    {   
        std::vector<cv::Point> vp;   
        // do something useful with results   
        //cout << "decoded " << symbol->get_type_name() << " symbol \" " << symbol->get_data() << "\"" <<" "<< endl;   
        int n = symbol->get_location_size();
        //cout << "symbol->get_location_size(): " << n << endl;
        for(int i=0;i<n;i++){   
            vp.push_back(cv::Point(symbol->get_location_x(i),symbol->get_location_y(i)));   
        }   
        cv::RotatedRect r = cv::minAreaRect(vp);   
        cv::Point2f pts[4];
        r.points(pts);   
        for(int i=0;i<4;i++){
            //std::cout << "i: " << i << std::endl;   
            //std::cout << "pts[i]: " << pts[i] << "; pts[(i+1)%4]:" << pts[(i+1)%4]  << std::endl;
            line(frame,pts[i],pts[(i+1)%4],cv::Scalar(255,0,0),3);   
        }   
        line(frame,pts[1],pts[3],cv::Scalar(0,0,255),3);
        line(frame,pts[0],pts[2],cv::Scalar(0,255,0),3);
        //cout<<"Angle: "<<r.angle<<endl;   
        findIntersection(pts, center);
        //std::cout<<"Center: "<<center<<std::endl;   
        circle(frame, center, 3, cv::Scalar(0,255,255), -1);
    }
    return true;
}


void QRZbar::findIntersection(cv::Point2f* pts, cv::Point2f& center)
{
    float x1,y1,x2,y2,x3,y3,x4,y4,u;
    x1 = pts[0].x;
    y1 = pts[0].y;
    x2 = pts[2].x;
    y2 = pts[2].y;
    x3 = pts[1].x;
    y3 = pts[1].y;
    x4 = pts[3].x;
    y4 = pts[3].y;
    /*
    std::cout << "x1: " << x1 << std::endl; 
    std::cout << "y1: " << y1 << std::endl; 
    std::cout << "x2: " << x2 << std::endl; 
    std::cout << "y2: " << y2 << std::endl;
    std::cout << "x3: " << x3 << std::endl; 
    std::cout << "y3: " << y3 << std::endl;
    std::cout << "x4: " << x4 << std::endl; 
    std::cout << "y4: " << y4 << std::endl;
    */
    u = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    //std::cout << "u: " << u << std::endl;   
    center.x = x1 + u * (x2-x1);
    center.y = y1 + u * (y2-y1);

}