//
//  Mandelbulb.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/27/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Mandelbulb_hpp
#define Mandelbulb_hpp

#include "Primitive.hpp"

namespace Fr
{
    
    class Mandelbulb  //:public DistanceEstimator
    {
    public:
        Mandelbulb(float power, const V3f & Z, const V3f & C,unsigned int max_iterations=15,float distance_threshold=0.001) : m_power(power),m_z(Z), m_c(C), m_max_iterations(max_iterations), m_distance_threshold(distance_threshold){}
        float estimate(const V3f & position) const;
        float distanceThreshold() const { return m_distance_threshold; }
        
    private:
        float m_power;
        V3f m_z;
        V3f m_c;
        float m_dr;
        unsigned int m_max_iterations;
        float m_distance_threshold;
    };

} // namespace Fr
#endif /* Mandelbulb_hpp */
