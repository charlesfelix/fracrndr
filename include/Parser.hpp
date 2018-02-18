//
//  Parser.hpp
//  FracRndr
//
//  Created by Charles-Felix on 10/23/16.
//  Copyright © 2016 Charles-Felix. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp
#include "json.hpp"
#include "Renderer.hpp"
#include "Material.hpp"
#include <map>

using json = nlohmann::json;

template <typename T>
T jsonGet(const std::string & name, json::const_iterator & jit,const T & default_value = T())
{
    json::const_iterator vit = jit->find(name);
    
    if (vit != jit->end())
    {
        return (*vit).get<T>();
    }
    
    return default_value;
}

namespace Fr
{
    class Parser
    {
        virtual int parse(const std::string & filepath) = 0;
        virtual const std::shared_ptr<RenderGlobals> & getGlobals() const =0;
        virtual const Camera::Ptr & getCamera() const =0;
        virtual const Scene::Ptr & getScene() const = 0;
        virtual const Film::Ptr & getFilm() const = 0;
        virtual const Background::Ptr & getBackground() const =0;
        virtual const std::map<std::string,Material::Ptr > & getMaterials() const = 0;
        virtual const std::map<std::string,RenderPrimitve::Ptr > & getPrimitives() const = 0;
        virtual const std::map<std::string, std::string> & getMaterialTable() const = 0;
        
        
    }; // Parser
    
    class JsonParser : public Parser
    {
    public:
        virtual int parse(const std::string & filepath);
        virtual const std::shared_ptr<RenderGlobals> & getGlobals() const;
        virtual const Camera::Ptr & getCamera() const;
        virtual const Scene::Ptr &getScene() const;
        virtual const Film::Ptr &getFilm() const;
        virtual const Background::Ptr & getBackground() const;
        virtual const std::map<std::string,Material::Ptr > & getMaterials() const;
        virtual const std::map<std::string,RenderPrimitve::Ptr > & getPrimitives() const;
        virtual const std::map<std::string, std::string> & getMaterialTable() const;
        

    private:
        void parseGlobals(const json & jit);
        void parseCamera(const json & jit);
        void parseFilm(const json & jit);
        void parseBackground(const json & jit);
        void parseMaterials(const json & jit);
        void parsePrimitives(const json & jit);
        void parseMaterialTable(const json &jit);
        
        
        std::shared_ptr<RenderGlobals> m_globals;
        Camera::Ptr m_camera;
        Scene::Ptr m_scene;
        Film::Ptr m_film;
        Background::Ptr m_bg;
        std::map<std::string,Material::Ptr > m_materials;
        std::map<std::string,RenderPrimitve::Ptr > m_primitives;
        std::map<std::string,std::string> m_material_table;
        
    };
} // Fr
#endif /* Parser_hpp */
