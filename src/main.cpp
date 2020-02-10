#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <future>
#include <vector>
#include <regex>

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

// for string delimiter
vector<string> parseStringdata (std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if(token.size() != 0){
            res.push_back (token);
        }
    }

    // last item check push back
    token = s.substr (pos_start);
    if(token.size() != 0){
        res.push_back (token);
    }
    return res;
}

void assignValuesFromLLH(std::vector<string> gps_data, double& latitude, double& longitude, double& altitude)
{
    latitude = std::stod(gps_data[2]);
    longitude = std::stod(gps_data[3]);
    altitude = std::stod(gps_data[4]);
}



void calcLoop(std::shared_ptr<Airspace::Config> originalConfig, bool& ok, float& normX, float& normY, bool& keepGoing, std::string& gps_data)
{
    /////////////////////////////////////////////////////////////////// Initialize


    //////////////// Camera
    Airspace::BoschConfiguration config; 

    config.deserialize(originalConfig);

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
    originalConfig->cfg_get_value("Target.alt", target_position.altitude);


    findOffsets(home_position, target_position);

    //Airspace::cBoschCameraCtrlDriver Camera(config);

    std::vector<std::string> gps_parsed;



    /////////////////////////////////////////////////////////////////// Thread Loop

    while (keepGoing)
    {
        gps_parsed.clear();
        std::cout<<"GPS Data: "<< gps_data <<std::endl;
        gps_parsed = parseStringdata(gps_data, " ");

        if(gps_parsed.size() != 0){
            assignValuesFromLLH(gps_parsed, target_position.latitude, target_position.longitude, target_position.altitude);
            std::cout<<"Target.lat: "       << std::fixed << std::setprecision(6)<<target_position.latitude <<std::endl;
            std::cout<<"Target.lon: "       << std::fixed << std::setprecision(6)<<target_position.longitude <<std::endl;
            std::cout<<"Target.baseHeight: "<< std::fixed << std::setprecision(6)<<target_position.height <<std::endl;
            std::cout<<"Target.altitude: "  << std::fixed << std::setprecision(6)<<target_position.altitude <<std::endl;
            findOffsets(home_position, target_position);
        }else{
            std::cout<<"No GPS data available"<<std::endl;
        }

        //findOffsets(home_position, target_position);


        if(ok == true){
            std::cout<<"Found Center! :"<< normX <<","<< normY<<std::endl;
            //Camera.moveCameraPix(normX, normY);
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
    int port_num;
    std::string ip_address;

    originalConfig->cfg_get_value("GPS.ipAddress", ip_address);
    originalConfig->cfg_get_value("GPS.port", port_num);

    cSocketComm oSocket(ip_address, port_num); 

    bool gps_socket_status = false;
    gps_socket_status = oSocket.initConnection();



    /////////////////////////////////////////////////////////////////// Thread Loop

    while (keepGoing)
    {
        if(!gps_socket_status)
        {
            std::cout<<"No connection with socket!" <<std::endl;
            oSocket.resetConnection();
            
        }else{
            oSocket.readSocket(gps_data,gps_socket_status);
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


    QRZbar qrzbar;
    qrzbar.init();

    int count = 0;

    namedWindow("result",1);
    Mat frame;


    bool ok = false;
    float normX = -1.0;
    float normY = -1.0;

    bool keepGoing = true;

    std::string gps_data = "";

    std::thread thread1(&calcLoop, originalConfig, std::ref(ok), std::ref(normX), std::ref(normY), std::ref(keepGoing), std::ref(gps_data));
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
		ok = qrzbar.FindQRCenter(frame,center);
  
        if (ok) 
        {
            normX = center.x/frame.cols;
            normY = center.y/frame.rows;
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
