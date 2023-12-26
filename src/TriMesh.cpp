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

bool TriangleMesh::Triangle::hit(const Fr::Ray &r, Real tmin, Real tmax, Fr::HitRecord &hit_record) const
{
    Real t, beta, gamma;

    bool has_hit = intersectTriangle(r,
                                     m_mesh->position(m_idx,0),
                                     m_mesh->position(m_idx,1),
                                     m_mesh->position(m_idx,2),
                                     r.mint, r.maxt, t, beta, gamma);
    
    if (has_hit)
    {
        hit_record.t = t;
        hit_record.position = r.positionAt(t);
        hit_record.material = m_mesh->m_material.get();
        assert(hit_record.material != nullptr);
        
        // interp normals
        V3f n0 = m_mesh->normal(m_idx,0);
        V3f n1 = m_mesh->normal(m_idx,1);
        V3f n2 = m_mesh->normal(m_idx,2);
        
        hit_record.normal = beta * n1 + gamma * n2 + (1.0 - beta - gamma)*n0;
    }
    return has_hit;
}

const Box3f & TriangleMesh::Triangle::getBounds() const
{
    return m_bounds;
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

TriangleMesh::~TriangleMesh()
{
}

TriangleMesh::Triangle TriangleMesh::triangle(size_t idx) const
{
    return TriangleMesh::Triangle(this,idx);
}

const V3f & TriangleMesh::position(size_t triangle_idx, unsigned int vertex_idx) const{
    return m_positions[m_triangles[triangle_idx*3 + vertex_idx]];
}


const V3f & TriangleMesh::normal(size_t triangle_idx, unsigned int vertex_idx) const{
    assert(vertex_idx < 3);
    assert(triangle_idx*3 + vertex_idx < m_triangles.size());
    return m_normals[m_triangles[triangle_idx*3 + vertex_idx]];
}


void TriangleMesh::recomputeNormals()
{
    m_normals.clear();
    const size_t num_tris = numTriangles();
    m_normals.resize( m_positions.size(), V3f(0));
    std::vector<size_t> counts;
    
    counts.resize( m_positions.size() , 0);
    
    for (size_t tri_idx = 0; tri_idx < num_tris; ++tri_idx)
    {
        V3f n = (position(tri_idx,1)-position(tri_idx,0)).cross(position(tri_idx,2)-position(tri_idx,0));
        
        n = n.normalize();
        
        const size_t offset = tri_idx*3;
        
        m_normals[m_triangles[offset]] += n;
        counts[m_triangles[offset]]++;
        
        m_normals[m_triangles[offset+1]] += n;
        counts[m_triangles[offset+1]]++;
        
        m_normals[m_triangles[offset+2]] += n;
        counts[m_triangles[offset+2]]++;
    }
    
    auto n_it = m_normals.begin();
    auto c_it = counts.begin();
    for ( ; n_it != m_normals.end() ; ++n_it, ++c_it)
    {
        *n_it /= *c_it;
    }
}

bool TriangleMesh::hit(const Fr::Ray &r, Real tmin, Real tmax, Fr::HitRecord &hit_record) const
{
    const size_t ntris = numTriangles();
    Real closest_t, closest_beta = 1.0, closest_gamma = 1.0;
    V3f closest_normal;
    closest_t = FLT_MAX;
    
    bool has_hit = false;

    size_t closest_tri = 0;
    for (unsigned i = 0; i < ntris; ++i)
    {
        Real t, beta, gamma;
        
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
        hit_record.material = this->m_material.get();

        // interp normals
        V3f n0 = this->normal(closest_tri,0);
        V3f n1 = this->normal(closest_tri,1);
        V3f n2 = this->normal(closest_tri,2);
        
        hit_record.normal = closest_beta * n1 + closest_gamma * n2 + (1.0 - closest_beta - closest_gamma)*n0;
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
