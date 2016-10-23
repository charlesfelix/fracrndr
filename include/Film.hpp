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
#include "frMath.h"

#include "ImageBuffer.hpp"

namespace Fr
{

    class Film {

    public:
        
        Film(size_t xres, size_t yres);
        
        void addSample(float x, float y, const C4f & color);
        
        bool writeImage(const std::string & filepath) const;
        
        size_t width() const;
        
        size_t height() const;
        
    private:
        Film(const Film&);
        
        std::shared_ptr<ImageBuffer> m_imagebuffer;
    }; // Film

} // Fr

#endif /* Film_hpp */
