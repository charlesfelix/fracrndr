//
//  IDF.hpp
//  FracRndr
//
//  Created by Charles-Felix on 5/28/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef IDF_hpp
#define IDF_hpp

#include "Primitive.hpp"

namespace Fr
{
    
    class SphereDE {
    public:
        
        SphereDE(const V3f & position, float radius, float distance_threshold=0.0001):
        m_position(position),m_radius(radius),m_distance_threshold(distance_threshold)
        {
        }
        
        float estimate(const V3f & position) const
        {
           return (position-m_position).length()-m_radius;
        }
        
        float distanceThreshold() const { return m_distance_threshold;}
        
    private:
        V3f m_position;
        float m_radius;
        float m_distance_threshold;
    };
    
    template <typename DistanceEstimator>
    class ImplicitDistanceField : public Primitive
    {
    public:
        ImplicitDistanceField(const DistanceEstimator & estimator,unsigned max_steps, std::shared_ptr<Material> material) : m_estimator(estimator),m_max_steps(max_steps), m_material(material){};
        
        virtual ~ImplicitDistanceField(){};
        
        virtual bool hit(const Ray & r, float tmin, float tmax, HitRecord & hit_record) const
        {
            V3f ray_dir = r.direction.normalizedExc();
            V3f ray_pos = r.origin;
            bool has_hit = false;
            const float threshold = m_estimator.distanceThreshold();
            unsigned num_steps = 0;
            
            while (!has_hit && num_steps++ < m_max_steps)
            {
                float de = m_estimator.estimate(ray_pos);
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
            const float deps = 0.001;
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
        std::shared_ptr<Material> m_material;
        
    };
    
};
#endif /* IDF_hpp */
