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
/** Implementation file for Shader class
 *
 * @file Shader.cpp
 * @author Andrew Keating
 */

#include <Shaders/Shader.h>


namespace Magic3D
{

const int Shader::attributeTypeCompCount[ MAX_ATTRIBUTE_TYPES ] = {
    4, // vertex
    3, // normal
    4, // color
    4, // color2
    2, // texcoord0
    2, // texcoord1
    2, // texcoord2
    2, // texcoord3
    2, // texcoord4
    2, // texcoord5
    2, // texcoord6
    2, // texcoord7
    3, // tangent
    3  // binormal
};

/** Create shader
 * @param vertexProgram source of the vertex program
 * @param fragmentProgram source of the fragment program
 * @param ... number of attributes followed by attribute pairs
 */
Shader::Shader( const char* vertexProgram, const char* fragmentProgram)
{
    // create new empty vertex and fragment shaders
    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);
	
    // Load vertex shader
	GLchar* shaderListing[1];
	shaderListing[0] = (GLchar*)vertexProgram;
	glShaderSource(vsId, 1, (const GLchar**)shaderListing, NULL);
	
	// Load fragment shader
	shaderListing[0] = (GLchar*)fragmentProgram;
	glShaderSource(fsId, 1, (const GLchar**)shaderListing, NULL);
   
    // Compile shader source
    glCompileShader(vsId);
    glCompileShader(fsId);
    
    // Check for compile errors
	GLint ret;
    glGetShaderiv(vsId, GL_COMPILE_STATUS, &ret);
    if(ret == GL_FALSE)
	{
		// delete shaders and throw exception
        glDeleteShader(vsId);
        glDeleteShader(fsId);
        throw_ShaderCompileException("Vertex shader failed to compile");
	}
    glGetShaderiv(fsId, GL_COMPILE_STATUS, &ret);
    if(ret == GL_FALSE)
	{
        glDeleteShader(vsId);
        glDeleteShader(fsId);
        throw_ShaderCompileException("Fragment shader failed to compile");
	}
    
    // create new program and attach compiled shaders
	programId = glCreateProgram();
    glAttachShader(programId, vsId);
    glAttachShader(programId, fsId);
    
    // delete temp shader storage
    glDeleteShader(vsId);
    glDeleteShader(fsId);
    
    nextIndex = 0;
}

/// destructor
Shader::~Shader()
{
    // delete the shader from opengl memory
    glDeleteProgram(this->programId);
}

/** Enable this shader to be used on the next drawing operation
 * and for setting uniforms
 */
void Shader::use()
{
    // set opengl to use this shader
    glUseProgram(this->programId);
    if (glGetError() != GL_NO_ERROR)
        throw_MagicException("Could not use shader program");
}




};











