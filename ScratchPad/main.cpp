//
//  main.cpp
//  ScratchPad
//
//  Created by Charles-Felix on 11/16/19.
//  Copyright © 2019 Charles-Felix. All rights reserved.
//

#include <stdio.h>

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

using namespace Fr;


void computeSummedAreaTable(std::vector<double> &sat, const ImageBuffer::Ptr & imgbuf_ptr)
{
    const size_t w = imgbuf_ptr->width();
    const size_t h = imgbuf_ptr->height();
    
    sat.resize(w*h);

    auto sat_it = sat.begin();
    double sum = 0.0;
    
    double inv_npixels = 1.0;///double(w*h);
    
    // 1st row
    for (size_t i = 0; i < w; ++i)
    {
        *sat_it++ = sum += FR_LUMINANCE(imgbuf_ptr->getPixel(i,0))*inv_npixels;
    }
    
    // first col
    sat_it = sat.begin();
    sum = 0.0;
    for (size_t j = 0; j < h; ++j)
    {
        *sat_it = sum += FR_LUMINANCE(imgbuf_ptr->getPixel(0,j))*inv_npixels;
        sat_it += w; // jump width pixels to get to next row
    }
    
    // fill inside
    sat_it = sat.begin() + w + 1; // position the cursor to (1,1)
    for (size_t j = 1; j < h; ++j)
    {
        sat_it = sat.begin() + j*w + 1; // position the cursor to (1,j)
        for (size_t i = 1; i < w; ++i)
        {
            const double d = (imgbuf_ptr->getPixel(i,j).r)*inv_npixels;
            const double c = sat[imgbuf_ptr->xyToLinear(i-1,j)];
            const double b = sat[imgbuf_ptr->xyToLinear(i,j-1)];
            const double a = sat[imgbuf_ptr->xyToLinear(i-1,j-1)];
            *sat_it += d + c + b - a;
            sat_it++;
        }
    }
    
    std::cout << "SAT MAX "  << sat[sat.size()-1] << std::endl;
    
}

void IBLTexture()
{
    ImageBuffer::Ptr imgbuf_ptr = std::make_shared<ImageBuffer>(4,4,C4f(1,1,1,1));
    std::string fpath = "/Users/charles-felix/Documents/Development/fracrndr/images/uffizi-tiny-mm.exr";
    ImageFileExr().read(fpath,*imgbuf_ptr);
    
    std::vector<double> summap;
    summap.resize(imgbuf_ptr->width()*imgbuf_ptr->height(),0.0);
    // compute the sum area map
    
    computeSummedAreaTable(summap,imgbuf_ptr);
    
    // compute the number of tiles
    const size_t tile_width = 8;
    const size_t tile_height = 8;
    const size_t ntiles_x = (imgbuf_ptr->width()/float(tile_width));
    const size_t ntiles_y = (imgbuf_ptr->height()/float(tile_height));
    
   ImageBuffer::Ptr output_buf =
        ImageBuffer::Ptr::make_shared(imgbuf_ptr->width(),imgbuf_ptr->height());
    
    const size_t w = imgbuf_ptr->width();
    const size_t h = imgbuf_ptr->height();
    
    auto output_it = output_buf->begin();
    
    for (size_t j = 0; j < h ; ++j)
    {
        for (size_t i = 0; i < w ; ++i)
        {
            const size_t x = i; const size_t y = j;
            
            const size_t tile_x = (x*ntiles_x/(w));
            const size_t tile_y = (y*ntiles_y/(h));
            
            const size_t x_tile_min = std::min((tile_width-1) * tile_x,w-1);
            const size_t y_tile_min = std::min((tile_height-1) * tile_y,h-1);
            const size_t x_tile_max = std::min((tile_width-1) * (tile_x + 1),w-1);
            const size_t y_tile_max = std::min((tile_height-1) * (tile_y + 1),h-1);
            
            double xk0 = 1.0;
            if (x_tile_min == 0) xk0 = 1.0;
            double yk0 = 1.0;
            if (y_tile_min == 0) yk0 = 1.0;
            
            const size_t a = imgbuf_ptr->xyToLinear(x_tile_min, y_tile_min);
            const size_t b = imgbuf_ptr->xyToLinear(x_tile_max, y_tile_min);
            const size_t c = imgbuf_ptr->xyToLinear(x_tile_min, y_tile_max);
            const size_t d = imgbuf_ptr->xyToLinear(x_tile_max, y_tile_max);
            
            const double tile_sum = summap[d] - summap[b]*yk0 - summap[c]*xk0 + summap[a]*xk0*yk0;
            
            Fr::C4f color = C4f(float(tile_x),summap[y*w+x],tile_sum,1.f);//,summap[y*w+x]
            *output_it++ = color;
        }
    }
    
    ImageFileExr().write("/Users/charles-felix/Desktop/sum.exr",*output_buf);

}

