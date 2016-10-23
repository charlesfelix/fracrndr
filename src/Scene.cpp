//
//  Scene.cpp
//  FracRndr
//
//  Created by Charles-Felix on 10/15/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Scene.hpp"

using namespace Fr;

Scene::Scene()
{

}

std::shared_ptr<const Background> Scene::getBackground() const
{
    return m_background;
}

std::shared_ptr<const Primitive> Scene::getPrimitives() const
{
    return m_primitives;
}

const Box3d & Scene::getBounds() const
{
    return m_bounds;
}

bool Scene::hit(const Ray & ray, HitRecord & hit_record) const
{
    return m_primitives->hit(ray,0,MAXFLOAT,hit_record);
}

void Scene::setPrimitives(const std::shared_ptr<Primitive> & primitives)
{
    m_primitives = primitives;
}

void Scene::setBackground(const std::shared_ptr<Background> & background)
{
    m_background = background;
}
