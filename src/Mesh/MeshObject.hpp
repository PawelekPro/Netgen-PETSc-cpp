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

public:
    void SetPhysicalSurfaceRegionLabel(int index, const std::string &label);

    std::string GetPhysicalSurfaceRegionLabel(int index) const;

    void SetPhysicalVolumeRegionLabel(int index, const std::string &label);

    std::string GetPhysicalVolumeRegionLabel(int index) const;

    int GetSurfaceRegionsNumber() const { return _physicalSurfaceRegions.size(); }
    int GetVolumeRegionsNumber() const { return _physicalVolumeRegions.size(); }

private:
    std::map<int, std::string> _physicalSurfaceRegions;
    std::map<int, std::string> _physicalVolumeRegions;
};


#endif
