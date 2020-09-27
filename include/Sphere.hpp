//
//  Sphere.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/23/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include "Fr.hpp"
#include "RenderPrimitive.hpp"
#include "Material.hpp"

namespace Fr {

    class Lambertian;
    class Sphere : public RenderPrimitive
    {
    public:
        Sphere(const V3f &center, float radius);
        virtual ~Sphere(){};
        virtual void setMaterial(const std::shared_ptr<Material> & material);
        virtual bool hit(const Ray & ray, float tmin, float tmax, HitRecord & hit_record) const;
        virtual bool intersect(const Ray & ray, float &t) const;
        virtual const Box3f & getBounds() const;
    private:
        
        Sphere();
        Sphere(const Sphere & sphere);
        V3f m_center;
        float m_radius;
        Box3f m_bounds;
        Material::Ptr m_material;
    };
};
#endif /* Sphere_hpp */
