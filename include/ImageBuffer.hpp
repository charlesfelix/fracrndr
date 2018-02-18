//
//  ImageBuffer.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef ImageBuffer_hpp
#define ImageBuffer_hpp

#include "Fr.hpp"
#include <vector>

namespace Fr {
    class ImageBuffer {
        
    public:
        DEF_SHARED_PTR_TYPES(ImageBuffer);
        
        typedef std::vector<C4f>::iterator iterator;
        typedef std::vector<C4f>::const_iterator const_iterator;
        
        ImageBuffer(size_t width, size_t height, const C4f &default_color =C4f(0.f,0.f,0.f,0.f));
        size_t width() const;
        size_t height() const;
        
        void resize(size_t width, size_t height, const C4f &default_color =C4f(0.f,0.f,0.f,0.f));
        
        iterator begin();
        const_iterator begin() const;
        
        iterator end();
        const_iterator end() const;
        
        void setPixel(size_t x, size_t y, const C4f & c);
        C4f & getPixel(size_t x, size_t y);
        const C4f & getPixel(size_t x, size_t y) const;
        C4f getPixel(float x, float y) const;
        
        V2i linearToXY(size_t linear);
        size_t xyToLinear(size_t x, size_t y);

        
    private:
        void clamp(float &x, float &y) const;
        void iclamp(size_t &x, size_t &y) const;
        ImageBuffer();
        ImageBuffer(const ImageBuffer & );
        std::vector<C4f> m_data;
        size_t m_width;
        size_t m_height;
        
    }; // ImageBuffer
    
    class MipMapBuffer {
    public:
        MipMapBuffer(){}
        ImageBuffer::Ptr level(unsigned int l) {return m_mm_levels[l];}
        void addLevel(ImageBuffer::Ptr & mm_level);
        unsigned int numLevels() const;
    private:
        std::vector<ImageBuffer::Ptr> m_mm_levels;
        
    }; // MipMapBuffer

}; // namespace Fr

#endif /* ImageBuffer_hpp */
