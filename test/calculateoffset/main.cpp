
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Airspace/Utils/Config.hpp"
#include "Airspace/Camera/cBoschCameraCtrlDriver.hpp"

#define DEG2RAD 0.01745329252

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






int main(int argc, const char** argv )
{

    std::shared_ptr<Airspace::Config> originalConfig  = std::make_shared<Airspace::Config>();
    originalConfig->init("../camera.cfg", argv, argc);

    std::string stream;

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

    
    std::cout << "Exiting Main Function" << std::endl;

    return 0;


}
