//
//  Material.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/25/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Material.hpp"

using namespace Fr;

//Sampler Lambertian::s_sampler = Sampler(1212);

inline float fresnel_dielectric(float cosi, float eta) {
    // special case: ignore fresnel
    if (eta == 0)
        return 1;
    
    // compute fresnel reflectance without explicitly computing the refracted direction
    if (cosi < 0.0f) eta = 1.0f / eta;
    float c = fabsf(cosi);
    float g = eta * eta - 1 + c * c;
    if (g > 0) {
        g = sqrtf(g);
        float A = (g - c) / (g + c);
        float B = (c * (g + c) - 1) / (c * (g - c) + 1);
        return 0.5f * A * A * (1 + B * B);
    }
    return 1.0f; // TIR (no refracted component)
}

inline float fresnel_refraction(const V3f& I, const V3f& N, float eta, V3f& T) {
    // compute refracted direction and fresnel term
    // return value will be 0 if TIR occurs
    // NOTE: I is the incoming ray direction (points toward the surface, normalized)
    //       N is the surface normal (points toward the incoming ray origin, normalized)
    //       T is the outgoing refracted direction (points away from the surface)
    V3f In = I.normalized();
    V3f Nn = N.normalized();
    float cosi = -In.dot(Nn);
    // check which side of the surface we are on
    float neta;
    if (cosi > 0) {
        // we are on the outside of the surface, going in
        neta = 1 / eta;
        Nn = Nn;
    } else {
        // we are inside the surface,
        cosi = -cosi;
        neta = eta;
        Nn = -Nn;
    }
    float arg = 1.0f - (neta * neta * (1.0f - cosi * cosi));
    if (arg >= 0) {
        float dnp = sqrt(arg);
        float nK = (neta * cosi) - dnp;
        T = In * neta + Nn * nK;
        return 1 - fresnel_dielectric(cosi, eta);
    }
    T = V3f(0.f,0.f,0.f);
    return 0;
}

bool Lambertian::scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered, Sampler & sampler)
{
    // trace an other ray
    V3f target = rec.position + rec.normal + sampler.sampleUnitSphere();
    ray_scattered = Ray(rec.position, target - rec.position);
    ray_scattered.depth = r.depth+1;
    attenuation = m_albedo;
    return true;
}

//Sampler SimpleMetal::s_sampler = Sampler(212);

bool SimpleMetal::scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered, Sampler & sampler)
{
    V3f jitter = V3f(0.f,0.f,0.f);
    if (m_roughness>0.f) jitter = sampler.sampleUnitSphere()*m_roughness;
    V3f reflected = reflect(r.direction.normalized(),rec.normal);
    ray_scattered = Ray(rec.position, reflected+jitter);
    attenuation = m_albedo;
    ray_scattered.depth = r.depth+1;
    return true;
}

//Sampler Glass::s_sampler = Sampler(2112);

bool Glass::scatter(const Ray & r, const HitRecord & rec, C3f & attenuation, Ray & ray_scattered, Sampler & sampler)
{
    V3f refracted;
   
    float fresnel = fresnel_refraction(r.direction, rec.normal, m_refraction_index, refracted);
    
    attenuation =  V3f(1.f,1.f,1.f);
    V3f reflected = reflect(r.direction.normalized(),rec.normal);
    
    float random_value = sampler.random();
    
    V3f scattered = refracted;
    if (random_value > fresnel) scattered = reflected;
    
    ray_scattered = Ray(rec.position, scattered);
    ray_scattered.depth = r.depth+1;
    return true;
}
