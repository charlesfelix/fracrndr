//
//  Mandelbulb.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/27/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Mandelbulb_hpp
#define Mandelbulb_hpp

#include "RenderPrimitive.hpp"

namespace Fr
{
    
    class Mandelbulb  //:public DistanceEstimator
    {
    public:
        Mandelbulb(Real power, const V3f & Z, const V3f & C,
        unsigned int max_iterations=15,Real distance_threshold=0.001) : 
            m_power(power),m_z(Z), m_c(C), m_max_iterations(max_iterations), 
            m_distance_threshold(distance_threshold){}
            
        Real estimate(const V3f & position) const;
        Real distanceThreshold() const { return m_distance_threshold; }
        
    private:
        Real m_power;
        V3f m_z;
        V3f m_c;
        Real m_dr;
        unsigned int m_max_iterations;
        Real m_distance_threshold;
    };

} // namespace Fr
#endif /* Mandelbulb_hpp */
