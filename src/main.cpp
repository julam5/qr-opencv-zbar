#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <future>

#include "qrzbar.hpp"
#include "Airspace/Utils/Config.hpp"
#include "Airspace/Camera/cBoschCameraCtrlDriver.hpp"

using namespace cv;
using namespace std;


void threadFunction(std::shared_ptr<Airspace::Config> originalConfig, std::shared_ptr<bool> pok, std::shared_ptr<float> pnormX, std::shared_ptr<float> pnormY, std::future<void> futureObj)
{
    Airspace::BoschConfiguration config; 

    originalConfig->cfg_get_value("Camera.camera_control_addr", config.camera_control_addr);
    originalConfig->cfg_get_value("Camera.camera_control_pass", config.camera_control_pass);
    originalConfig->cfg_get_value("Camera.panOffset", config.panOffset);
    originalConfig->cfg_get_value("Camera.tiltOffset", config.tiltOffset);

    //config.camera_control_addr = "10.1.203.20";
    //config.camera_control_pass = "Groundspace123#";
    //config.panOffset = 120; 
    //config.tiltOffset = 197; 

    Airspace::BoschLookupEntry e;
    e.level=1;
    e.range = 100;
    config.zoomLookup.insert(std::pair<int,Airspace::BoschLookupEntry>(1,e));

    Airspace::cBoschCameraCtrlDriver Camera(config);

    while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
    {
        //std::cout<<"Threading"<<std::endl;
        if(*pok == true){
            std::cout<<"Found Center! :"<< *pnormX <<","<< *pnormY<<std::endl;
            Camera.moveCameraPix(*pnormX, *pnormY);
            sleep(3);
            std::cout<<"Get Pan :"<< std::fixed << std::setprecision(6)<<Camera.getPan() <<std::endl;
            sleep(1);
            std::cout<<"Get Tilt :"<< std::fixed<< std::setprecision(6)<<Camera.getTilt() <<std::endl;
        }
        sleep(5);
    }
    std::cout<<"Left thread"<<std::endl;
}

int main(int argc, const char** argv )
{

    std::shared_ptr<Airspace::Config> originalConfig  = std::make_shared<Airspace::Config>();
    originalConfig->init("camera.cfg", argv, argc);


    if (argc == 1){
        cout<< "Usage:\n";
        cout<< "./camtest <camera_index> | <movie_file>\n";
        cout<< "e.g.\n";
        cout<< "./camtest 0\n";
        cout<< "./camtest /home/ubuntu/Downloads/DJI_0004.mp4\n" << endl;

    }


    string stream;
    originalConfig->cfg_get_value("Camera.stream", stream);
    //std::cout << stream << std::endl;

    // VideoCapture cap(argv[1]); // open the camera

    const string input = (argc > 1) ? argv[1] : stream; // default to 1
    char* p;
    int converted = strtol(input.c_str(), &p, 10);
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

    //cap.set(cv::CAP_PROP_FPS, 60);
    //cap.set(cv::CAP_PROP_FOURCC, CV_FOURCC('Y', 'U', 'Y', 'V'));
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);

    QRZbar qrzbar;
    qrzbar.init();

    int count = 0;

    namedWindow("result",1);
    Mat frame;


    bool ok = false;
    std::shared_ptr<bool> pok = std::make_shared<bool>(ok);

    float normX = -1.0;
    std::shared_ptr<float> pnormX = std::make_shared<float>(normX);
    float normY = -1.0;
    std::shared_ptr<float> pnormY = std::make_shared<float>(normY);


    // Create a std::promise object
	std::promise<void> exitSignal;
 
	//Fetch std::future object associated with promise
	std::future<void> futureObj = exitSignal.get_future();




    std::thread th(&threadFunction, originalConfig, pok, pnormX, pnormY, std::move(futureObj));


    // Using time point and system_clock
    std::chrono::time_point<std::chrono::system_clock> start, prev_start;

    start = std::chrono::system_clock::now();
    for(;;)
    {
        cap >> frame; // get a new frame from camera


        if(frame.empty())
            break;

	if(count == 10)
	{
        Point2f center;
		*pok = qrzbar.FindQRCenter(frame,center);
        //cout<<"End of result drawn"<<endl;   
        if (*pok) 
        {
            //std::cout<<"Center: "<<center<<std::endl; 
            //std::cout<<"Frame cols: "<<frame.cols<<std::endl; 
            //std::cout<<"Frame rows: "<<frame.rows<<std::endl; 
            //std::cout<<"center.x/frame.cols: "<<center.x/frame.cols << "  center.y/frame.rows: " << center.y/frame.rows<<std::endl; 
            
            //Camera.moveCameraPix(center.x/frame.cols, center.y/frame.rows);
            //sleep(10);
            //std::cout<<"Found Center Main! :"<< *pnormX <<","<< *pnormY<<std::endl;
            *pnormX = center.x/frame.cols;
            *pnormY = center.y/frame.rows;
        }else{
            //std::cout<<"Not Found"<<std::endl;
        }
		count = 0;
	}else{
		count++;
	}

        // show image
        cv::imshow( "result", frame );

        if(waitKey(1) >= 0) break;
    }
    //Set the value in promise
    exitSignal.set_value();
    //Wait for thread to join
    th.join();
    std::cout << "Exiting Main Function" << std::endl;

    return 0;


}
