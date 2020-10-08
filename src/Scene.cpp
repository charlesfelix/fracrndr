//
//  Scene.cpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Scene.hpp"
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
    // TODO: create the BVH from all the primitives in the primitive lists
    std::vector<RenderPrimitive::ConstPtr> prims;
    primitives->getSubPrimitives(prims);
    
    // make BVH
    PrimitiveList::Ptr list = std::make_shared<PrimitiveList>();
    
    for (auto p: prims)
    {
        RenderPrimitive::Ptr _p = std::const_pointer_cast<RenderPrimitive>(p);
        list->addPrimitive(_p);
    }
    
    RenderPrimitive::ConstPtr root = std::make_shared<BVHNode>(prims,0,prims.size());

    //m_primitives = root;
    m_primitives = primitives;
    m_accelerator = root;

    // dump all bounds
    std::vector<Box3f> all_bounds;
    dynamic_cast<const BVHNode*>(root.get())->getAllBounds(all_bounds);
    std::string fileout = "/Users/charles-felix/bvh.csv";
    std::ofstream fout (fileout.c_str() );
    fout << "minx,miny,minz,maxx,maxy,maxz " << std::endl;
    for (auto b: all_bounds){
        fout << b.min.x << "," << b.min.y << "," << b.min.z << "," << b.max.x << "," << b.max.y << "," << b.max.z  << std::endl;
    }

}

void Scene::setBackground(Background::ConstPtr background)
{
    m_background = background;
}
