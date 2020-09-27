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

namespace Fr {
    

    inline bool
    intersectTriangle(Ray r, const V3f& p0, const V3f& p1, const V3f& p2,
                      float tmin, float tmax, float& t, float& beta, float& gamma)
    {
        float A = p0[0] - p1[0];
        float B = p0[1] - p1[1];
        float C = p0[2] - p1[2];
        
        float D = p0[0] - p2[0];
        float E = p0[1] - p2[1];
        float F = p0[2] - p2[2];
        
        
        float G = r.direction[0];
        float H = r.direction[1];
        float I = r.direction[2];
        
        float J = p0[0] - r.origin[0];
        float K = p0[1] - r.origin[1];
        float L = p0[2] - r.origin[2];
        
        float EIHF = E*I-H*F;
        float GFDI = G*F-D*I;
        float DHEG = D*H-E*G;
        
        float denom = (A*EIHF + B*GFDI + C*DHEG);
        
        beta = (J*EIHF + K*GFDI + L*DHEG) / denom;
        
        if (beta <= 0.0 || beta >= 1) return false;
        
        float AKJB = A*K - J*B;
        float JCAL = J*C - A*L;
        float BLKC = B*L - K*C;
        
        gamma = (I*AKJB + H*JCAL + G*BLKC)/denom;
        if (gamma <= 0.0 || beta + gamma >= 1.0) return false;
        
        t =  -(F*AKJB + E*JCAL + D*BLKC)/denom;
        return (t >= tmin && t <= tmax);
    }
    
    class TriangleShape{
    public:
        V3f points[3];
    };
    
    class TriangleMesh : public RenderPrimitive
    {
    public:
        class Triangle
        {
        public: // member functions
            Triangle(const TriangleMesh * mesh, unsigned i0, unsigned i1, unsigned i2);
            const V3f & position(unsigned int pt) const;
            V3f position(unsigned int pt);
            V3f normal(unsigned int pt);
            
            
        public: // member variables
            unsigned m_indices[3]; // vertex indices in the mesh position array
            const TriangleMesh * m_mesh;
        };

    public:
        TriangleMesh(const std::vector<V3f> & positions, const std::vector<V3f> & uvs, const std::vector<V3f> &normals, const std::vector<unsigned> & triangles);
        
        size_t numTriangles() const { return m_triangles.size()/3;};
        Triangle triangle(unsigned idx) const;
        void recomputeNormals();
                    
        virtual bool hit(const Fr::Ray &r, float tmin, float tmax, Fr::HitRecord &hit_record) const;
        virtual void setMaterial(const std::shared_ptr<Material> & material);
        virtual const Box3f & getBounds() const;
        
    private:
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
