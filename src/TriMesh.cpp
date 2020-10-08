//
//  TriMesh.cpp
//  FracRndr
//
//  Created by Charles-Felix on 6/25/18.
//  Copyright © 2018 Charles-Felix. All rights reserved.
//

#include "Fr.hpp"
#include "TriMesh.hpp"


using namespace Fr;




TriangleMesh::Triangle::Triangle(const TriangleMesh * mesh, size_t idx):m_mesh(mesh),m_idx(idx)
{
    m_idx = idx;
    m_mesh = mesh;

    m_bounds = Box3f();
    m_bounds.extendBy(position(0));
    m_bounds.extendBy(position(1));
    m_bounds.extendBy(position(2));
}


const V3f & TriangleMesh::Triangle::position(unsigned int pt) const
{
    assert(pt < 3);
    return m_mesh->position(m_idx,pt);
}


const V3f & TriangleMesh::Triangle::normal(unsigned int pt) const
{
    assert(pt < 3);
    return m_mesh->normal(m_idx,pt);
}

bool TriangleMesh::Triangle::hit(const Fr::Ray &r, float tmin, float tmax, Fr::HitRecord &hit_record) const
{
    float t, beta, gamma;

    bool has_hit = intersectTriangle(r,
                                     m_mesh->position(m_idx,0),
                                     m_mesh->position(m_idx,1),
                                     m_mesh->position(m_idx,2),
                                     r.mint, r.maxt, t, beta, gamma);
    
    if (has_hit)
    {
        hit_record.t = t;
        hit_record.position = r.positionAt(t);
        hit_record.normal = m_mesh->normal(m_idx,0); // TODO: compute barycentric interpolation
        hit_record.material = m_mesh->m_material.get();
    }
    return has_hit;
}

const Box3f & TriangleMesh::Triangle::getBounds() const
{
    return m_bounds;
}

TriangleMesh::~TriangleMesh()
{
    return;
}

// TODO: bad design figure out a way to make this better.
void TriangleMesh::Triangle::setMaterial(const std::shared_ptr<Material> &material)
{
    //m_material = material;
}


TriangleMesh::TriangleMesh( const std::vector<V3f> & positions, const std::vector<V3f> & uvs, const std::vector<V3f> &normals, const std::vector<unsigned> & triangles )
{
    m_positions = positions;
    m_normals = normals;
    m_uvs = uvs;
    m_triangles = triangles;
}

TriangleMesh::Triangle TriangleMesh::triangle(size_t idx) const
{
    return TriangleMesh::Triangle(this,idx);
}

const V3f & TriangleMesh::position(size_t triangle_idx, unsigned int vertex_idx) const{
    return m_positions[m_triangles[triangle_idx*3 + vertex_idx]];
}


const V3f & TriangleMesh::normal(size_t triangle_idx, unsigned int vertex_idx) const{
    return m_normals[triangle_idx*3 + vertex_idx];
}


void TriangleMesh::recomputeNormals()
{
    m_normals.clear();
    m_normals.resize(numTriangles()*3);
    const size_t n = numTriangles();
    for (size_t i = 0; i < n; ++i)
    {
        V3f c = (position(i,1)-position(i,0)).cross(position(i,2)-position(i,0));
        c = c.normalize();
        m_normals[i*3]=m_normals[i*3+1]=m_normals[i*3+2]=c;
    }
}

bool TriangleMesh::hit(const Fr::Ray &r, float tmin, float tmax, Fr::HitRecord &hit_record) const
{
    const size_t ntris = numTriangles();
    float closest_t, closest_beta, closest_gamma;
    V3f closest_normal;
    closest_t = FLT_MAX;
    
    bool has_hit = false;

    size_t closest_tri = 0;
    for (unsigned i = 0; i < ntris; ++i)
    {
        float t, beta, gamma;
        bool intersect = intersectTriangle(r, position(i,0), position(i,1), position(i,2), r.mint, r.maxt, t, beta, gamma);
        
        has_hit = has_hit || intersect;
        
        if (intersect && (t < closest_t))
        {
            closest_t = t;
            closest_beta = beta;
            closest_gamma = gamma;
            closest_tri = i;
        }
        
    }
    
    if (has_hit)
    {
        closest_normal = normal(closest_tri,0); // compute barycentric interpolation
        hit_record.t = closest_t;
        hit_record.position = r.positionAt(closest_t);
        hit_record.normal = closest_normal;
        hit_record.material = this->m_material.get();
    }
    
    return has_hit;
}

void TriangleMesh::setMaterial(const std::shared_ptr<Material> &material)
{
    m_material = material;
}


const Box3f & TriangleMesh::getBounds() const
{
    return m_bounds;
}

bool TriangleMesh::getSubPrimitives(std::vector<RenderPrimitive::ConstPtr> & subprims) const
{
    const size_t ntris = numTriangles();
    subprims = std::vector<RenderPrimitive::ConstPtr>(ntris);
    for (size_t i = 0; i < ntris; ++i)
    {
        TriangleMesh::Triangle tri = triangle(i);
        subprims[i] = RenderPrimitive::ConstPtr(new TriangleMesh::Triangle(this, i));
    }
    return true;
}
