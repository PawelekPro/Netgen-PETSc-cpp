#ifndef FVMSIMULATION_HPP
#define FVMSIMULATION_HPP

#include <string>

#include "Model.hpp"

class FvmMeshContainer;

class FvmSimulation {
public:
    FvmSimulation();

    ~FvmSimulation() = default;

    void GenerateMesh(const std::string &filepath) const;

    int ConstructGlobalFvmMesh();

    void ExportMeshPartitions() const;

    void DecomposeMesh() const;

    static int Start(const std::string &filepath);

private:
    std::unique_ptr<Model> _model;
    std::shared_ptr<FvmMeshContainer> _globalFvmMesh;
};


#endif
