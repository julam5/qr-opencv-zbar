
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>


#include "Airspace/Utils/Config.hpp"
#include "Airspace/Camera/cBoschCameraCtrlDriver.hpp"


int main(int argc, const char** argv )
{

    std::shared_ptr<Airspace::Config> originalConfig  = std::make_shared<Airspace::Config>();
    originalConfig->init("../camera.cfg", argv, argc);

   
    //////////////// Camera
    Airspace::BoschConfiguration config; 

    config.deserialize(originalConfig);

    Airspace::BoschLookupEntry e;
    e.level=1;
    e.range = 100;
    config.zoomLookup.insert(std::pair<int,Airspace::BoschLookupEntry>(1,e));

    Airspace::cBoschCameraCtrlDriver Camera(config);

    /////////////////////////////////////////////////////////////////// Thread Loop

    //Camera.moveCameraPTZ(0,0,1);

    while (true)
    {
        

        std::cout<<"Get Pan :"<< std::fixed << std::setprecision(6)<<Camera.getPan() <<std::endl;
        sleep(1);
        std::cout<<"Get Tilt :"<< std::fixed<< std::setprecision(6)<<Camera.getTilt() <<std::endl<<std::endl;
        //findOffsets(home_position, target_position);
        sleep(1);

    }
    std::cout<<"Left get values loop"<<std::endl;

    return 0;


}
