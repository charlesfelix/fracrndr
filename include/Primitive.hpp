//
//  Primitive.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Primitive_hpp
#define Primitive_hpp
#include "frMath.h"
#include "Ray.hpp"
#include <vector>
namespace Fr{
    
    class Material;
    
    struct HitRecord {
        HitRecord():t(MAXFLOAT){}
        HitRecord(const HitRecord & hr):t(hr.t),position(hr.position),normal(hr.normal),color(hr.color){}
        float t;
        V3f position;
        V3f normal;
        C4f color;
        std::shared_ptr<Material> material;
    };
    
    class Primitive{
        
    public:
        virtual ~Primitive(){};
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const = 0;
    };
    
    class PrimitiveList : public Primitive
    {
    public:
        virtual ~PrimitiveList(){};
        PrimitiveList & addPrimitive(std::shared_ptr<Primitive> & primitive);
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const;
    private:
        std::vector<std::shared_ptr<Primitive>> m_primitives;
    };
    
}; // namespace Fr
#endif /* Primitive_hpp */
