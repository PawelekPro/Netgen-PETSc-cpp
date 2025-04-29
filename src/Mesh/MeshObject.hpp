#ifndef MESHOBJECT_HPP
#define MESHOBJECT_HPP

#include "NetgenPluginDefs.hpp"

#ifndef OCCGEOMETRY
#define OCCGEOMETRY
#endif
#include <meshing.hpp>
#include <occgeom.hpp>

namespace netgen {
    NETGENPLUGIN_DLL_HEADER
    extern MeshingParameters mparam;
}


class MeshObject : public netgen::Mesh {
    public:
		MeshObject();
		~MeshObject() = default;
};


#endif
