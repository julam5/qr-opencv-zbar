#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "qrzbar.hpp"
#include "Airspace/Camera/cBoschCameraCtrlDriver.hpp"

using namespace cv;
using namespace std;



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
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);

    QRZbar qrzbar;
    qrzbar.init();

    int count = 0;

    namedWindow("result",1);
    Mat frame;

  Airspace::BoschConfiguration config; 
  config.camera_control_addr = "10.1.2.101";
  config.camera_control_pass = "Gr0und!Sp4c3";
  config.panOffset = 120; 
  config.tiltOffset = 197; 

  Airspace::BoschLookupEntry e;
  e.level=1;
  e.range = 100;
  config.zoomLookup.insert(std::pair<int,Airspace::BoschLookupEntry>(1,e));

  Airspace::cBoschCameraCtrlDriver Camera(config);


    // Using time point and system_clock
    std::chrono::time_point<std::chrono::system_clock> start, prev_start;

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
        Point2f center;
		bool ok = qrzbar.FindQRCenter(frame,center);
        //cout<<"End of result drawn"<<endl;   
        if (ok) 
        {
            std::cout<<"Center: "<<center<<std::endl; 
            std::cout<<"Frame cols: "<<frame.cols<<std::endl; 
            std::cout<<"Frame rows: "<<frame.rows<<std::endl; 
            std::cout<<"center.x/frame.cols: "<<center.x/frame.cols << "  center.y/frame.rows: " << center.y/frame.rows<<std::endl; 
            
            Camera.moveCameraPix(center.x/frame.cols, center.y/frame.rows);
            sleep(10);
        }else{
            std::cout<<"Not Found"<<std::endl;
        }
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
