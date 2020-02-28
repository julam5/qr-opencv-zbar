#ifndef CALCULATEOFFSET_H
#define CALCULATEOFFSET_H

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>

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

class cCalculateOffset
{
    public:
        cCalculateOffset(/* args */);
        ~cCalculateOffset();
        void localOffsetFromGPSDeg(data3d& deltaNed, gps_t* target, gps_t* origin);
        void findOffsets(gps_t homePos, gps_t targetPos, float panCam, float tiltCam);
        
};

#endif