//
//  Film.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Film_hpp
#define Film_hpp

#include <string>
#include "Fr.hpp"

#include "ImageBuffer.hpp"

namespace Fr
{

    class Film {

    public:
        
        DEF_SHARED_PTR_TYPES(Film);
        
        Film(size_t xres, size_t yres);
        
        void addSample(float x, float y, const C4f & color);
        
        bool writeImage(const std::string & filepath) const;
        
        size_t width() const;
        
        size_t height() const;
        
    private:
        Film(const Film&);
        
        ImageBuffer::Ptr m_imagebuffer;
    }; // Film

} // Fr

#endif /* Film_hpp */
