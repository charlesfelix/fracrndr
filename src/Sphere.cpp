//
//  Sphere.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/23/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Sphere.hpp"
using namespace Fr;

Sphere::Sphere(const V3f &center, Real radius):m_center(center),m_radius(radius), m_material(nullptr)
{
    m_bounds.makeEmpty();
    m_bounds.extendBy(center+V3f(radius,radius,radius));
    m_bounds.extendBy(center-V3f(radius,radius,radius));
    
}

void Sphere::setMaterial(const std::shared_ptr<Material> & material)
{
    m_material = material;
}

bool Sphere::hit(const Ray & ray, Real tmin, Real tmax, HitRecord & hit_record) const
{

    Real t;
    bool has_it = intersect(ray, t);
    if (has_it && t >= tmin && t <= tmax)
    {
        hit_record.position = ray.positionAt(t);
        hit_record.normal = (hit_record.position - m_center).normalize();
        hit_record.t = t;
        hit_record.material = m_material.get();
        return true;
    }
    return false;
}

bool Sphere::intersect(const Ray & ray, Real &t) const
{
    V3f oc = ray.origin - m_center;
    Real a = ray.direction.dot(ray.direction); // always 1 isnt' it?
    Real b = 2.f * oc.dot(ray.direction);
    Real c = oc.dot(oc) - m_radius*m_radius;
    Real discr = b*b - 4.f*a*c;
    if (discr < 0)
        return false;
    Real t0 = (-b - sqrt(discr))/(2.f*a);
    Real t1 = (-b + sqrt(discr))/(2.f*a);
    
    if (t0 > t1) std::swap(t0, t1);
    
    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0) return false; // both t0 and t1 are negative
    }
    
    t = t0;
    return true;
}

const Box3f & Sphere::getBounds() const
{
    return m_bounds;
}
