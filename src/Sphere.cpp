//
//  Sphere.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/23/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Sphere.hpp"
using namespace Fr;

Sphere::Sphere(const V3f center, float radius):m_center(center),m_radius(radius), m_material(nullptr) {}

void Sphere::setMaterial(const std::shared_ptr<Material> & material)
{
    m_material = material;
}

bool Sphere::hit(const Ray & ray, float tmin, float tmax, HitRecord & hit_record) const
{

    float t;
    bool has_it = intersect(ray, t);
    if (has_it && t >= tmin && t <= tmax)
    {
        hit_record.position = ray.positionAt(t);
        hit_record.normal = (hit_record.position - m_center).normalize();
        hit_record.t = t;
        hit_record.material = m_material;
        return true;
    }
    return false;
}

bool Sphere::intersect(const Ray & ray, float &t) const
{
    V3f oc = ray.origin - m_center;
    float a = ray.direction.dot(ray.direction); // always 1 isnt' it?
    float b = 2.f * oc.dot(ray.direction);
    float c = oc.dot(oc) - m_radius*m_radius;
    float discr = b*b - 4.f*a*c;
    if (discr < 0)
        return false;
    float t0 = (-b - sqrt(discr))/(2.f*a);
    float t1 = (-b + sqrt(discr))/(2.f*a);
    
    if (t0 > t1) std::swap(t0, t1);
    
    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0) return false; // both t0 and t1 are negative
    }
    
    t = t0;
    return true;
}
