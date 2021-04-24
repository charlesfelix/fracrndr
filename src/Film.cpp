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

Film::Film(size_t resx, size_t resy):m_imagebuffer(std::make_shared<ImageBuffer>(resx,resy)),
                                                   m_aovbuffer_1(std::make_shared<ImageBuffer>(resx,resy))
{
}

void Film::addSample(Real x, Real y, const C4f& color, const C3f& normal, float depth)
{
    m_imagebuffer->setPixel(x,y,color);
    C4f aov;
    aov.r = normal.x;
    aov.g = normal.y;
    aov.b = normal.z;
    aov.a = depth;
    m_aovbuffer_1->setPixel(x, y, aov);
}

bool Film::writeImage(const std::string & filepath) const
{
    ImageFileExr fout;
    fout.write(filepath.c_str(),*m_imagebuffer);
    std::string aovpath = filepath;
    std::string aov_suffix = ".aov1.exr";
    aovpath.replace(aovpath.end() - 4, aovpath.end(), aov_suffix);
    fout.write(aovpath.c_str(),*m_aovbuffer_1);

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
