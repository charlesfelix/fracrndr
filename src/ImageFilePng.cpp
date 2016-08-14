//
//  ImageFilePng.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/17/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "ImageFilePng.hpp"

#include <fstream>

using namespace Fr;

int toInt(float v)
{
    v = std::min(v,1.f);
    v = std::max(v,0.f);
    return int(v*255.f);
}
void ImageFilePng::write (const std::string & filepath, const ImageBuffer & imgbuf)
{
    std::ofstream fout;
    fout.open(filepath);
    fout << "P3" << std::endl;
    fout << imgbuf.width() << std::endl;
    fout << imgbuf.height() << std::endl;
    fout << "255" << std::endl;

    for (auto c : imgbuf){
        fout << toInt(c.r) << " " << toInt(c.g) << " " << toInt(c.b) << std::endl;
    }
    fout.close();

}
