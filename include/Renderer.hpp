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
    class Renderer
    {
    public:
        Renderer();
        
        std::shared_ptr<const Camera> getCamera() const;
        void setCamera(const std::shared_ptr<Camera> &camera);

        std::shared_ptr<const Film> getFilm() const;
        void setFilm(const std::shared_ptr<Film> &film);
        
        std::shared_ptr<const Scene> getScene() const;
        void setScene(const std::shared_ptr<Scene> &scene);
        
        C4f Li(const Ray & r,std::shared_ptr<const Primitive> primitives, const Background & bg) const;
        
        void render() const;
        
        void outputProgress(float progress) const;
    private:
        std::shared_ptr<Camera> m_camera;
        std::shared_ptr<Film> m_film;
        std::shared_ptr<Scene> m_scene;
        
        Renderer(const Renderer &);
        

    };
}
#endif /* Renderer_h */
