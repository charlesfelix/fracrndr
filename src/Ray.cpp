//
//  Ray.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Ray.hpp"

using namespace Fr;

V3f Ray::positionAt(Real t) const
{
    V3f d = direction;
    return origin + t*d;
}
