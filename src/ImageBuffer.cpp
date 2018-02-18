//
//  ImageBuffer.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

// fast bilinear interpolation - sse:
// http://fastcpp.blogspot.com/2011/06/bilinear-pixel-interpolation-using-sse.html

#include "ImageBuffer.hpp"

namespace Fr {
    
    void ImageBuffer::clamp(float &x, float &y) const
    {
        if (x < 0.f) x = 0.f;
        if (x > 1) x = 1.f;
        if (y < 0.f) y = 0.f;
        if (y > 1.0) y = 1.f;
    }
    
    void ImageBuffer::iclamp(size_t &x, size_t &y) const
    {
        if (x > m_width-1) x = m_width-1;
        if (y > m_height-1) y = m_height-1;
    }
    
    ImageBuffer::ImageBuffer(size_t width, size_t height, const C4f &default_color):m_width(width), m_height(height)
    {
        size_t s = m_width*m_height;
        m_data = std::vector<C4f>(s,default_color);
    }
    size_t ImageBuffer::width() const { return m_width; };
    size_t ImageBuffer::height() const { return m_height; };

    void ImageBuffer::resize(size_t width, size_t height, const C4f &default_color)
    {
        m_width=width;
        m_height=height;
        size_t s = m_width*m_height;
        m_data = std::vector<C4f>(s,default_color);
    }
    
    ImageBuffer::iterator ImageBuffer::begin() { return m_data.begin(); }
    ImageBuffer::const_iterator ImageBuffer::begin() const { return m_data.begin(); };
    ImageBuffer::iterator ImageBuffer::end() { return m_data.end(); }
    ImageBuffer::const_iterator ImageBuffer::end() const {return m_data.end(); };
    
    void ImageBuffer::setPixel(size_t x, size_t y, const C4f & c) { getPixel(x,y) = c; }
    C4f & ImageBuffer::getPixel(size_t x, size_t y) { return m_data[y*m_width + x]; };
    const C4f & ImageBuffer::getPixel(size_t x, size_t y) const { return m_data[y*m_width + x]; };
    
    C4f ImageBuffer::getPixel(float x, float y) const {
        
        // make sure that the x and y values are in [0,1]
        clamp(x,y);
        
        // convert the coordinates in image space
        float ix = x * (m_width-1);
        float iy = y * (m_height-1);

        // compute the bilinear weights
        float frac_ix = ix - std::floor(ix);
        float frac_iy = iy - std::floor(iy);
        
        // generate the coordinates for the 4 pixel samples
        size_t i0 = static_cast<size_t>(ix);
        size_t i1 = i0 + 1;
        size_t j0 = static_cast<size_t>(iy);
        size_t j1 = j0 + 1;
        
        // clamp the pixel coordinates to avoid bad memory access
        iclamp(i0,j0);
        iclamp(i1,j1);
        
        // get the colors of all 4 pixels
        C4f p00 = getPixel(i0, j0);
        C4f p01 = getPixel(i1, j0);
        C4f p10 = getPixel(i0, j1);
        C4f p11 = getPixel(i1, j1);
        
        // perform and return the bilinear interpolation
        return (1-frac_iy)*((1-frac_ix) * p00 + frac_ix * p01) + frac_iy*((1-frac_ix) * p10 + frac_ix * p11);
    };

    V2i ImageBuffer::linearToXY(size_t linear_coord)
    {
        V2i coord;
        coord.x = (int)linear_coord/width();
        coord.y = (int)linear_coord/height();
        return coord;
    }
    
    size_t ImageBuffer::xyToLinear(size_t x, size_t y)
    {
        return y*width() + x;
    }
    
    void MipMapBuffer::addLevel(ImageBuffer::Ptr & mm_level)
    {
        m_mm_levels.push_back(mm_level);
    }
    unsigned int MipMapBuffer::numLevels() const
    {
        return static_cast<unsigned int> (m_mm_levels.size());
    }

    
}
