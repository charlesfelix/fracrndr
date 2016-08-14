//
//  ImageFilePng.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/17/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef ImageFilePng_hpp
#define ImageFilePng_hpp

#include "ImageFile.hpp"
namespace Fr{
    
class ImageFilePng {
public:
    virtual void write (const std::string & filepath, const ImageBuffer & imgbuf) ;
};

} // namespace Fr
#endif /* ImageFilePng_hpp */
