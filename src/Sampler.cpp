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
        p = static_cast<Real>(2.0)*V3f(m_random.nextf(),m_random.nextf(),m_random.nextf())-V3f(1.,1.,1.);
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
        p = static_cast<Real>(2.0)*V3f(random(), random(), random())-V3f(1.,1.,1.);
    } while (p.length2()>=1.0);
return p;
}

V3f SamplerXorS::sampleHemisphere() const
{
    V3f p;
    // cos(theta) = r1 = y
    // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
    const float r1 = random();
    const float r2 = random();
    float sinTheta = sqrtf(1 - r1*r1);
    float phi = 2 * M_PI * r2;
    float x = sinTheta * cosf(phi);
    float z = sinTheta * sinf(phi);
    return V3f(x, r1, z);
}


V3f SamplerXorS::sampleHemisphereCosine() const
{
    V3f p;
    // cos(theta) = r1 = y
    // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
    const float r1 = random();
    const float r2 = random();
    float y = sqrtf(1 - r2);
    float sqrtr2 = sqrtf(r2);
    float phi = 2 * M_PI * r1;
    float x = sqrtr2 * cosf(phi);
    float z = sqrtr2 * sinf(phi);
    return V3f(x, y, z);
}

