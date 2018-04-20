//
//  Camera.h
//  FracRndr
//
//  Created by Charles-Felix on 4/28/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef fr_Camera_h
#define fr_Camera_h

#include "Fr.hpp"
#include "Ray.hpp"

namespace Fr{
    
    class PerspectiveProjection
    {
    public:
        PerspectiveProjection();
    };
    
    
    
    class Camera
    {
    public:
        DEF_SHARED_PTR_TYPES(Camera);
        
        Camera(float focal = 50.0f, float aperture = 42.4214f, float aspect_ratio = 1.f, float near = 0.0001, float far = 100000000.f);
        
        
        V3f worldToCamera(const V3f & wpos) const;
        V3f cameraToWorld(const V3f & wpos) const;
        V2f cameraToNdc(const V3f & cpos) const;
        V3f ndcToCamera(const V2f & ndc_pos) const;
        
        V2f project(const V3f & wpos) const;
        Ray unproject(const V2f & ndc_pos) const;
        Ray unproject(const V2f & ndc_pos, float dx, float dy, Ray & r_dx, Ray & r_dy) const;

        V3f getPosition() const;
        void setPosition(const V3f &pos);
        
        const M44f & getTransform() const;
        M44f & getTransform();
        void setTransform(const M44f & xform);
        
    private:
        M44f  m_xform;
        M44f  m_xform_inv;
        float m_focal;
        float m_aperture;
        float m_aspect_ratio;
        V2f m_clipping_planes;
        float m_cam_x_max;
        float m_cam_y_max;
    };
}
#endif /* fr_Camera_h */
