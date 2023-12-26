//
//  Scene.cpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Scene.hpp"
#include "TriMesh.hpp"
#include "BVH.hpp"
#include "easylogging++.h"

using namespace Fr;

Scene::Scene()
{

}

const Background::ConstPtr & Scene::getBackground() const
{
    return m_background;
}

const RenderPrimitive::ConstPtr & Scene::getPrimitives() const
{
    return m_accelerator;
}

const Box3d & Scene::getBounds() const
{
    return m_bounds;
}

bool Scene::hit(const Ray & ray, HitRecord & hit_record) const
{
    return m_primitives->hit(ray,0,MAXFLOAT,hit_record);
}

void Scene::setPrimitives(RenderPrimitive::ConstPtr primitives)
{
    // because we're using shared pointers we don't want to loose ownership
    // TODO: poor design, fix it.
    m_primitives_owner = primitives;
    
    std::vector<RenderPrimitive::ConstPtr> prims;
    primitives->getSubPrimitives(prims);
    
    PrimitiveList::Ptr list = std::make_shared<PrimitiveList>();
    
    for (auto p: prims)
    {
        RenderPrimitive::Ptr _p = std::const_pointer_cast<RenderPrimitive>(p);
        list->addPrimitive(_p);
    }

    RenderPrimitive::ConstPtr root = std::make_shared<BVHNode>(prims, 0, prims.size() - 1);

    m_primitives = root;
    m_accelerator = root;
}

void Scene::setBackground(Background::ConstPtr background)
{
    m_background = background;
}
