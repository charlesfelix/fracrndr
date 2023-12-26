//
//  Renderer.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/16/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Renderer_h
#define Renderer_h

#include "Camera.hpp"
#include "Scene.hpp"
#include "Film.hpp"
#include "Sampler.hpp"

namespace Fr
{
    class RenderGlobals    {
    public:
        RenderGlobals();
        unsigned m_aa;
        unsigned m_max_ray_depth;
        std::string m_output_file;
        
    };
    
    class RenderStats    {
    public:
        RenderStats():num_rays(0){};
        unsigned long num_rays;
    };

    struct RenderChannels {
        RenderChannels():color(C4f(0.f,0.f,0.f,0.f)), normal(0.f), matid(0), depth(999999999999999999999999.f), num_hits_accel(0){};
        C4f color;
        C3f normal;
        unsigned matid;
        Real depth;
        size_t num_hits_accel;
    };
    
    class Renderer
    {
    public:
        
        Renderer();
        
        void setRenderGlobals(const RenderGlobals & rg);
        
        Camera::ConstPtr getCamera() const;
        void setCamera(const Camera::Ptr &camera);

        Film::ConstPtr getFilm() const;
        
        void setFilm(const Film::Ptr &film);
        
        Scene::ConstPtr getScene() const;
        void setScene(const Scene::Ptr &scene);
        
        C4f Li(const Ray & r,const RenderPrimitive::ConstPtr & primitives, const Background & bg, RenderStats & rs, Sampler & sampler) const;
        
        int LiNR(const Ray & r,const RenderPrimitive::ConstPtr & primitives, const Background & bg, RenderStats & rs, Sampler & sampler, RenderChannels & channels) const;

        void render() const;
        
        void outputProgress(float progress) const;
        
        int initFromFile(const std::string & scenefile);
    private:
        
        RenderGlobals m_render_globals;
        mutable RenderStats m_render_stats;
        Camera::Ptr m_camera;
        Film::Ptr m_film;
        Scene::Ptr m_scene;
        
        Renderer(const Renderer &);
        

    };
}
#endif /* Renderer_h */
