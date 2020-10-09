//
//  Camera.cpp
//  FracRndr
//
//  Created by Charles-Felix on 5/1/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#include "Camera.hpp"

using namespace Fr;

Camera::Camera(Real focal, Real aperture, Real aspect_ratio, Real near, Real far):m_xform(), m_xform_inv(),m_focal(focal),m_aperture(aperture),m_aspect_ratio(aspect_ratio),m_clipping_planes(V2f(1,100000))
{
    Real fov = 2 * atan(aperture / (2*focal));
    m_cam_x_max = tan(fov*0.5f) ; 
    m_cam_y_max = m_cam_x_max / m_aspect_ratio;
}

V3f Camera::worldToCamera(const V3f & wpos) const
{
    V3f cpos;
    m_xform_inv.multVecMatrix(wpos, cpos);
    return cpos;
}
V3f Camera::cameraToWorld(const V3f & cpos) const
{
    V3f wpos;
    m_xform.multVecMatrix(cpos, wpos);
    return wpos;
}

V2f Camera::cameraToNdc(const V3f & cpos) const
{
    V2f ndc;
    // project the 3 point on the camera plane
    Real one_over_depth = 1.f/std::abs(cpos.z);
    ndc.x = cpos.x * one_over_depth;
    ndc.y = cpos.y * one_over_depth;
    // normalize the ndc coordinates between 0 and 1 based on the aspect ration
    // and the field of view
    ndc.x = ufit<float>(ndc.x,-m_cam_x_max,m_cam_x_max,0.f,1.f);
    ndc.y = ufit<float>(ndc.y,-m_cam_y_max,m_cam_y_max,0.f,1.f);
    return ndc;

}

V3f Camera::ndcToCamera(const V2f & ndc_pos) const
{
    V3f cpos;
    cpos.x = fit<float>(ndc_pos.x,0.f,1.f,-m_cam_x_max,m_cam_x_max);
    cpos.y = fit<float>(ndc_pos.y,0.f,1.f,-m_cam_y_max,m_cam_y_max);
    cpos.z = -1;
    return cpos;
}

V2f Camera::project(const V3f & wpos) const
{
    return cameraToNdc(worldToCamera(wpos));
}

Ray Camera::unproject(const V2f & ndc_pos) const
{
    V3f screen_pos = cameraToWorld(ndcToCamera(ndc_pos));
    const V3f cam_pos = getPosition();
    V3f dir = screen_pos - cam_pos;
    dir.normalize();
    return Ray(cam_pos,dir);
}

Ray Camera::unproject(const V2f & ndc_pos, Real dx, Real dy, Ray & ray_dx, Ray & ray_dy) const
{
    V3f screen_pos = cameraToWorld(ndcToCamera(ndc_pos));
    V3d screen_pos_dx = cameraToWorld(ndcToCamera(ndc_pos + V2f(dx,0.f)));
    V3d screen_pos_dy = cameraToWorld(ndcToCamera(ndc_pos + V2f(0.f,dy)));

    const V3f cam_pos = getPosition();
    V3f dir = screen_pos - cam_pos;
    dir.normalize();
    
    V3f dir_dx = screen_pos_dx - cam_pos;
    dir_dx.normalize();
    ray_dx = Ray(cam_pos,dir_dx);
    
    V3f dir_dy = screen_pos_dy - cam_pos;
    dir_dy.normalize();
    ray_dy = Ray(cam_pos,dir_dy);
    
    
    return Ray(cam_pos,dir);
}


V3f Camera::getPosition() const
{
    return m_xform.translation();
}

void Camera::setPosition(const V3f &pos)
{
    m_xform.setTranslation(pos);
    m_xform_inv = m_xform;
    m_xform_inv.invert();
}

const M44f & Camera::getTransform() const
{
    return m_xform;
}

M44f & Camera::getTransform()
{
    return m_xform;
}


void Camera::setTransform(const M44f & xform)
{
    m_xform = xform;
    m_xform_inv = m_xform;
    m_xform_inv.invert();
}
