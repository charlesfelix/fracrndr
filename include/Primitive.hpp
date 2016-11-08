//
//  Primitive.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/22/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Primitive_hpp
#define Primitive_hpp
#include "Fr.hpp"
#include "Ray.hpp"
//#include "Material.hpp"
#include <vector>

namespace Fr{

    class Material; // forward decl.
    
    struct HitRecord {

        HitRecord():t(MAXFLOAT){}
        HitRecord(const HitRecord & hr):t(hr.t),position(hr.position),normal(hr.normal),color(hr.color){}
        float t;
        V3f position;
        V3f normal;
        C4f color;
        std::shared_ptr<Material> material; // do not use Material::Ptr to avoid cyclic includes
    };
    
    class Primitive{
        
    public:
        DEF_SHARED_PTR_TYPES(Primitive);

        virtual ~Primitive(){};
        virtual void setMaterial(const std::shared_ptr<Material> & material) {};
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const = 0;
    };
    
    class PrimitiveList : public Primitive
    {
    public:
        DEF_SHARED_PTR_TYPES(PrimitiveList);

        virtual ~PrimitiveList(){};
        PrimitiveList & addPrimitive(Primitive::Ptr & primitive);
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const;
    private:
        std::vector<Primitive::Ptr> m_primitives;
    };
    
}; // namespace Fr
#endif /* Primitive_hpp */
