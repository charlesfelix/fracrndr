//
//  Sampler.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/24/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Sampler_hpp
#define Sampler_hpp

#include "frMath.h"

namespace Fr
{
    class Sampler
    {
    public:
        Sampler(unsigned long seed = 11212);
        Sampler(const Sampler & sampler):m_seed(sampler.m_seed+3),m_random(FR_RAND48(sampler.m_seed+3)){}
        V3f sampleUnitSphere() const;
        float random() const;
    private:
        unsigned long m_seed;
        mutable FR_RAND48 m_random;
    };
    
} // namespace Fr
#endif /* Sampler_hpp */
