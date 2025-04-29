#ifndef NETGENPLUGIN_MESHER_H
#define NETGENPLUGIN_MESHER_H

#include "NetgenPluginDefs.hpp"

#include <memory>

namespace netgen {
	class OCCGeometry;
	class Mesh;
}

class gp_XYZ;
class TopoDS_Shape;
class NetgenPlugin_Netgen2VTK;
class MGTMeshUtils_ViscousLayers;
class MeshAlgorithm;
class MeshObject;

class NETGENPLUGIN_EXPORT NetgenPluginMesher {
public:
	NetgenPluginMesher(
		const TopoDS_Shape &shape, const std::shared_ptr<MeshAlgorithm> &algorithm,
		const std::shared_ptr<MeshObject> &mesh);

	explicit NetgenPluginMesher(const TopoDS_Shape &shape);

	~NetgenPluginMesher();

	int ComputeMesh();

	static void CancelMeshGeneration();

	static void PrepareOCCGeometry(
		netgen::OCCGeometry &occGeom, const TopoDS_Shape &shape);

	static void RestrictLocalSize(
		netgen::Mesh &ngMesh, const gp_XYZ &p, double size, bool overrideMinH = true);

	static void SetLocalSize(
		netgen::OCCGeometry &occGeom, netgen::Mesh &ngMesh);

	void SetMeshParameters();

	[[nodiscard]] std::shared_ptr<MeshObject> GetOutputMesh() { return _ngMesh; };

private:
	const TopoDS_Shape &_shape;
	std::shared_ptr<MeshAlgorithm> _algorithm;
	bool _optimize;
	int _fineness;
	bool _isViscousLayers2D;

	std::shared_ptr<MeshObject> _ngMesh;
	netgen::OCCGeometry *_occGeom;

	// a pointer to NetgenPlugin_Mesher* field of the holder, that will be
	// nullified at destruction of this
	NetgenPluginMesher **_selfPtr;
};

#endif
