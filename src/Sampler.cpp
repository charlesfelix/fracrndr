//
//  Sampler.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/24/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Sampler.hpp"
using namespace Fr;

Sampler::Sampler(unsigned long seed )
{
    m_random = FR_RAND48( seed );
}

V3f Sampler::sampleUnitSphere() const
{
    V3f p;
    do
    {
        p = 2.f*V3f(m_random.nextf(),m_random.nextf(),m_random.nextf())-V3f(1.f,1.f,1.f);
    } while (p.length2()>=1.0);
    return p;
}

float Sampler::random() const { return m_random.nextf() ; };
