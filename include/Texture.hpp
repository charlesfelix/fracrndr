//
//  Texture.hpp
//  FracRndr
//
//  Created by Charles-Felix on 11/26/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

#include "Fr.hpp"
#include "ImageBuffer.hpp"

namespace Fr {
    class Texture
    {
    public:
        virtual C4f sample(const V3f & uv) const = 0;
    }; // Texture


    class ImageTexture
    {
    public:
        DEF_SHARED_PTR_TYPES(ImageTexture);
        
        ImageTexture(const ImageTexture & t):m_imgbuf(t.m_imgbuf){};
        ImageTexture(const ImageBuffer::ConstPtr & imgbuf): m_imgbuf(imgbuf){};
        virtual C4f sample(const V3f & uv) const;
    
    private:
        ImageTexture();
        ImageBuffer::ConstPtr m_imgbuf;
    }; // ImageTexture

    class LightTexture
    {
    public:
        DEF_SHARED_PTR_TYPES(LightTexture);
        
        LightTexture(const LightTexture & t):m_imgbuf(t.m_imgbuf){};
        LightTexture(const ImageBuffer::ConstPtr & imgbuf): m_imgbuf(imgbuf){};
        virtual C4f sample(const V3f & uv) const;
        
    private:
        LightTexture();
        ImageBuffer::ConstPtr m_imgbuf;
        
    }; // LightTexture


}

#endif /* Texture_h */
