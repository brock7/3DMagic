/** Header file for CustomModel class
 * 
 * @file CustomModel.h
 * @author Andrew Keating
 */
#ifndef MAGIC3D_CUSTOM_MODEL_H
#define MAGIC3D_CUSTOM_MODEL_H

#include "./Model.h"
#include "../Resources/ModelResource.h"

namespace Magic3D
{

/** Respresents a model built from a loaded model
 * resource
 */
class CustomModel : public Model
{
protected:
	
public:
	/** Standard constructor
	 * @param resource the resource to make a model out of
	 */
	CustomModel(const VertexAttribSpec* spec, const ModelResource& resource);
	
	/// destructor
	virtual ~CustomModel();
	
	/** Get the model's type name
	 */
	virtual const char* getTypeName();

};

};



#endif










