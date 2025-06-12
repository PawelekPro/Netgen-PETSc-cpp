#include "MeshObject.hpp"

#include "petscksp.h"

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkXMLUnstructuredGridWriter.h>

using namespace netgen;

//----------------------------------------------------------------------------
MeshObject::MeshObject() {
}

//----------------------------------------------------------------------------
void MeshObject::DecomposeMesh(const int nProc) {
    if (nProc <= 1) {
        throw std::runtime_error("Error, number of processors must be greater than 1");
    }
    PetscPrintf(PETSC_COMM_WORLD, "Decomposing domain to %d partitions\n", nProc);

    this->ParallelMetis(nProc + 1);
    this->SetProcNumber(nProc);
}

//----------------------------------------------------------------------------
void MeshObject::SetPhysicalSurfaceRegionLabel(
    const int index, const std::string &label) {
    _physicalSurfaceRegions[index] = label;
}

//----------------------------------------------------------------------------
std::string MeshObject::GetPhysicalSurfaceRegionLabel(const int index) const {
    return _physicalSurfaceRegions.at(index);
}

//----------------------------------------------------------------------------
void MeshObject::SetPhysicalVolumeRegionLabel(
    const int index, const std::string &label) {
    _physicalVolumeRegions[index] = label;
}

//----------------------------------------------------------------------------
std::string MeshObject::GetPhysicalVolumeRegionLabel(const int index) const {
    return _physicalVolumeRegions.at(index);
}

//----------------------------------------------------------------------------
void MeshObject::WritePartitionedVtk(const std::string &cwd) const {
    for (int index = 0; index < _procNumber; ++index) {
        const auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

        std::map<int, vtkIdType> pointMap;
        const int np = this->GetNP();
        for (int i = 0; i < np; ++i) {
            const Point3d &p = this->Point(i + 1);
            const vtkIdType pid = points->InsertNextPoint(p.X(), p.Y(), p.Z());
            pointMap[i] = pid;
        }

        grid->SetPoints(points);

        const int ne = GetNE();
        for (int i = 1; i < ne; ++i) {
            const Element &el = VolumeElement(i);

            if (vol_partition[i] != index)
                continue;

            const vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
            for (int j = 0; j < el.GetNP(); ++j)
                ids->InsertNextId(pointMap[el.PNum(j)]);

            switch (el.GetType()) {
                case TET:
                case TET10:
                case PYRAMID:
                    if (ids->GetNumberOfIds() == 4)
                        grid->InsertNextCell(VTK_TETRA, ids);
                    break;
                case HEX:
                case HEX20:
                case HEX7:
                    if (ids->GetNumberOfIds() == 8)
                        grid->InsertNextCell(VTK_HEXAHEDRON, ids);
                    break;
                case PRISM:
                case PRISM12:
                case PYRAMID13:
                case PRISM15:
                    if (ids->GetNumberOfIds() == 6)
                        grid->InsertNextCell(VTK_WEDGE, ids);
                    break;
                case TRIG:
                    if (ids->GetNumberOfIds() == 3)
                        grid->InsertNextCell(VTK_TRIANGLE, ids);
                    break;
                case QUAD:
                case QUAD6:
                case QUAD8:
                    if (ids->GetNumberOfIds() == 4)
                        grid->InsertNextCell(VTK_QUAD, ids);
                    break;
                case SEGMENT:
                case SEGMENT3:
                    if (ids->GetNumberOfIds() == 2)
                        grid->InsertNextCell(VTK_LINE, ids);
                    break;
                default:
                    std::cerr << "Warning: unknown element type\n";
                    break;
            }
        }

        std::string fileName = "mesh_" + std::to_string(index) + ".vtu";
        const auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
        writer->SetFileName(fileName.c_str());
        writer->SetInputData(grid);
        writer->SetDataModeToAscii();
        writer->Write();
    }
}
