//
//  ImageFileExr.hpp
//  FracRndr
//
//  Created by Charles-Felix on 6/18/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef ImageFileExr_hpp
#define ImageFileExr_hpp

#include "ImageFile.hpp"
namespace Fr{
    
    class ImageFileExr {
    public:
        virtual void read (const std::string & filepath, ImageBuffer & imgbuf) ;
        virtual void readmm (const std::string & filepath, MipMapBuffer & mmbuf) ;
        virtual void write (const std::string & filepath, const ImageBuffer & imgbuf) ;
    };
}

#endif /* ImageFileExr_hpp */
