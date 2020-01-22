#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "qrzbar.hpp"

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
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);

    QRZbar qrzbar;
    qrzbar.init();

    int count = 0;

    namedWindow("result",1);
    Mat frame;

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
