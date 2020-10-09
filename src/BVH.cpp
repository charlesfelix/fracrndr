//
//  BVH.cpp
//  FracRndr
//
//  Created by Charles-Felix on 4/21/18.
//  Copyright © 2018 Charles-Felix. All rights reserved.
//

//https://blog.frogslayer.com/kd-trees-for-faster-ray-tracing-with-triangles/
//https://raytracing.github.io/books/RayTracingTheNextWeek.html#boundingvolumehierarchies/thebvhnodeclass

#include <BVH.hpp>

using namespace Fr;

inline bool box_compare(const RenderPrimitive::ConstPtr a, const RenderPrimitive::ConstPtr b, int axis) {
    Box3f box_a = a->getBounds();
    Box3f box_b = b->getBounds();

    return box_a.min[axis] < box_b.min[axis];
}


bool box_x_compare (const RenderPrimitive::ConstPtr a, const RenderPrimitive::ConstPtr b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const RenderPrimitive::ConstPtr a, const RenderPrimitive::ConstPtr b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const RenderPrimitive::ConstPtr a, const RenderPrimitive::ConstPtr b) {
    return box_compare(a, b, 2);
}

bool BVHNode::hit(const Ray & r, Real tmin, Real tmax, HitRecord & hit_record) const
{
    const bool test = intersectAABB(m_bounds, r.origin, r.direction, tmin, tmax);
    if (test)
    {
        const bool hit_left = m_left->hit(r, tmin, tmax, hit_record);
        if (m_right) // if has 2 children
        {
            const bool hit_right = m_right->hit(r, tmin, hit_left ? hit_record.t : tmax, hit_record);
            return hit_left || hit_right;
        }
        return hit_left;
    }
    return false;
}

const Box3f & BVHNode::getBounds() const { return m_bounds; }

BVHNode::BVHNode(std::vector<RenderPrimitive::ConstPtr>& objects, size_t start, size_t end) {
    int axis = randomInt(0,2);
    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1)
    {
        m_left = objects[start];
        m_right = nullptr;
    }
    else if (object_span == 2)
    {
        if (comparator(objects[start], objects[start+1]))
        {
            m_left = objects[start];
            m_right = objects[start+1];
        }
        else
        {
            m_left = objects[start+1];
            m_right = objects[start];
        }
    }
    else
    {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span/2;
        m_left  = std::make_shared<BVHNode>(objects, start, mid);
        m_right = std::make_shared<BVHNode>(objects, mid, end);
    }

    Box3f box_left = m_left->getBounds();
    m_bounds = box_left;
    
    if (m_right)
    {
        Box3f box_right = m_right->getBounds();
        m_bounds.extendBy(box_right);
    }
}

void BVHNode::getAllBounds(std::vector<Box3f> & bounds) const
{
    std::vector<Box3f> left_bounds;
    const BVHNode * nleft = dynamic_cast<const BVHNode*>(m_left.get());
    if (nleft)
    {
        nleft->getAllBounds(left_bounds);
    }
    else
    {
        left_bounds.push_back(m_left->getBounds());
    }
    bounds.insert(bounds.end(), left_bounds.begin(), left_bounds.end());

    if (m_right)
    {
        std::vector<Box3f> right_bounds;
        const BVHNode * nright = dynamic_cast<const BVHNode*>(m_right.get());
        if (nright)
        {
            nright->getAllBounds(right_bounds);
        }
        else
        {
            right_bounds.push_back(m_right->getBounds());
        }
        bounds.insert(bounds.end(), right_bounds.begin(), right_bounds.end());

    }
}

