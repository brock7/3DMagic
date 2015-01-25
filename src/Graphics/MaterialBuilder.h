/* 
Copyright (c) 2011 Andrew Keating

This file is part of 3DMagic.

3DMagic is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

3DMagic is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with 3DMagic.  If not, see <http://www.gnu.org/licenses/>.

*/
/** Header file for MaterialBuilder class
 *
 * @file MaterialBuilder.h
 * @author Andrew Keating
 */
#ifndef MAGIC3D_MATERIAL_BUILDER_H
#define MAGIC3D_MATERIAL_BUILDER_H


#include "Material.h"

namespace Magic3D
{


/** Builds materials
 */
class MaterialBuilder
{
private:
    /// the material being built
    Material* material;
    
    int curAutoUniform;
    
    int curNamedUniform;
    
public:
    inline MaterialBuilder(): material(NULL), curAutoUniform(0), curNamedUniform(0) {}
    
    inline void begin(Material* material, int autoUniformCount, int namedUniformCount)
    {
        this->material = material;
        material->allocate(autoUniformCount, namedUniformCount);
        this->curAutoUniform = 0;
        this->curNamedUniform = 0;
    }

    inline void modify(Material* material)
    {
        this->material = material;
        this->curAutoUniform = material->autoUniformCount;
        this->curNamedUniform = material->namedUniformCount;
    }

    inline void expand(Material* material, const Material& basis)
    {
        this->material = material;
        this->material->set(basis);
        this->curAutoUniform = material->autoUniformCount;
        this->curNamedUniform = material->namedUniformCount;
    }
    
    inline void setShader(Shader* shader)
    {
        MAGIC_THROW(material == NULL, "Tried to modify material without calling begin().");
        material->shader = shader;
    }
    
    inline void setRenderPrimitive(VertexArray::Primitives primitive)
    {
        MAGIC_THROW(material == NULL, "Tried to modify material without calling begin().");
        material->primitive = primitive;
    }
    
    inline void setTransparentFlag(bool transparent)
    {
        MAGIC_THROW(material == NULL, "Tried to modify material without calling begin().");
        material->transparent = transparent;
    }
    
    void addAutoUniform(const char* varName, Material::AutoUniformType type);
    
    void addNamedUniform(const char* varName, VertexArray::DataTypes datatype,
        int comp_count, const void* data);
    
    inline void setTexture(std::shared_ptr<Texture> t, int number = 0)
    {
        MAGIC_THROW(material == NULL, "Tried to modify material without calling begin().");
        MAGIC_THROW(number < 0 || number > 7, "Tried to set out-of-range texture." );
        material->textures[number] = t;
    }
    
    /// the ammount to lie to the depth buffer when rendering object
    inline void setDepthBufferLie(float lie)
    {
        MAGIC_THROW(material == NULL, "Tried to modify material without calling begin().");
        material->depthBufferLie = lie;
    }
    
    void end();
    
    
};



};

























#endif


