//
//  Ray.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Ray_hpp
#define Ray_hpp

#include "frMath.h"

namespace Fr {
    
    class Ray {
    public:
        
        Ray():origin(V3f(0.f,0.f,0.f)), direction(V3f(0.f,0.f,-1.f)), mint(0.f),maxt(std::numeric_limits<float>::max()),time(0.f),depth(0){}
        
        Ray(const V3f & o, const V3f &dir, float min_t=0.f, float max_t=std::numeric_limits<float>::max(), float t = 0.f, int d = 0 ):origin(o),direction(dir),mint(min_t),maxt(max_t),depth(d){}
        
        V3f positionAt(float t) const;
        V3f origin;
        V3f direction;
        float mint;
        float maxt;
        float time;
        int depth;
        
    };
    
}; // namespace Fr

#endif /* Ray_hpp */
