//
//  Primitive.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "RenderPrimitive.hpp"
#include "easylogging++.h"

using namespace Fr;

PrimitiveList & PrimitiveList::addPrimitive(RenderPrimitive::Ptr &primitive)
{
    m_primitives.push_back(primitive);
    m_bounds.extendBy(primitive->getBounds());
    return *this;
}

bool PrimitiveList::hit(const Fr::Ray &r, Real tmin, Real tmax, HitRecord &hit_record) const
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

const Box3f & PrimitiveList::getBounds() const
{
    return m_bounds;
}

bool PrimitiveList::getSubPrimitives(std::vector<RenderPrimitive::ConstPtr> & subprims) const
{
    for (auto p: m_primitives)
    {
        std::vector<RenderPrimitive::ConstPtr> _subprims;
        if (p->isAggregate())
        {
            LOG(INFO) << "AGGREGATE";
            p->getSubPrimitives(_subprims);
        }
        else
        {
            _subprims.push_back(p);
        }
        subprims.insert(subprims.end(), _subprims.begin(), _subprims.end());
    }
    return true;
}
