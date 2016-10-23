//
//  Film.cpp
//  FracRndr
//
//  Created by Charles-Felix on 10/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Film.hpp"
#include "ImageFileExr.hpp"

using namespace Fr;

Film::Film(size_t resx, size_t resy):m_imagebuffer(std::make_shared<ImageBuffer>(resx,resy))
{
}

void Film::addSample(float x, float y, const C4f& color)
{
    m_imagebuffer->setPixel(x,y,color);
}

bool Film::writeImage(const std::string & filepath) const
{
    ImageFileExr fout;
    fout.write(filepath.c_str(),*m_imagebuffer);
    return true;
}

size_t Film::width() const
{
    return m_imagebuffer->width();
}

size_t Film::height() const
{
    return m_imagebuffer->height();
}
