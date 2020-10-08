//
//  BVH.hpp
//  FracRndr
//
//  Created by Charles-Felix on 4/20/18.
//  Copyright © 2018 Charles-Felix. All rights reserved.
//

#ifndef BVH_h
#define BVH_h

#include "Fr.hpp"
#include "RenderPrimitive.hpp"
#include "Material.hpp"

namespace Fr
{

    class BVHNode : public RenderPrimitive
    {
    public:
        DEF_SHARED_PTR_TYPES(BVHNode);
        
        BVHNode(std::vector<RenderPrimitive::ConstPtr> & objects, size_t start, size_t end);

        virtual ~BVHNode() {};
        
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const;
        virtual bool isAggregate() const {return true;};
        virtual const Box3f & getBounds() const;
        
        void getAllBounds(std::vector<Box3f> & bounds) const;
        
    private:
        BVHNode();
        BVHNode(const BVHNode &);
        
        RenderPrimitive::ConstPtr m_left;
        RenderPrimitive::ConstPtr m_right;

        Box3f m_bounds;
    };
};

#endif /* BVH_h */
