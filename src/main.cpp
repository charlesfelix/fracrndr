//
//  main.cpp
//  FracRndr
//
//  Created by Charles-Felix on 4/10/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//


//TODO: Ray Differentials
//TODO: UVs & Pattern material to test Ray differentials
//TODO: Importance Sampling
//TODO: Texture Filtering

#ifdef GLOG
    #include <glog/logging.h>
#else
    #include "easylogging++.h"
    INITIALIZE_EASYLOGGINGPP
#endif

#include "Renderer.hpp"


using namespace Fr;


int main(int argc, const char * argv[]) {
    
#ifdef GLOG
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_v =2;
#endif
    
    std::string filepath = "/Development/fracrndr/scenes/scene1.json";

    Renderer ren;
    ren.initFromFile(filepath);
    ren.render();

    return 0;
}
