//
//  main.cpp
//  FracRndr
//
//  Created by Charles-Felix on 4/10/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

//TODO: BVH
//TODO: Ray Differentials
//TODO: UVs & Pattern material to test Ray differentials
//TODO: Importance Sampling
//TODO: Texture Filtering
//TODO: plugins http://www.boost.org/doc/libs/1_65_1/doc/html/boost_dll/tutorial.html

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>

#ifdef GLOG
    #include <glog/logging.h>
#else
    #include "easylogging++.h"
    INITIALIZE_EASYLOGGINGPP
#endif

#include <fstream>

#include "Renderer.hpp"
#include "ImageFileExr.hpp"
#include "Fr.hpp"
#include "TriMesh.hpp"
#include "Sphere.hpp"
#include "BVH.hpp"

using namespace Fr;

void makeCube(TriangleMesh & mesh)
{
    std::vector<V3f> box_positions;
    box_positions.resize(8);
    box_positions[0] = {0.f,0.f,0.f};
    box_positions[1] = {0.f,1.f,0.f};
    box_positions[2] = {1.f,1.f,0.f};
    box_positions[3] = {1.f,0.f,0.f};

    box_positions[4] = {0.f,0.f,1.f};
    box_positions[5] = {1.f,0.f,1.f};
    box_positions[6] = {1.f,1.f,1.f};
    box_positions[7] = {0.f,1.f,1.f};

    std::vector<unsigned> triangles = {
            0, 1, 2, // back
            0, 2, 3,
            5, 6 ,7, // front
            5, 7, 4,
            5, 3, 6, // right
            3, 2, 6,
            0, 4, 1, // left
            4, 7, 1,
            1, 7, 6, // top
            1, 6, 2,
            0, 5, 4, // bottom
            0, 3, 5 };
        
    std::vector<V3f> normals;
    std::vector<V3f> uvs;
    
    mesh = TriangleMesh(box_positions,uvs,normals,triangles);
    mesh.recomputeNormals();
    
}

void testBVH()
{
    std::vector<RenderPrimitive::Ptr> prims;
    
    for (unsigned int i = 0; i < 1000; ++i)
    {
        const V3f pos = { float(randomDouble(0, 10)), float(randomDouble(0, 10)), float(randomDouble(0, 10)) };
        float radius = randomDouble(0, 0.3);
        prims.push_back(RenderPrimitive::Ptr(new Sphere(pos,radius)));
    }
    
    RenderPrimitive::Ptr root = std::make_shared<BVHNode>(prims,0,prims.size());
    
    return;
}

int main(int argc, const char * argv[]) {
    
#ifdef GLOG
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = 1;
    FLAGS_v =2;
#endif
    
#if 0
    //DEM();
    test_oiio();
#endif
    
#if 0
    std::string filepath = "/Users/charles-felix/Documents/Development/fracrndr/scenes/scene3.json";

    Renderer ren;
    ren.initFromFile(filepath);
    
    ren.render();
#endif
    
    testBVH();

    return 0;

}
