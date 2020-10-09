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

Real r(const V3f &pos)
{
    return pos.length();
}

Real phi(const V3f & pos)
{
    return atan2(pos.y,pos.x);
}

Real theta(const V3f & pos)
{
    Real l = r(pos);
    if (l < 0.00001) return 0.f;
    return acos(pos.z/l);
}

Real bulbx(const V3f & pos, Real n)
{
    return sin(n*theta(pos)) * cos(n*phi(pos));
}

Real bulby(const V3f & pos, Real n)
{
    return sin(n*theta(pos)) * sin(n*phi(pos));
}

Real bulbz(const V3f & pos, Real n)
{
    return cos(n*theta(pos));
}

V3f bulb(Real N, int max_iter, const V3f & Z,const V3f & C, Real &dr)
{
    V3f z = Z;
    V3f c = C;
    
    int keep_going = 1;
    int iter = 0;
    Real power = N;
    dr = 1;
    
    while (keep_going)
    {
        
        Real radius = z.length();
        if (radius > 4 || iter > max_iter)
        {
            keep_going = 0;
        }
        else
        {
            Real m = pow(z.length(),power);
            const Real bx = bulbx(z,power);
            const Real by = bulby(z,power);
            const Real bz = bulbz(z,power);

            z = V3f(bx,by,bz)*m + c;
            dr = pow(radius,power-1)*power*dr + 1;
        }
        iter++;
    }
    
    return z;
}

Real Mandelbulb::estimate(const V3f & position) const
{
    Real dr = 1.f;

    V3f z = bulb(m_power,m_max_iterations,position,position,dr);
    const Real radius = z.length();
    return 0.5f*log(radius)*radius/dr;
}

