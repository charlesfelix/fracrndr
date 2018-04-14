//
//  main.cpp
//  FracRndr
//
//  Created by Charles-Felix on 4/10/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//


//TODO: Implement PolyMesh
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

#include "Renderer.hpp"
#include "ImageFileExr.hpp"
#include "Fr.hpp"
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

    Renderer ren;
    ren.initFromFile(filepath);
    ren.render();

    
    IBLTexture();
    return 0;

}
