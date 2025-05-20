#ifndef FVMSETUP_HPP
#define FVMSETUP_HPP

#include <memory>

class FvmMeshContainer;
class BoundaryConditions;
class MaterialsBase;


class FvmSetup {
public:
    FvmSetup(
        const std::shared_ptr<FvmMeshContainer> &fvmMesh,
        const std::shared_ptr<BoundaryConditions> &fvmBndCnd,
        const std::shared_ptr<MaterialsBase> &materialsBase);

    void SetGhosts() const;

    void SetCenters() const;

    void SetInitialConditions() const;

    void SetInitialFlux() const;

    void SetBoundary() const;

private:
    std::shared_ptr<FvmMeshContainer> _fvmMesh;
    std::shared_ptr<BoundaryConditions> _fvmBndCnd;
    std::shared_ptr<MaterialsBase> _materialsBase;
};


#endif
