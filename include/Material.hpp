//
//  Material.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/25/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Material_hpp
#define Material_hpp

#include "Ray.hpp"
#include "Primitive.hpp"
#include "Sampler.hpp"

namespace Fr {
    class Material
    {
    public:
        virtual bool scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered) = 0;
    };
    
    class Lambertian : public Material
    {
    public:
        Lambertian(const C3f & albedo=C3f(.5f,.5f,.5f)) : m_albedo(albedo) {}
        virtual bool scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered);
    private:
        C3f m_albedo;
        static Sampler s_sampler;
    };
    
    class SimpleMetal : public Material
    {
    public:
        SimpleMetal(const C3f & albedo=C3f(.5f,.5f,.5f), float roughness=0.f):
        m_albedo(albedo), m_roughness(roughness) {}
        virtual bool scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered);
    private:
        C3f m_albedo;
        float m_roughness;
        static Sampler s_sampler;
    };
    
    class Glass : public Material
    {
    public:
        Glass(float refraction_index, float roughness=0.f):
        m_refraction_index(refraction_index), m_roughness(roughness) {}
        virtual bool scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered);
    private:
        float m_refraction_index;
        float m_roughness;
        static Sampler s_sampler;
    };

    
    
    
};
#endif /* Material_hpp */
