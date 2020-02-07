#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <future>

#include "socketcomm.hpp"
#include "qrzbar.hpp"
#include "Airspace/Utils/Config.hpp"
#include "Airspace/Camera/cBoschCameraCtrlDriver.hpp"

#define DEG2RAD 0.01745329252

using namespace cv;
using namespace std;

struct data3d{
    double x;
    double y;
    double z;
    
    // default constructor
    data3d(int x=0, int y=0, int z=0):x(x),y(y),z(z)
    {
    }

    // add operator
    data3d operator+(const data3d& a){
        return data3d(x+a.x,y+a.y,z+a.z); 
    }
};


typedef struct gps_st{

    double latitude;
    double longitude;
    double altitude;
    double height;
}gps_t;


inline double deg_to_rad( double deg )
{
    return deg * M_PI / 180.0;
}

void localOffsetFromGPSDeg(data3d& deltaNed, gps_t* target, gps_t* origin)
{
    //Expecting gps in rad
    double R_EARTH = 6378137.0;
    double deltaLon = deg_to_rad(target->longitude - origin->longitude);
    double deltaLat = deg_to_rad(target->latitude  - origin->latitude );

    deltaNed.x = deltaLat * R_EARTH;
    deltaNed.y = deltaLon * R_EARTH * abs(cos(deg_to_rad(target->latitude)));
    deltaNed.z = target->height - origin->height;
}


void findOffsets(gps_t homePos, gps_t targetPos)
{

    data3d anchorToTarget;
    localOffsetFromGPSDeg(anchorToTarget, &targetPos, &homePos);
    double yaw = atan2(anchorToTarget.y, anchorToTarget.x) / DEG2RAD;
    double range = sqrt(anchorToTarget.x * anchorToTarget.x + anchorToTarget.y * anchorToTarget.y);
    double tilt = atan2(anchorToTarget.z, range) / DEG2RAD;


    std::cout << "Pan, Range, and Tilt (Real) P=" << yaw << " R=" << range <<" T=" << tilt << std::endl;
    //double tiltOffset = this->config.tiltOffset; //196; // approx. for level view
    //double panOffset = this->config.panOffset;  //19.2; // add to get true north

}



void calcLoop(std::shared_ptr<Airspace::Config> originalConfig, std::shared_ptr<bool> pok, std::shared_ptr<float> pnormX, std::shared_ptr<float> pnormY, bool& keepGoing, std::string& gps_data)
{
    /////////////////////////////////////////////////////////////////// Initialize


    //////////////// Camera
    Airspace::BoschConfiguration config; 

    config.deserialize(originalConfig);

    //originalConfig->cfg_get_value("Camera.camera_control_addr", config.camera_control_addr);
    //originalConfig->cfg_get_value("Camera.camera_control_pass", config.camera_control_pass);
    //originalConfig->cfg_get_value("Camera.panOffset", config.panOffset);
    //originalConfig->cfg_get_value("Camera.tiltOffset", config.tiltOffset);

    //config.camera_control_addr = "10.1.203.20";
    //config.camera_control_pass = "Groundspace123#";
    //config.panOffset = 120; 
    //config.tiltOffset = 197; 

    //Airspace::BoschLookupEntry e;
    //e.level=1;
    //e.range = 100;
    //config.zoomLookup.insert(std::pair<int,Airspace::BoschLookupEntry>(1,e));


    gps_t home_position;
    originalConfig->cfg_get_value("Location.lat", home_position.latitude);
    originalConfig->cfg_get_value("Location.lon", home_position.longitude);
    originalConfig->cfg_get_value("Location.baseHeight", home_position.height);
    originalConfig->cfg_get_value("Location.alt", home_position.altitude);

    gps_t target_position;
    originalConfig->cfg_get_value("Target.lat", target_position.latitude);
    originalConfig->cfg_get_value("Target.lon", target_position.longitude);
    originalConfig->cfg_get_value("Target.baseHeight", target_position.height);
    originalConfig->cfg_get_value("Target.alt", target_position .altitude);


    findOffsets(home_position, target_position);

    //Airspace::cBoschCameraCtrlDriver Camera(config);



    /////////////////////////////////////////////////////////////////// Thread Loop

    while (keepGoing)
    {
        
        std::cout<<"GPS Data: "<< gps_data <<std::endl;
        if(*pok == true){
            std::cout<<"Found Center! :"<< *pnormX <<","<< *pnormY<<std::endl;
            //Camera.moveCameraPix(*pnormX, *pnormY);
            sleep(3);
            //std::cout<<"Get Pan :"<< std::fixed << std::setprecision(6)<<Camera.getPan() <<std::endl;
            sleep(1);
            //std::cout<<"Get Tilt :"<< std::fixed<< std::setprecision(6)<<Camera.getTilt() <<std::endl;
        }
        sleep(5);
    }
    std::cout<<"Left calcLoop"<<std::endl;
}

void fetchGPSLoop(std::shared_ptr<Airspace::Config> originalConfig, bool& keepGoing, std::string& gps_data)
{
    /////////////////////////////////////////////////////////////////// Initialize
    /////////////// GPS
    cSocketComm oSocket; 
    int port_num;
    std::string ip_address;

    originalConfig->cfg_get_value("GPS.ipAddress", ip_address);
    originalConfig->cfg_get_value("GPS.port", port_num);

    bool gps_socket_status = false;
    gps_socket_status = oSocket.initConnection();



    /////////////////////////////////////////////////////////////////// Thread Loop

    while (keepGoing)
    {
        if(!gps_socket_status)
        {
            std::cout<<"No connection with socket!" <<"\n";
            
        }else{
            gps_data = oSocket.readSocket();
        }
        usleep(500);
    }
    std::cout<<"Left fetchGPSLoop"<<std::endl;
}

int main(int argc, const char** argv )
{

    std::shared_ptr<Airspace::Config> originalConfig  = std::make_shared<Airspace::Config>();
    originalConfig->init("../camera.cfg", argv, argc);


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

    bool keepGoing = true;

    std::string gps_data = "No gps data!";

    std::thread thread1(&calcLoop, originalConfig, pok, pnormX, pnormY, std::ref(keepGoing), std::ref(gps_data));
    std::thread thread2(&fetchGPSLoop, originalConfig, std::ref(keepGoing), std::ref(gps_data));


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
    keepGoing = false;
    //Wait for thread to join
    thread1.join();
    thread2.join();
    std::cout << "Exiting Main Function" << std::endl;

    return 0;


}
