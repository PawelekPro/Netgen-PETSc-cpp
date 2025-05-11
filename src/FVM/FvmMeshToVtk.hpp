#ifndef FVMMESHTOVTK_HPP
#define FVMMESHTOVTK_HPP

#include "FvmMesh.hpp"

#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>


class vtkPolyData;
class vtkMultiBlockDataSet;

class FvmMeshToVtk {
public:
    explicit FvmMeshToVtk(const std::shared_ptr<FvmMeshContainer> &fvmMesh);

    void ConvertFvmMeshToVtk();

    void SaveVtkMeshToFile(const std::string &filename) const;

private:
    vtkSmartPointer<vtkUnstructuredGrid> ConvertFvmInternalMeshToVtk() const;

    std::vector<vtkSmartPointer<vtkPolyData> > ConvertFvmBoundaryMeshToPolyData() const;

private:
    std::shared_ptr<FvmMeshContainer> _fvmMesh;

    vtkSmartPointer<vtkMultiBlockDataSet> _vtkMultiBlock;
};


#endif
