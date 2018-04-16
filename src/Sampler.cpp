//
//  Sampler.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/24/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Sampler.hpp"
using namespace Fr;

static unsigned int XorShift32(unsigned int& state)
{
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    state = x;
    return x;
}

SamplerS::SamplerS(unsigned long seed )
{
    m_random = FR_RAND48( seed );
}
float SamplerS::random() const { return m_random.nextf() ; };

V3f SamplerS::sampleUnitSphere() const
{
    V3f p;
    do
    {
        p = 2.f*V3f(m_random.nextf(),m_random.nextf(),m_random.nextf())-V3f(1.f,1.f,1.f);
    } while (p.length2()>=1.0);
    return p;
}

SamplerXorS::SamplerXorS(unsigned int state):m_state(state) {}

float SamplerXorS::random() const {
    return (XorShift32(m_state) & 0xFFFFFF) / 16777216.0f;
}

V3f SamplerXorS::sampleUnitSphere() const
{
    V3f p;
    do
    {
        p = 2.f*V3f(random(),random(),random())-V3f(1.f,1.f,1.f);
    } while (p.length2()>=1.0);
    return p;
}

