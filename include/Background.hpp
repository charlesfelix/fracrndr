//
//  Background.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Fr_Background_h
#define Fr_Background_h

#include "Texture.hpp"

using namespace Fr;
class Background
{ 
public:
    DEF_SHARED_PTR_TYPES(Background);
    
    Background(const ImageTexture::ConstPtr & texture):m_texture(texture)
    {
    }
    C4f color ( const Ray & r ) const
    {
        V3f d = r.direction.normalized();
        float u = .5f*(1.f + atan2(d.x,-d.z) / M_PI);
        float v = acos(d.y) / M_PI;
        
        //return m_imgbuf_ptr->getPixel(u*(m_imgbuf_ptr->width()-1), v*(m_imgbuf_ptr->height()-1));
        V3f uv(u,v,0.f);
        return m_texture->sample(uv);
    }
    
private:
    ImageTexture::ConstPtr m_texture;
};


#endif /* Background_h */
