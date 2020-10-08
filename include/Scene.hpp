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
#include "RenderPrimitive.hpp"
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
        
        const RenderPrimitive::ConstPtr & getPrimitives() const;
        
        const Background::ConstPtr & getBackground() const;
        
        const Fr::Box3d & getBounds() const;
        
        bool hit(const Ray & ray, HitRecord & hit_record) const;
        
        void setPrimitives(RenderPrimitive::ConstPtr primitives);
        
        void setBackground(Background::ConstPtr background);
        
    private:
        Scene(const Scene & );
        RenderPrimitive::ConstPtr m_primitives;
        RenderPrimitive::ConstPtr m_accelerator;
        Background::ConstPtr m_background;
        Fr::Box3d m_bounds;
        

    };
}; // namespace

#endif /* Scene_h */
