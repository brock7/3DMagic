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
/** Header file for ResourceManager class
 * 
 * @file ResourceManager.h
 * @author Andrew Keating
 */
#ifndef MAGIC3D_RESOURCE_MANAGER_H
#define MAGIC3D_RESOURCE_MANAGER_H

#include "Resource.h"
#include "TextResource.h"
#include "../Exceptions/MagicException.h"
#include "../Exceptions/ResourceNotFoundException.h"
#include <string>
#include <map>
#include <memory>
#include <fstream>


namespace Magic3D
{


/** Manages access to resources (text, image, raw data, models, etc.)
 */
class ResourceManager
{
private:
	/// mapping of resource names and resources
	std::map<std::string, std::weak_ptr<Resource>> resources;
	
	/// directory where resources are contained
	std::string resourceDir;
	
	/// default constructor
	inline ResourceManager() {}

	template<class T>
	inline std::shared_ptr<T> _get(const std::string& fullPath)
	{
		/* intentionally left blank, always need a specialization */
		return std::make_shared<T>(fullPath, fullPath);
	}
	
public:

	/** Standard constructor
	 * @param resourceDir the directory where resources are located
	 */
	inline ResourceManager(std::string resourceDir): resourceDir(resourceDir)
		{ /*intentionally left blank*/ }
		
	/// destructor
	virtual ~ResourceManager();
		
	/** Check if a resource exists, to be to avoid exceptions for optional resources
	 * @param name the name of the resource
	 * @return true for exists, false otherwise
	 */
	bool doesResourceExist(const std::string& path);
		
	/** get a resource
	 * @param name the name of the resource including any extra path info
	 * @return handle to resource
	 */
	template <class T>
	inline std::shared_ptr<T> get(const std::string& path)
	{
		// check if resource is already loaded
		auto it = resources.find(path);
		if (it != resources.end() && !it->second.expired())
		{
			std::shared_ptr<Resource> ptr = std::shared_ptr<Resource>(it->second);
			return std::dynamic_pointer_cast<T>(ptr);
		}
		
		// otherwise, create new resource

		// make sure file exists
		if (!this->doesResourceExist(path))
			throw_ResourceNotFoundException(path);

		// load resource
		std::shared_ptr<T> resource = this->_get<T>(resourceDir + "/" + path);
		resources.insert(std::pair<std::string, std::weak_ptr<Resource>>(
			path, std::weak_ptr<Resource>(std::dynamic_pointer_cast<Resource>(resource))));
		return resource;
	}
	
};



template<>
inline std::shared_ptr<TextResource> ResourceManager::_get<TextResource>(const std::string& fullPath)
{
	std::ifstream file;
	file.open (fullPath, std::ios::binary );
	
	if (!file.is_open() || !file.good())
	{
		file.close();
		throw_MagicException("Failed to open text resource");
	}

	// get length of file:
	file.seekg (0, std::ios::end);
	int length = (int)file.tellg();
	file.seekg (0, std::ios::beg);

	// read all data into memory
	char* text = new char [length+1];
	file.read (text,length);
	text[length] = 0; // null terminated string
  
	return std::make_shared<TextResource>(text);
}


};






#endif







