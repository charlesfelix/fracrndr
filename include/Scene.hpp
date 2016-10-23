//
//  Scene.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include "frMath.h"
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
        Scene();
        virtual ~Scene() {};
        
        std::shared_ptr<const Primitive> getPrimitives() const;
        
        std::shared_ptr<const Background> getBackground() const;
        
        const Fr::Box3d & getBounds() const;
        
        bool hit(const Ray & ray, HitRecord & hit_record) const;
        
        void setPrimitives(const std::shared_ptr<Primitive> & primitives);
        
        void setBackground(const std::shared_ptr<Background> & background);
        
    private:
        Scene(const Scene & );
        std::shared_ptr<Primitive> m_primitives;
        std::shared_ptr<Background> m_background;
        Fr::Box3d m_bounds;
        

    };
}; // namespace

#endif /* Scene_h */
