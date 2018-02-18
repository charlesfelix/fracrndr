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
        Material * material; // do not use Material::Ptr to avoid many shared_ptr copies, using a plain pointer saved 10pct runtime
    };
    
    class RenderPrimitve{
        
    public:
        DEF_SHARED_PTR_TYPES(RenderPrimitve);

        virtual ~RenderPrimitve(){};
        virtual void setMaterial(const std::shared_ptr<Material> & material) {};
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const = 0;
    };
    
    class PrimitiveList : public RenderPrimitve
    {
    public:
        DEF_SHARED_PTR_TYPES(PrimitiveList);

        virtual ~PrimitiveList(){};
        PrimitiveList & addPrimitive(RenderPrimitve::Ptr & primitive);
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const;
    private:
        std::vector<RenderPrimitve::Ptr> m_primitives;
    };
    
}; // namespace Fr
#endif /* Primitive_hpp */
