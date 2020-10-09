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
        
        Camera(Real focal = 50.0f, Real aperture = 42.4214f, Real aspect_ratio = 1.f, Real near = 0.0001, Real far = 100000000.f);
        
        
        V3f worldToCamera(const V3f & wpos) const;
        V3f cameraToWorld(const V3f & wpos) const;
        V2f cameraToNdc(const V3f & cpos) const;
        V3f ndcToCamera(const V2f & ndc_pos) const;
        
        V2f project(const V3f & wpos) const;
        Ray unproject(const V2f & ndc_pos) const;
        Ray unproject(const V2f & ndc_pos, Real dx, Real dy, Ray & r_dx, Ray & r_dy) const;

        V3f getPosition() const;
        void setPosition(const V3f &pos);
        
        const M44f & getTransform() const;
        M44f & getTransform();
        void setTransform(const M44f & xform);
        
    private:
        M44f  m_xform;
        M44f  m_xform_inv;
        Real m_focal;
        Real m_aperture;
        Real m_aspect_ratio;
        V2f m_clipping_planes;
        Real m_cam_x_max;
        Real m_cam_y_max;
    };
}
#endif /* fr_Camera_h */
