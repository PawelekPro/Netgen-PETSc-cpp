#include "FvmMeshToVtk.hpp"

#include <vtkPolyData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInformation.h>
#include <vtkTetra.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkPolyDataWriter.h>

FvmMeshToVtk::FvmMeshToVtk(const std::shared_ptr<FvmMeshContainer> &fvmMesh)
    : _fvmMesh(fvmMesh) {
}

void FvmMeshToVtk::ConvertFvmMeshToVtk() {
    _vtkMultiBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();

    // const std::vector<vtkSmartPointer<vtkPolyData> > bndData = ConvertFvmBoundaryMeshToPolyData();
    // const vtkSmartPointer<vtkUnstructuredGrid> intData = ConvertFvmInternalMeshToVtk();
    //
    // const int blocksNb = bndData.size() + 1;
    // _vtkMultiBlock->SetNumberOfBlocks(blocksNb);
    // _vtkMultiBlock->SetBlock(0, intData);
    // _vtkMultiBlock->GetMetaData(static_cast<unsigned int>(0))->Set(
    //     vtkCompositeDataSet::NAME(), "VolumeMesh");
    //
    // int index = 1;
    // for (const auto pd: bndData) {
    //     _vtkMultiBlock->SetBlock(index, pd);
    //     _vtkMultiBlock->GetMetaData(index)->Set(
    //         vtkCompositeDataSet::NAME(), _fvmMesh->GetBoundaryLabel(index));
    //     index++;
    // }
}

void FvmMeshToVtk::SaveVtkMeshToFile(const std::string &filename) const {
    const vtkSmartPointer<vtkUnstructuredGrid> intData = ConvertFvmInternalMeshToVtk();

    auto writer = vtkSmartPointer<vtkUnstructuredGridWriter>::New();
    writer->SetFileName(filename.c_str());
    writer->SetInputData(intData);
    writer->SetFileTypeToASCII();
    writer->Write();

    const std::vector<vtkSmartPointer<vtkPolyData> > bndData = ConvertFvmBoundaryMeshToPolyData();
    int index = 1;
    for (const auto pd: bndData) {
        auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        std::string fileToSave = std::string("bnd") + "_" + std::to_string(index) + ".vtk";
        writer->SetFileName(fileToSave.c_str());
        writer->SetInputData(pd);
        writer->SetFileTypeToASCII();
        writer->Write();
        index++;
    }
}

std::vector<vtkSmartPointer<vtkPolyData> > FvmMeshToVtk::ConvertFvmBoundaryMeshToPolyData() const {
    std::vector<vtkSmartPointer<vtkPolyData> > polyDataVec;

    for (int i = 0; i < _fvmMesh->GetSurfacesRegionsNumber(); i++) {
        const auto polyData = vtkSmartPointer<vtkPolyData>::New();
        const auto points = vtkSmartPointer<vtkPoints>::New();
        auto polys = vtkSmartPointer<vtkCellArray>::New();

        for (const auto &patch: _fvmMesh->patches) {
            if (patch.physReg == i) {
                for (const auto &node: patch.nodes) {
                    const FvmMesh::Vector3 vec = _fvmMesh->GetNode(node);
                    points->InsertNextPoint(vec.x, vec.y, vec.z);
                }

                vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
                for (const int nid: patch.nodes)
                    ids->InsertNextId(nid);
                polys->InsertNextCell(ids);
            }
        }

        polyData->SetPoints(points);
        polyData->SetPolys(polys);
        polyDataVec.push_back(polyData);
    }

    return polyDataVec;
}

vtkSmartPointer<vtkUnstructuredGrid> FvmMeshToVtk::ConvertFvmInternalMeshToVtk() const {
    auto vtkMesh
            = vtkSmartPointer<vtkUnstructuredGrid>::New();


    const auto points = vtkSmartPointer<vtkPoints>::New();
    for (const auto &v: _fvmMesh->nodes)
        points->InsertNextPoint(v.x, v.y, v.z);

    vtkMesh->SetPoints(points);

    for (const auto &elem: _fvmMesh->elements) {
        vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();

        for (const int nodeId: elem.nodes) {
            ids->InsertNextId(nodeId - 1);
        }

        switch (elem.type) {
            case FvmMesh::ElementType::TETRAHEDRON:
                if (ids->GetNumberOfIds() == 4)
                    vtkMesh->InsertNextCell(VTK_TETRA, ids);
                break;
            case FvmMesh::ElementType::HEXAHEDRON:
                if (ids->GetNumberOfIds() == 8)
                    vtkMesh->InsertNextCell(VTK_HEXAHEDRON, ids);
                break;
            case FvmMesh::ElementType::PRISM:
                if (ids->GetNumberOfIds() == 6)
                    vtkMesh->InsertNextCell(VTK_WEDGE, ids);
                break;
            case FvmMesh::ElementType::TRIANGLE:
                if (ids->GetNumberOfIds() == 3)
                    vtkMesh->InsertNextCell(VTK_TRIANGLE, ids);
                break;
            case FvmMesh::ElementType::QUADRANGLE:
                if (ids->GetNumberOfIds() == 4)
                    vtkMesh->InsertNextCell(VTK_QUAD, ids);
                break;
            case FvmMesh::ElementType::BEAM:
                if (ids->GetNumberOfIds() == 2)
                    vtkMesh->InsertNextCell(VTK_LINE, ids);
                break;
            default:
                std::cerr << "Error: unknown element type" << std::endl;
                break;
        }
    }

    return vtkMesh;
}
