//
//  Sampler.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/24/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Sampler_hpp
#define Sampler_hpp

#include "Fr.hpp"

namespace Fr
{
    
    class SamplerS
    {
    public:
        SamplerS(unsigned long seed = 11212);
        SamplerS(const SamplerS & sampler):m_seed(sampler.m_seed+3),m_random(FR_RAND48(sampler.m_seed+3)){}
        V3f sampleUnitSphere() const;
        float random() const;
    private:
        SamplerS();
        unsigned long m_seed;
        mutable FR_RAND48 m_random;
    };
    
    class SamplerXorS
    {
    public:
        SamplerXorS(unsigned int seed = 11212);
        V3f sampleUnitSphere() const;
        float random() const;
    private:
        mutable unsigned int m_state;
    };
    
    typedef SamplerXorS Sampler;
    

} // namespace Fr
#endif /* Sampler_hpp */
