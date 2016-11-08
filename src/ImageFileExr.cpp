//
//  ImageFileExr.cpp
//  FracRndr
//
//  Created by Charles-Felix on 6/18/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "ImageFileExr.hpp"
#include "Fr.hpp"
#include <ImfArray.h>
#include <ImfInputFile.h>
#include <ImfRgbaFile.h>
#include <ImfTiledRgbaFile.h>
#include <ImfRgba.h>
#include <fstream>

using namespace Fr;
using namespace Imf;
using namespace Imath;


void ImageFileExr::read(const std::string & filepath, ImageBuffer & imgbuf)
{
    Array2D<Rgba> pixels;
    int width,height;

    RgbaInputFile file (filepath.c_str());
    Box2i dw = file.dataWindow();
    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;
    pixels.resizeErase (height, width);
    file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels (dw.min.y, dw.max.y);
    
    imgbuf.resize(width,height);
    for (size_t j = 0; j < height; ++j)
        for (size_t i = 0; i < width; ++i)
        {
            Rgba & pixel = pixels[j][i];
            C4f c = C4f(pixel.r,pixel.g,pixel.b,pixel.a);
            imgbuf.setPixel(i,j,c);
            
        }

}

void ImageFileExr::readmm(const std::string & filepath, MipMapBuffer & mmbuf)
{
    // todo - make sure this is a mip map texture
    //
    
    Array2D<Rgba> pixels;
    int width,height;
    
    TiledRgbaInputFile in (filepath.c_str());
    std::cout << "Num X Levels " << in.numXLevels() << std::endl;
    std::cout << "Num Y Levels " << in.numYLevels() << std::endl;
    std::cout << "Num Levels " << in.numLevels() << std::endl;
    int num_levels = in.numLevels();
    
    Header h = in.header();
    
    int nx_tiles = in.numXTiles();
    int ny_tiles = in.numYTiles();
    
    for (int l = 0; l < num_levels; ++l)
    {
        Box2i dw = in.dataWindowForLevel(l,l);

        width = dw.max.x - dw.min.x + 1;
        height = dw.max.y - dw.min.y + 1;
        
        int dx = dw.min.x;
        int dy = dw.min.y;
        
        pixels.resizeErase (height, width);
        
        in.setFrameBuffer (&pixels[-dy][-dx], 1, width);
        
        std::cout << "-----------/nImageFileExr::readmm::load level " << l << std::endl;
        std::cout << "ImageFileExr::readmm::--nx_tiles " << nx_tiles << std::endl;
        std::cout << "ImageFileExr::readmm::--ny_tiles " << ny_tiles << std::endl;
        std::cout << "ImageFileExr::readmm::--width " << width << std::endl;
        std::cout << "ImageFileExr::readmm::--height " << height << std::endl;
        
        in.readTiles (0, nx_tiles - 1, 0, ny_tiles - 1, l);
        
        ImageBuffer::Ptr imgbuf_ptr = std::make_shared<ImageBuffer>(width,height);
        
        for (size_t j = 0; j < height; ++j)
            for (size_t i = 0; i < width; ++i)
            {
                Rgba & pixel = pixels[j][i];
                C4f c = C4f(pixel.r,pixel.g,pixel.b,pixel.a);
                imgbuf_ptr->setPixel(i,j,c);
                
            }
        mmbuf.addLevel(imgbuf_ptr);
        // update variables for next iterations
        nx_tiles = std::max(1,(nx_tiles + 1) / 2);
        ny_tiles = std::max(1,(ny_tiles + 1) / 2);

    }
    
    
}

void ImageFileExr::write (const std::string & filepath, const ImageBuffer & imgbuf)
{

    RgbaOutputFile file (filepath.c_str(), int(imgbuf.width()), int(imgbuf.height()), WRITE_RGBA); // 1
    
    Rgba * pixels = new Rgba[imgbuf.width()*imgbuf.height()]();
    Rgba * pixel_cur = pixels;
    for (size_t j = 0; j < imgbuf.height(); ++j)
    {
        for (size_t i = 0; i < imgbuf.width(); ++i)
        {
            C4f c = imgbuf.getPixel(i, j);
            *pixel_cur=Rgba(half(c.r),half(c.g),half(c.b),half(c.a));
            ++pixel_cur;
        }
    }
    file.setFrameBuffer (pixels, 1, imgbuf.width()); // 2
    file.writePixels (int(imgbuf.height())); // 3

    delete [] pixels;
}
