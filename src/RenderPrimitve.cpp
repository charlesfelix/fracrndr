//
//  Primitive.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "RenderPrimitve.hpp"

using namespace Fr;

PrimitiveList & PrimitiveList::addPrimitive(RenderPrimitve::Ptr &primitive)
{
    m_primitives.push_back(primitive);
    return *this;
}

bool PrimitiveList::hit(const Fr::Ray &r, float tmin, float tmax, HitRecord &hit_record) const
{
    HitRecord closest_hit;
    bool has_hit = false;
    for (auto &p : m_primitives)
    {
        HitRecord current_hit;
        if (p->hit(r,tmin,tmax,current_hit) && current_hit.t < closest_hit.t)
        {
            closest_hit = current_hit;
            has_hit = true;
        }
    }
    hit_record = closest_hit;
    return has_hit;
}
