#include "MeshGenerator.hpp"
#include "MeshAlgorithm.hpp"
#include "MeshObject.hpp"
#include "MeshComputeError.hpp"
#include "NetgenPluginMesher.hpp"

//----------------------------------------------------------------------------
MeshGenerator::MeshGenerator(
    const TopoDS_Shape &shape,
    const std::shared_ptr<MeshAlgorithm> &algorithm,
    const std::shared_ptr<MeshObject> &meshObject)
    : _meshObject(meshObject)
      , _shape(&shape)
      , _algorithm(algorithm) {
}

//----------------------------------------------------------------------------
std::shared_ptr<MeshObject> MeshGenerator::GetOutputMesh() const {
    return _meshObject;
}

//----------------------------------------------------------------------------
int MeshGenerator::Compute() {
    if (!_meshObject || !_shape || !_algorithm) {
        return COMPERR_BAD_PARAMETERS;
    }

    NetgenPluginMesher netgenMesher(*_shape, _algorithm, _meshObject);
    const int result = netgenMesher.ComputeMesh();
    _meshObject = netgenMesher.GetOutputMesh();
    return result;
}

//----------------------------------------------------------------------------
void MeshGenerator::CancelMeshGeneration() {
    NetgenPluginMesher::CancelMeshGeneration();
}
