#ifndef MESHINFO_HPP
#define MESHINFO_HPP

#include "NetgenPluginDefs.hpp"

#include <memory>

class MeshObject;

struct NETGENPLUGIN_EXPORT MeshInfo {
	explicit MeshInfo(std::shared_ptr<MeshObject> mesh);

	~MeshInfo() = default;

	void PrintSelf();

private:
	std::shared_ptr<MeshObject> _mesh;
};


#endif
