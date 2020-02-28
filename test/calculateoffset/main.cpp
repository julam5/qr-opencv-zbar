
#include <iostream>

#include "Airspace/Utils/Config.hpp"
#include "calculateoffset.hpp"

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


    //findOffsets(home_position, target_position);
    cCalculateOffset cco;

    cco.findOffsets(home_position, target_position, 0.0, 0.0);
    

    
    std::cout << "Exiting Main Function" << std::endl;

    return 0;


}