void test_oiio()
{
    const char * filename = "/tmp/cool_image1.exr";
    
    OIIO::ImageSpec spec1;
    //spec1.
    
    OIIO::ImageInput *in = OIIO::ImageInput::open (filename);
    if (! in)
        return;
    const OIIO::ImageSpec &spec = in->spec();
    int xres = spec.width;
    int yres = spec.height;
    int channels = spec.nchannels;
    std::vector<unsigned char> pixels (xres*yres*channels);

    in->read_image (OIIO::TypeDesc::UINT8, &pixels[0]);
    in->close ();
    OIIO::ImageInput::destroy (in);
    
    OIIO::ImageBuf imgbuf(filename);
    imgbuf.read();
    imgbuf.write("/tmp/oiio_write.exr");
    
}

void testTriMesh()
{
//    Fr::TriangleMesh trimesh;
    
}

void testRandom()
{
    SamplerXorS sampler = SamplerXorS(1212);
    std::ofstream fout;
    fout.open("/tmp/a.txt");
    for (int i =0 ; i < 10000; ++i)
    {
        V3f r = sampler.sampleUnitSphere();

        fout << r.x << " " << r.y << " " << r.z << std::endl;
    
    }
    fout.close();
}

void testHemisphereRandom()
{
    SamplerXorS sampler = SamplerXorS(1212);
    std::ofstream fout;
    fout.open("/tmp/a2.txt");
    
    const V3f nN = V3f(0.5,0.5,.5).normalized();
    V3f tangent, bitangent;
    Basis::build(nN,tangent,bitangent);
    
    LOG(INFO) << "n " << nN.x << " " << nN.y <<  " " << nN.z << std::endl;
    LOG(INFO) << "t " << tangent.x << " " << tangent.y <<  " " << tangent.z << std::endl;
    LOG(INFO) << "b " << bitangent.x << " " << bitangent.y <<  " " << bitangent.z << std::endl;
    for (int i =0 ; i < 10000; ++i)
    {
        const V3f ndir = sampler.sampleHemisphere();
        
        
        V3f dir;
        dir.x = ndir.x * bitangent.x + ndir.y * nN.x + ndir.z * tangent.x;
        dir.y = ndir.x * bitangent.y + ndir.y * nN.y + ndir.z * tangent.y;
        dir.z = ndir.x * bitangent.z + ndir.y * nN.z + ndir.z * tangent.z;
        
        fout << dir.x << " " << dir.y << " " << dir.z << std::endl;
        
    }
    fout.close();
}


void testTrimesh()
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
    
    TriangleMesh mesh = TriangleMesh(box_positions,uvs,normals,triangles);
    mesh.recomputeNormals();
    LOG(INFO) << "TriangleMesh::numTriangles(): " << mesh.numTriangles() << std::endl;
    for (unsigned i = 0; i < mesh.numTriangles(); ++i)
    {
        TriangleMesh::Triangle t = mesh.triangle(i);
        LOG(INFO) << "TriangleMesh Triangle " << i;
        LOG(INFO) << "TriangleMesh::position(): " << t.position(0);
        LOG(INFO) << "TriangleMesh::position(): " << t.position(1);
        LOG(INFO) << "TriangleMesh::position(): " << t.position(2);

    }
    
    Ray ray;
    HitRecord rec;
    ray.origin = V3f(0.501f,0.5f,3.f);
    ray.direction = V3f(0.f,0.f,-1.f);
    bool does_hit = mesh.hit(ray,FLT_MIN,FLT_MAX,rec);
    if (does_hit)
        LOG(INFO) << "TriangleMesh::hit() p=" << rec.position << " | n = " << rec.normal;
    else
        LOG(INFO) << "TriangleMesh::hit() - no hit";
    
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
    
    std::string filepath = "/Development/fracrndr/scenes/scene1.json";


    
    //testRandom();
    //testHemisphereRandom();
    //IBLTexture();
    testTrimesh();
    return 0;

}
