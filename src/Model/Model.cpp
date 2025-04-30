#include "Model.hpp"

#include <MeshInfo.hpp>

#include "MeshObject.hpp"
#include "GeometryObject.hpp"
#include "MeshComputeError.hpp"
#include "MeshGenerator.hpp"
#include "MeshAlgorithm.hpp"

//----------------------------------------------------------------------------
Model::Model()
    : _geometry(std::make_shared<GeometryObject>())
      , _meshAlgorithm(std::make_shared<MeshAlgorithm>())
      , _mesh(std::make_shared<MeshObject>()) {
};

//----------------------------------------------------------------------------
void Model::ImportSTEP(const std::string &filePath) const {
    _geometry->ImportSTEP(filePath);
}

//----------------------------------------------------------------------------
void Model::SetMeshAlgorithm(const std::shared_ptr<MeshAlgorithm> &algorithm) {
    _meshAlgorithm = algorithm;
}

//----------------------------------------------------------------------------
void Model::GenerateMesh() {
    for (const auto &it: _geometry->GetShapesMap()) {
        MeshGenerator meshGenerator(it.second, _meshAlgorithm, _mesh);
        const int result = meshGenerator.Compute();
        if (result != MeshComputeError::COMPERR_OK) {
            throw std::runtime_error("Error while computing mesh");
        }
        _mesh = meshGenerator.GetOutputMesh();
        auto meshInfo2 = MeshInfo(_mesh);
        meshInfo2.PrintSelf();
    }
}

//----------------------------------------------------------------------------
void Model::SaveMeshToFile(const std::string &filePath) const {
    if (!_mesh) {
        throw std::runtime_error("Error, mesh object was not initialized");
    }
    _mesh->SetGeometry(nullptr); // Note: Without this line netgen crashes in meshclass.cpp:1134
    _mesh->Save(std::filesystem::path(filePath));
}
