//
//  TriMesh.hpp
//  FracRndr
//
//  Created by Charles-Felix on 4/17/18.
//  Copyright © 2018 Charles-Felix. All rights reserved.
//

#ifndef __FRTRIMESH_HPP__
#define __FRTRIMESH_HPP__


#include "Fr.hpp"
#include "RenderPrimitive.hpp"
#include "Material.hpp"
#include <vector>

// thanks to pshirley
// http://www.cs.utah.edu/~shirley/galileo/MeshTriangle.h
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
namespace Fr {
    
    inline bool
    intersectTriangle(Ray r, const V3f& p0, const V3f& p1, const V3f& p2,
                      Real tmin, Real tmax, Real& t, Real& beta, Real& gamma)
    {
        
        float u, v = 0.f;
        V3f v0v1 = p1 - p0;
        V3f v0v2 = p2 - p0;
        V3f pvec = r.direction.cross(v0v2);
        float det = v0v1.dot(pvec);
        
    #define CULLING 0
    #ifdef CULLING
        // if the determinant is negative the triangle is backfacing
        // if the determinant is close to 0, the ray misses the triangle
        if (det < EPSILON) return false;
    #else
        // ray and triangle are parallel if det is close to 0
        if (fabs(det) < EPSILON) return false;
    #endif
        float invDet = 1 / det;
     
        V3f tvec = r.origin - p0;
        u = tvec.dot(pvec) * invDet;
        if (u < 0 || u > 1) return false;
     
        V3f qvec = tvec.cross(v0v1);
        v = r.direction.dot(qvec) * invDet;
        if (v < 0 || u + v > 1) return false;
     
        t = v0v2.dot(qvec) * invDet;
    
        beta = u;
        gamma = v;
    
        return (t >= tmin && t <= tmax);
    }

    class TriangleMesh : public RenderPrimitive
    {
    public:
        DEF_SHARED_PTR_TYPES(TriangleMesh);

        
        class Triangle : public RenderPrimitive
        {
        public: // member functions
            Triangle(const TriangleMesh * mesh, size_t idx);
            const V3f & position(unsigned pt) const;
            const V3f & normal(unsigned pt) const;
            
            
        public: // member variables
            size_t m_idx; // triangle index in the mesh
            const TriangleMesh * m_mesh;
        private:
            virtual bool hit(const Fr::Ray &r, Real tmin, Real tmax, Fr::HitRecord &hit_record) const;
            virtual void setMaterial(const std::shared_ptr<Material> & material);
            virtual const Box3f & getBounds() const;
            
            Box3f m_bounds;

        };
        TriangleMesh(const std::vector<V3f> & positions, const std::vector<V3f> & uvs, const std::vector<V3f> &normals, const std::vector<unsigned> & triangles);
        
        virtual ~TriangleMesh();
        
        size_t numTriangles() const { return m_triangles.size()/3;};
        Triangle triangle(size_t idx) const;
        void recomputeNormals();
        
        const V3f & normal(size_t triangle_idx, unsigned vertex_idx) const;
        const V3f & position(size_t triangle_idx, unsigned vertex_idx) const;

        virtual bool hit(const Fr::Ray &r, Real tmin, Real tmax, Fr::HitRecord &hit_record) const;
        virtual void setMaterial(const std::shared_ptr<Material> & material);
        virtual const Box3f & getBounds() const;
        virtual bool isAggregate() const { return true; };
        virtual bool getSubPrimitives(std::vector<RenderPrimitive::ConstPtr> & subprims) const; // return true if has
        
    public:
        std::vector<V3f> m_positions;
        std::vector<V3f> m_normals;
        std::vector<V3f> m_triangle_normals;
        std::vector<V3f> m_uvs;
        std::vector<unsigned> m_triangles;
        Box3f m_bounds;
        Material::Ptr m_material;

    };
    
};
#endif /* TriMesh_h */
