//
//  Texture.cpp
//  FracRndr
//
//  Created by Charles-Felix on 11/26/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Texture.hpp"

using namespace Fr;
C4f ImageTexture::sample(const V3f & uv) const
{
    return m_imgbuf->getPixel(float(uv[0]),float(uv[1]));
}
