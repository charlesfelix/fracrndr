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




TriangleMesh::Triangle::Triangle(const TriangleMesh * mesh, unsigned int i0, unsigned int i1, unsigned int i2)
{
    m_indices[0]=i0;
    m_indices[1]=i1;
    m_indices[2]=i2;
    m_mesh = mesh;
}


const V3f & TriangleMesh::Triangle::position(unsigned int pt) const
{
    assert(pt < 3);
    return m_mesh->m_positions[m_indices[pt]];
}

V3f TriangleMesh::Triangle::position(unsigned int pt)
{
    assert(pt < 3);
    return m_mesh->m_positions[m_indices[pt]];
}

V3f TriangleMesh::Triangle::normal(unsigned int pt)
{
    assert(pt < 3);
    return m_mesh->m_normals[m_indices[pt]];
}


TriangleMesh::TriangleMesh( const std::vector<V3f> & positions, const std::vector<V3f> & uvs, const std::vector<V3f> &normals, const std::vector<unsigned> & triangles )
{
    m_positions = positions;
    m_normals = normals;
    m_uvs = uvs;
    m_triangles = triangles;
}

TriangleMesh::Triangle TriangleMesh::triangle(unsigned idx) const
{
    return TriangleMesh::Triangle(this,
                                  m_triangles[idx*3],
                                  m_triangles[idx*3+1],
                                  m_triangles[idx*3+2]);
}

void TriangleMesh::recomputeNormals()
{
    m_normals.clear();
    m_normals.resize(numTriangles());
    const size_t n = numTriangles();
    for (unsigned i = 0; i < n; ++i)
    {
        TriangleMesh::Triangle tri = triangle(i);
        V3f c = (tri.position(1)-tri.position(0)).cross(tri.position(2)-tri.position(0));
        c = c.normalize();
        m_normals[i]=c;
    }
}

bool TriangleMesh::hit(const Fr::Ray &r, float tmin, float tmax, Fr::HitRecord &hit_record) const
{
    const size_t ntris = numTriangles();
    float closest_t, closest_beta, closest_gamma;
    V3f closest_normal;
    closest_t = FLT_MAX;
    
    bool has_hit = false;
    
    for (unsigned i = 0; i < ntris; ++i)
    {
        float t, beta, gamma;
        TriangleMesh::Triangle tri = triangle(i);
        bool intersect = intersectTriangle(r, tri.position(0), tri.position(1), tri.position(2), r.mint, r.maxt, t, beta, gamma);
        
        has_hit = has_hit || intersect;
        
        if (intersect && (t < closest_t))
        {
            closest_t = t;
            closest_beta = beta;
            closest_gamma = gamma;
            closest_normal = m_normals[i]; // compute barycentric interpolation
        }
        
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
