//
//  IDF.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/28/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef IDF_hpp
#define IDF_hpp

#include "RenderPrimitive.hpp"
#include "Material.hpp"

namespace Fr
{
    
    class SphereDE {
    public:
        
        SphereDE(const V3f & position, Real radius, Real distance_threshold=0.0001):
        m_position(position),m_radius(radius),m_distance_threshold(distance_threshold)
        {
        }
        
        Real estimate(const V3f & position) const
        {
           return (position-m_position).length()-m_radius;
        }
        
        Real distanceThreshold() const { return m_distance_threshold;}
        
    private:
        V3f m_position;
        Real m_radius;
        Real m_distance_threshold;
    };
    
    template <typename DistanceEstimator>
    class ImplicitDistanceField : public RenderPrimitive
    {
    public:
        ImplicitDistanceField(const DistanceEstimator & estimator,unsigned max_steps) : m_estimator(estimator),m_max_steps(max_steps), m_material(nullptr){};
        
        virtual ~ImplicitDistanceField(){};
        
        void setMaterial(const std::shared_ptr<Material> & material)
        {
            m_material = material;
        }
        virtual bool hit(const Ray & r, Real tmin, Real tmax, HitRecord & hit_record) const
        {
            V3f ray_dir = r.direction.normalized();
            V3f ray_pos = r.origin;
            bool has_hit = false;
            const Real threshold = m_estimator.distanceThreshold();
            unsigned num_steps = 0;
            
            while (!has_hit && num_steps++ < m_max_steps)
            {
                Real de = m_estimator.estimate(ray_pos);
                if (de< threshold)
                    has_hit = true;
                ray_pos = ray_pos + de*ray_dir;
            }
            
            if (!has_hit) return false;
            
            hit_record.t = (r.origin-ray_pos).length();
            hit_record.position = ray_pos;
            hit_record.normal = computeNormal(ray_pos);
            hit_record.material = m_material;
            
            if (hit_record.t < threshold*10.f) return false;
            return true;
        }

        V3f computeNormal(const V3f & position) const
        {
            const Real deps = 0.001;
            const V3f dx(deps,0.f,0.f);
            const V3f dy(0.f,deps,0.f);
            const V3f dz(0.f,0.f,deps);
            
            V3f n = V3f(m_estimator.estimate(position+dx)-m_estimator.estimate(position-dx),
                        m_estimator.estimate(position+dy)-m_estimator.estimate(position-dy),
                        m_estimator.estimate(position+dz)-m_estimator.estimate(position-dz));
            n.normalize();
            return n;

        }
        
        
    private:
        DistanceEstimator m_estimator;
        unsigned m_max_steps;
        Material::Ptr m_material;
        
    };
    
};
#endif /* IDF_hpp */
