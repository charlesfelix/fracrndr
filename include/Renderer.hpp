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

namespace Fr
{
    class RenderGlobals    {
    public:
        RenderGlobals();
        int m_aa;
        std::string m_output_file;
        
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
        
        std::shared_ptr<const Scene> getScene() const;
        void setScene(const Scene::Ptr &scene);
        
        C4f Li(const Ray & r,Primitive::ConstPtr primitives, const Background & bg) const;
        
        void render() const;
        
        void outputProgress(float progress) const;
        
        void initFromFile(const std::string & scenefile);
    private:
        
        RenderGlobals m_render_globals;
        Camera::Ptr m_camera;
        Film::Ptr m_film;
        Scene::Ptr m_scene;
        
        Renderer(const Renderer &);
        

    };
}
#endif /* Renderer_h */
