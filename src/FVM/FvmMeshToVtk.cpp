#include "FvmMeshToVtk.hpp"

#include <vtkPolyData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkInformation.h>
#include <vtkTetra.h>
#include <vtkXMLMultiBlockDataWriter.h>
#include <vtkIntArray.h>
#include <vtkCellData.h>

FvmMeshToVtk::FvmMeshToVtk(const std::shared_ptr<FvmMeshContainer> &fvmMesh)
    : _fvmMesh(fvmMesh) {
}

void FvmMeshToVtk::ConvertFvmMeshToVtk() {
    _vtkMultiBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();

    const std::vector<vtkSmartPointer<vtkPolyData> > bndData = ConvertFvmBoundaryMeshToPolyData();
    const vtkSmartPointer<vtkUnstructuredGrid> intData = ConvertFvmInternalMeshToVtk();

    const int blocksNb = bndData.size() + 1;
    _vtkMultiBlock->SetNumberOfBlocks(blocksNb);
    _vtkMultiBlock->SetBlock(0, intData);
    _vtkMultiBlock->GetMetaData(static_cast<unsigned int>(0))->Set(
        vtkCompositeDataSet::NAME(), "VolumeMesh");

    int index = 1;
    for (const auto pd: bndData) {
        _vtkMultiBlock->SetBlock(index, pd);
        _vtkMultiBlock->GetMetaData(index)->Set(
            vtkCompositeDataSet::NAME(), _fvmMesh->GetBoundaryLabel(index));
        index++;
    }
}

void FvmMeshToVtk::SaveVtkMeshToFile(const std::string &filename) const {
    vtkSmartPointer<vtkXMLMultiBlockDataWriter> writer =
            vtkSmartPointer<vtkXMLMultiBlockDataWriter>::New();

    writer->SetFileName(filename.c_str());
    writer->SetInputData(_vtkMultiBlock);
    writer->SetDataModeToAscii();
    writer->Write();
}

std::vector<vtkSmartPointer<vtkPolyData> > FvmMeshToVtk::ConvertFvmBoundaryMeshToPolyData() const {
    std::vector<vtkSmartPointer<vtkPolyData> > polyDataVec;

    int patchesNb = _fvmMesh->GetSurfacesRegionsNumber();
    polyDataVec.resize(patchesNb);

    for (int i = 0; i < patchesNb; ++i) {
        auto polyData = vtkSmartPointer<vtkPolyData>::New();
        auto points = vtkSmartPointer<vtkPoints>::New();
        auto polys = vtkSmartPointer<vtkCellArray>::New();
        auto partitionArray = vtkSmartPointer<vtkIntArray>::New();
        partitionArray->SetName("procId");
        partitionArray->SetNumberOfComponents(1);

        std::map<int, vtkIdType> globalToLocal;

        for (const auto &patch: _fvmMesh->patches) {
            if (patch.physReg != i + 1)
                continue;

            auto ids = vtkSmartPointer<vtkIdList>::New();

            for (const int node: patch.nodes) {
                auto it = globalToLocal.find(node);
                if (it == globalToLocal.end()) {
                    const FvmMesh::Vector3 vec = _fvmMesh->GetNode(node - 1);
                    vtkIdType newId = points->InsertNextPoint(vec.x, vec.y, vec.z);
                    globalToLocal[node] = newId;
                    ids->InsertNextId(newId);
                } else {
                    ids->InsertNextId(it->second);
                }
            }

            polys->InsertNextCell(ids);
            partitionArray->InsertNextValue(patch.partition);
        }

        polyData->SetPoints(points);
        polyData->SetPolys(polys);
        polyData->GetCellData()->AddArray(partitionArray);
        polyDataVec[i] = polyData;
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

    vtkSmartPointer<vtkIntArray> partitionArray = vtkSmartPointer<vtkIntArray>::New();
    partitionArray->SetName("procId");
    partitionArray->SetNumberOfComponents(1);
    partitionArray->SetNumberOfTuples(_fvmMesh->elementsNb);
    for (int i = 0; i < _fvmMesh->elementsNb; ++i) {
        partitionArray->SetValue(i, _fvmMesh->elements[i].partition);
    }

    vtkMesh->GetCellData()->AddArray(partitionArray);
    return vtkMesh;
}
