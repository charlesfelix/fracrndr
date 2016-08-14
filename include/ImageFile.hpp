//
//  ImageFile.hpp
//  FracRndr
//
//  Created by Charles-Felix on 6/18/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef ImageFile_hpp
#define ImageFile_hpp

#include "ImageBuffer.hpp"
namespace Fr{
    
    class ImageFile {
    public:
        virtual void read (const std::string & filepath, ImageBuffer & imgbuf) =  0;
        virtual void write ( const std::string & filepath, const ImageBuffer & imgbuf) = 0;
    };
}; // namespace
#endif /* ImageFile_hpp */
