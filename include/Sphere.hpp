//
//  Sphere.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/23/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include "frMath.h"
#include "Primitive.hpp"
namespace Fr {

    class Lambertian;
    class Sphere : public Primitive
    {
    public:
        Sphere(const V3f center, float radius, std::shared_ptr<Material> & material);
        virtual ~Sphere(){};
        virtual bool hit(const Ray & ray, float tmin, float tmax, HitRecord & hit_record) const;
        virtual bool intersect(const Ray & ray, float &t) const;
    private:
        Sphere();
        Sphere(const Sphere & sphere);
        V3f m_center;
        float m_radius;
        std::shared_ptr<Material> m_material;
    };
};
#endif /* Sphere_hpp */
