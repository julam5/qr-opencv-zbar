#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <zbar.h>  

using namespace cv;
using namespace std;
using namespace zbar;  


void findIntersection(Point2f* pts, Point2f& center)
{
    for(int i=0;i<4;i++){
                cout << "x: " << i << endl;   
                cout << "pts[x]: " << pts[i] << "; pts[(x+1)%4]:" << pts[(i+1)%4]  << endl; 
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
    cout << "x1: " << x1 << endl; 
    cout << "y1: " << y1 << endl; 
    cout << "x2: " << x2 << endl; 
    cout << "y2: " << y2 << endl;
    cout << "x3: " << x3 << endl; 
    cout << "y3: " << y3 << endl;
    cout << "x4: " << x4 << endl; 
    cout << "y4: " << y4 << endl;

    u = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    cout << "u: " << u << endl;   
    center.x = x1 + u * (x2-x1);
    center.y = y1 + u * (y2-y1);

}




int main(int argc, char** argv )
{
    // Usage:
    // ./camtest (default index is 1)
    // ./camtest <camera_index> | <movie_file>
    // e.g.
    // ./camtest 0
    // ./camtest /home/ubuntu/Downloads/DJI_0004.mp4

    if (argc == 1){
        cout<< "Usage:\n";
        cout<< "./camtest <camera_index> | <movie_file>\n";
        cout<< "e.g.\n";
        cout<< "./camtest 0\n";
        cout<< "./camtest /home/ubuntu/Downloads/DJI_0004.mp4\n" << endl;

    }


    // VideoCapture cap(argv[1]); // open the camera

    const string input = (argc > 1) ? argv[1] : "1"; // default to 1
    char* p;
    int converted = strtol(input.c_str(), &p, 10);
    // cout << "input: " << input << endl;
    // cout << "converted: "<< converted << endl;
    // cout << "p: " << p << endl;
    VideoCapture cap;
    if (*p){
        cap.open(input);
    } else {
        cap.open(converted);
    }





    if(!cap.isOpened()){
        // check if we succeeded
        cout << "Capture open failed !\n" << endl;
        return -1;
    }

    cap.set(cv::CAP_PROP_FPS, 60);
    cap.set(cv::CAP_PROP_FOURCC, CV_FOURCC('Y', 'U', 'Y', 'V'));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);


ImageScanner scanner;   
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);   
   double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video  
   double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video  
   cout << "Frame size : " << dWidth << " x " << dHeight << endl;  


    int count = 0;

    namedWindow("result",1);
    Mat frame;

    // Using time point and system_clock
    std::chrono::time_point<std::chrono::system_clock> start, prev_start;

/*
  start = std::chrono::system_clock::now();
  for(int i = 0; i < 60; i++)
  {
  cap >> frame; // get a new frame from camera
  }

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

*/

    start = std::chrono::system_clock::now();
    for(;;)
    {
        prev_start = start;
        start = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = start - prev_start;
        //std::cout << "FPS: " << 1.0/elapsed_seconds.count() << "s\n";
        cap >> frame; // get a new frame from camera
        //end = std::chrono::system_clock::now();


        if(frame.empty())
            break;

	if(count %5 == 0)
	{
		Mat grey;  
		cv::cvtColor(frame,grey,CV_BGR2GRAY);  
		int width = frame.cols;   
		int height = frame.rows;   
		uchar *raw = (uchar *)grey.data;   
		// wrap image data   
		Image image(width, height, "Y800", raw, width * height);   
		// scan the image for barcodes   
		int n = scanner.scan(image);   
		// extract results   
		for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) 
		{   
		    vector<Point> vp;   
		    // do something useful with results   
		    //cout << "decoded " << symbol->get_type_name() << " symbol \" " << symbol->get_data() << "\"" <<" "<< endl;   
		    int n = symbol->get_location_size();
            //cout << "symbol->get_location_size(): " << n << endl;
		    for(int i=0;i<n;i++){   
		        vp.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));   
		    }   
		    RotatedRect r = minAreaRect(vp);   
		    Point2f pts[4];
            Point2f center; 
		    r.points(pts);   
		    for(int i=0;i<4;i++){
                cout << "i: " << i << endl;   
                cout << "pts[i]: " << pts[i] << "; pts[(i+1)%4]:" << pts[(i+1)%4]  << endl;
		        line(frame,pts[i],pts[(i+1)%4],Scalar(255,0,0),3);   
		    }   
            line(frame,pts[1],pts[3],Scalar(0,0,255),3);
            line(frame,pts[0],pts[2],Scalar(0,255,0),3);
		    //cout<<"Angle: "<<r.angle<<endl;   
            findIntersection(pts, center);
            cout<<"Center: "<<center<<endl;   
            circle(frame, center, 3, Scalar(0,255,255), -1);
		}
        //cout<<"End of result drawn"<<endl;   
		count = 0;
	}else{
		count++;
	}

        // show image
        cv::imshow( "result", frame );
        int fps = cap.get(cv::CAP_PROP_FPS);
        //printf("%d\n",fps);

        if(waitKey(1) >= 0) break;
    }


    return 0;


}
