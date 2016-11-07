//
//  Background.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Fr_Background_h
#define Fr_Background_h

#include "ImageBuffer.hpp"

using namespace Fr;
class Background
{
public:
    DEF_SHARED_PTR_TYPES(Background);
    
    Background(const ImageBuffer::Ptr & imgbuf_ptr):m_imgbuf_ptr(imgbuf_ptr)
    {
    }
    C4f color ( const Ray & r ) const
    {
        V3f d = r.direction.normalized();
        float u = .5f*(1.f + atan2(d.x,-d.z) / M_PI);
        float v = acos(d.y) / M_PI;
        
        return m_imgbuf_ptr->getPixel(u*(m_imgbuf_ptr->width()-1), v*(m_imgbuf_ptr->height()-1));
    }
    
private:
    ImageBuffer::Ptr m_imgbuf_ptr;
};


#endif /* Background_h */
