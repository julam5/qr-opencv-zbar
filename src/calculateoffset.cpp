#include "calculateoffset.hpp"

cCalculateOffset::cCalculateOffset(/* args */)
{
}

cCalculateOffset::~cCalculateOffset()
{
}



inline double deg_to_rad( double deg )
{
    return deg * M_PI / 180.0;
}

void cCalculateOffset::localOffsetFromGPSDeg(data3d& deltaNed, gps_t* target, gps_t* origin)
{
    //Expecting gps in rad
    double R_EARTH = 6378137.0;
    double deltaLon = deg_to_rad(target->longitude - origin->longitude);
    double deltaLat = deg_to_rad(target->latitude  - origin->latitude );

    deltaNed.x = deltaLat * R_EARTH;
    deltaNed.y = deltaLon * R_EARTH * abs(cos(deg_to_rad(target->latitude)));
    deltaNed.z = (target->altitude + target->height) - (origin->altitude + origin->height);
}


void cCalculateOffset::findOffsets(gps_t homePos, gps_t targetPos)
{

    data3d anchorToTarget;
    localOffsetFromGPSDeg(anchorToTarget, &targetPos, &homePos);
    double yaw = atan2(anchorToTarget.y, anchorToTarget.x) / DEG2RAD;
    double range = sqrt(anchorToTarget.x * anchorToTarget.x + anchorToTarget.y * anchorToTarget.y);
    double tilt = atan2(anchorToTarget.z, range) / DEG2RAD;


    //std::cout << std::fixed << std::setprecision(6)<< "Pan, Range, and Tilt (Real) P=" << yaw << " R=" << range <<" T=" << tilt << std::endl;
    std::cout << std::fixed << std::setprecision(6)<< "Pan (True North Bearing)= " << yaw << std::endl;
    std::cout << std::fixed << std::setprecision(6)<< "Tilt (Horizon Zero)= " << tilt << std::endl;
    //double tiltOffset = this->config.tiltOffset; //196; // approx. for level view
    //double panOffset = this->config.panOffset;  //19.2; // add to get true north

}

