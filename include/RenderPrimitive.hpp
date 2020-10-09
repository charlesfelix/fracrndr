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
        Real t;
        V3f position;
        V3f normal;
        C4f color;
        Material * material; // do not use Material::Ptr to avoid many shared_ptr copies, using a plain pointer saved 10pct runtime
    };
    
    class RenderPrimitive{
        
    public:
        DEF_SHARED_PTR_TYPES(RenderPrimitive);

        virtual ~RenderPrimitive(){};
        virtual void setMaterial(const std::shared_ptr<Material> & material) {};
        virtual bool hit(const Ray & r, Real tmin, Real tmax, HitRecord & hit_record) const = 0;
        virtual bool isAggregate() const {return false;};
        virtual const Box3f & getBounds() const =0;
        virtual bool getSubPrimitives(std::vector<RenderPrimitive::ConstPtr> & subprims) const {return false;};

    };
    
    class PrimitiveList : public RenderPrimitive
    {
    public:
        DEF_SHARED_PTR_TYPES(PrimitiveList);

        virtual ~PrimitiveList(){};
        PrimitiveList & addPrimitive(RenderPrimitive::Ptr & primitive);
        virtual bool hit(const Ray & r, Real tmin, Real tmax, HitRecord & hit_record) const;
        virtual bool isAggregate() const {return true;};
        virtual const Box3f & getBounds() const;
        virtual bool getSubPrimitives(std::vector<RenderPrimitive::ConstPtr> & subprims) const;

    private:
        std::vector<RenderPrimitive::Ptr> m_primitives;
        Box3f m_bounds;
    };
    
}; // namespace Fr
#endif /* Primitive_hpp */
