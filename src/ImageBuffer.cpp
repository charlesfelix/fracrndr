//
//  ImageBuffer.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "ImageBuffer.hpp"

namespace Fr {
    
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
    
    void MipMapBuffer::addLevel(std::shared_ptr<ImageBuffer> & mm_level)
    {
        m_mm_levels.push_back(mm_level);
    }
    unsigned int MipMapBuffer::numLevels() const
    {
        return static_cast<unsigned int> (m_mm_levels.size());
    }

    
}