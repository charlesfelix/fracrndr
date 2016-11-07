//
//  main.cpp
//  FracRndr
//
//  Created by Charles-Felix on 4/10/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <tbb/tbb.h>
#include <tbb/mutex.h>
//#include <glog/logging.h>
#include "easylogging++.h"

#include "frMath.h"
#include "Camera.hpp"
#include "ImageBuffer.hpp"
#include "ImageFilePng.hpp"
#include "ImageFileExr.hpp"
#include "Sphere.hpp"
#include "Sampler.hpp"
#include "Material.hpp"
#include "Mandelbulb.hpp"
#include "DistanceField.hpp"
#include "Scene.hpp"
#include "Film.hpp"
#include "Renderer.hpp"
#include "json.hpp"
#include "Parser.hpp"

// for convenience
using json = nlohmann::json;

// INITLOGGING
INITIALIZE_EASYLOGGINGPP

//TODO: Ray Differentials
//TODO: UVs & Pattern material to test Ray differentials
//TODO: Importance Sampling

using namespace Fr;


int main(int argc, const char * argv[]) {
    
#ifdef GLOG
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_v =2;
#endif
    
    std::string filepath = "/Users/charles-felix/Documents/Development/fracrndr/scenes/scene1.json";

    Renderer ren;
    ren.initFromFile(filepath);
    ren.render();

    return 0;
}
