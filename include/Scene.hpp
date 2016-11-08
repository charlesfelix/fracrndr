//
//  Scene.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "Fr.hpp"
#include "Primitive.hpp"
#include "Background.hpp"


//TODO: Change the hit function naming
//TODO: add hit predicate
//TODO: implement getBounds on the primitive class
namespace Fr
{
    class Scene
    {
       public:
        DEF_SHARED_PTR_TYPES(Scene);
        
        Scene();
        virtual ~Scene() {};
        
        Primitive::ConstPtr getPrimitives() const;
        
        Background::ConstPtr getBackground() const;
        
        const Fr::Box3d & getBounds() const;
        
        bool hit(const Ray & ray, HitRecord & hit_record) const;
        
        void setPrimitives(const Primitive::Ptr & primitives);
        
        void setBackground(const Background::Ptr & background);
        
    private:
        Scene(const Scene & );
        Primitive::Ptr m_primitives;
        Background::Ptr m_background;
        Fr::Box3d m_bounds;
        

    };
}; // namespace

#endif /* Scene_h */
