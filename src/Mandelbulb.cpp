//
//  Mandelbulb.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/27/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Mandelbulb.hpp"

using namespace Fr;

// The fractal calculation
//
// Calculate the closest distance to the fractal boundary and use this
// distance as the size of the step to take in the ray marching.
//
// Fractal formula:
//    z' = z^p + c
//
// For each iteration we also calculate the derivative so we can estimate
// the distance to the nearest point in the fractal set, which then sets the
// maxiumum step we can move the ray forward before having to repeat the calculation.
//
//   dz' = p * z^(p-1)
//
// The distance estimation is then calculated with:
//
//   0.5 * |z| * log(|z|) / |dz|
//

float r(const V3f &pos)
{
    return pos.length();
}

float phi(const V3f & pos)
{
    return atan2(pos.y,pos.x);
}

float theta(const V3f & pos)
{
    float l = r(pos);
    if (l < 0.00001) return 0.f;
    return acos(pos.z/l);
}

float bulbx(const V3f & pos, float n)
{
    return sin(n*theta(pos)) * cos(n*phi(pos));
}

float bulby(const V3f & pos, float n)
{
    return sin(n*theta(pos)) * sin(n*phi(pos));
}

float bulbz(const V3f & pos, float n)
{
    return cos(n*theta(pos));
}

V3f bulb(float N, int max_iter, const V3f & Z,const V3f & C, float &dr)
{
    V3f z = Z;
    V3f c = C;
    
    int keep_going = 1;
    int iter = 0;
    float power = N;
    dr = 1;
    
    while (keep_going)
    {
        
        float radius = z.length();
        if (radius > 4 || iter > max_iter)
        {
            keep_going = 0;
        }
        else
        {
            float m = pow(z.length(),power);
            const float bx = bulbx(z,power);
            const float by = bulby(z,power);
            const float bz = bulbz(z,power);

            z = V3f(bx,by,bz)*m + c;
            dr = pow(radius,power-1)*power*dr + 1;
        }
        iter++;
    }
    
    return z;
}

float Mandelbulb::estimate(const V3f & position) const
{
    float dr = 1.f;

    V3f z = bulb(m_power,m_max_iterations,position,position,dr);
    const float radius = z.length();
    return 0.5f*log(radius)*radius/dr;
}

