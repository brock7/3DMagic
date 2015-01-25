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
/** Implementation file for World class
 *
 * @file World.h
 * @author Andrew Keating
 */
 
#include <World/World.h>
#include <Cameras/FPCamera.h>

namespace Magic3D
{
    

void World::stepPhysics()
{
    // step physics
    if ( physicsStepTime > 0.0f )
    {
        static float normalStepSize = 1.0f/60.0f;
        int subSteps = (int) (physicsStepTime / normalStepSize);
        if (subSteps < 1 )
            subSteps = 1;
        
        for (int i=0; i < physicsStepsPerFrame; i++)
            physics.stepSimulation(physicsStepTime, subSteps);
    }
}
    
    
void World::renderObjects()
{   
    // ensure that we have a camera
    MAGIC_THROW(camera == NULL, "Tried to process a frame without a camera set." );
    
    // Clear the color and depth buffers
	graphics.clearDisplay();
	
	// get view and projection matrices for scene (same for all objects)
    Matrix4 view;
    camera->getPosition().getCameraMatrix(view);
    const Matrix4& projection = camera->getProjectionMatrix();
    
    // setup wireframe if set to
    if (wireframeEnabled)
    {
        glEnable(GL_BLEND);
        glEnable(GL_LINE_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

	// render all objects
	std::set<Object*>::iterator it = objects.begin();
	Object* ob;
	VertexArray* array;
	const Mesh::AttributeData* adata;
	int attributeCount;
	int vertexCount;
	Shader* shader; 
	for(; it != objects.end(); it++)
	{
	    // get object and entity
	    ob = (*it);
	    
	    /// render this object and all it's attachments
	    Object* base_ob = ob;
	    std::set<Object*>::iterator it = base_ob->attachments.begin();
	    while (ob != NULL)
	    {   
			const std::shared_ptr<Meshes> meshes = ob->getMeshes();
			if (meshes == nullptr)
				break;

            // get mesh and material data
			const Material* material = ob->getMaterial();
            
            // get model/world matrix for object (same for all meshes in object)
            Matrix4 model;
            ob->getPosition().getTransformMatrix(model);
			model.multiply(ob->getTransformMatrix());
                
            // ensure there is a shader
            shader = material->shader;
            MAGIC_ASSERT(shader != NULL);

			for(const std::shared_ptr<Mesh> mesh : *meshes)
			{
                
				// get mesh data
				adata = mesh->getAttributeData();
				attributeCount = mesh->getAttributeCount();
				vertexCount = mesh->getVertexCount();
                
				// 'use' shader
				shader->use();
                
				// set named uniforms
				for(int i=0; i < material->namedUniformCount; i++)
				{
					Material::NamedUniform& u = material->namedUniforms[i];
					switch(u.datatype)
					{
						case VertexArray::FLOAT:
							shader->setUniformfv(u.varName, u.comp_count, (const float*)u.data);
							break;
						case VertexArray::INT:
							shader->setUniformiv(u.varName, u.comp_count, (const int*)u.data);
							break;
						default:
							throw_MagicException("Unsupported Auto Uniform datatype." );
							break;
					}
				}
                
				// set auto uniforms
				Matrix4 temp4m;
				Matrix4 temp4m2;
				Matrix3 temp3m;
				Point3 tempp3;
				for(int i=0; i < material->autoUniformCount; i++)
				{
					Material::AutoUniform& u = material->uniforms[i];
					switch (u.type)
					{
						case Material::MODEL_MATRIX:                   // mat4
							shader->setUniformMatrix( u.varName, 4, model.getArray() );
							break;
						case Material::VIEW_MATRIX:                    // mat4
							shader->setUniformMatrix( u.varName, 4, view.getArray() );
							break;
						case Material::PROJECTION_MATRIX:              // mat4
							shader->setUniformMatrix( u.varName, 4, projection.getArray() );
							break;
						case Material::MODEL_VIEW_MATRIX:              // mat4
							temp4m.multiply(view, model);
							shader->setUniformMatrix( u.varName, 4, temp4m.getArray() );
							break;
						case Material::VIEW_PROJECTION_MATRIX:         // mat4
							temp4m.multiply(projection, view);
							shader->setUniformMatrix( u.varName, 4, temp4m.getArray() );
							break;
						case Material::MODEL_PROJECTION_MATRIX:        // mat4
							temp4m.multiply(projection, model);
							shader->setUniformMatrix( u.varName, 4, temp4m.getArray() );
							break;
						case Material::MODEL_VIEW_PROJECTION_MATRIX:   // mat4
							temp4m.multiply(view, model);
							temp4m2.multiply(projection, temp4m);
							shader->setUniformMatrix( u.varName, 4, temp4m2.getArray() );
							break;
						case Material::NORMAL_MATRIX:                  // mat3
							temp4m.multiply(view, model);
							temp4m.extractRotation(temp3m);
							shader->setUniformMatrix( u.varName, 3, temp3m.getArray() );
							break;
						case Material::FPS:                            // int
							shader->setUniformiv( u.varName, 1, &this->actualFPS );
							break;
						case Material::TEXTURE0:                       // sampler2D
							MAGIC_THROW(material->textures[0] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[0].get() );
							break;
						case Material::TEXTURE1:                       // sampler2D
							MAGIC_THROW(material->textures[1] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[1].get() );
							break;
						case Material::TEXTURE2:                       // sampler2D
							MAGIC_THROW(material->textures[2] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[2].get() );
							break;
						case Material::TEXTURE3:                       // sampler2D
							MAGIC_THROW(material->textures[3] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[3].get() );
							break;
						case Material::TEXTURE4:                       // sampler2D
							MAGIC_THROW(material->textures[4] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[4].get() );
							break;
						case Material::TEXTURE5:                       // sampler2D
							MAGIC_THROW(material->textures[5] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[5].get() );
							break;
						case Material::TEXTURE6:                       // sampler2D
							MAGIC_THROW(material->textures[6] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[6].get() );
							break;
						case Material::TEXTURE7:                       // sampler2D
							MAGIC_THROW(material->textures[7] == NULL, "Material has auto-bound "
								"texture set, but no texture set for the index." );
							shader->setTexture( u.varName, material->textures[7].get() );
							break;
						case Material::LIGHT_LOCATION:                 // vec3
							MAGIC_THROW(light == NULL, "Material has the light location "
								"auto-bound uniform set, but no light is set for the world." );
							tempp3 = light->getLocation().transform(view);
							shader->setUniformf( u.varName, tempp3.x(),
								tempp3.y(), tempp3.z() );
							break;
						case Material::FLAT_PROJECTION:   // mat4
							temp4m.createOrthographicMatrix(0, this->graphics.getDisplayWidth(), 
								0, (Scalar)this->graphics.getDisplayHeight(), -1.0, 1.0);
							shader->setUniformMatrix( u.varName, 4, temp4m.getArray() );
							break;
						default:
							MAGIC_ASSERT( false );
							break;
					}
				}
                
				// bind vertexArray
				array = new VertexArray();
				for(int j=0; j < attributeCount; j++)
				{
					int bind = shader->getAttribBinding(adata[j].type);
					if (bind < 0) // shader does not have attribute
						continue; 
					array->setAttributeArray(bind, Shader::attributeTypeCompCount[(int)adata[j].type],
						VertexArray::FLOAT, adata[j].buffer);
				}
                
				// check for a depth lie
				if (material->depthBufferLie)
				{
					glPolygonOffset( material->depthBufferLie, 1.0f );
					glEnable(GL_POLYGON_OFFSET_FILL);
				}
                
				// draw mesh
				array->draw(material->primitive, vertexCount);
                
				// disable depth lie if it was enabled
				if (material->depthBufferLie)
					glDisable(GL_POLYGON_OFFSET_FILL);
                
				// delete bound array
				delete array;
			}
            
            // check object for (more) attachments
            if ( it != base_ob->attachments.end() )
            {
                ob = (*it);
                it++;
            }
            else
                ob = NULL;
            
	    } // end of this object and all attachments
	    
	} // end of all objects 
	
	// restore after wireframe
    if (wireframeEnabled)
    {
        glDisable(GL_LINE_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
	
	// Do the buffer Swap
    graphics.swapBuffers();
}










};


















