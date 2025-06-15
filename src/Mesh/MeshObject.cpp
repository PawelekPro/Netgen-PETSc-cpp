#include "MeshObject.hpp"

#include "petscksp.h"

#include <vtkCellData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <filesystem>

using namespace netgen;
namespace fs = std::filesystem;

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

        const int ne = GetNE();
        for (int i = 0; i < ne; ++i) {
            const Element &el = VolumeElement(i + 1);

            if (vol_partition[i] != index + 1)
                continue;

            for (int j = +1; j < el.GetNP(); ++j) {
                const int pNum = el.PNum(j);
                if (!pointMap.contains(pNum)) {
                    const Point3d &p = this->Point(pNum);
                    const vtkIdType pid = points->InsertNextPoint(p.X(), p.Y(), p.Z());
                    pointMap[pNum] = pid;
                }
            }
        }

        grid->SetPoints(points);

        for (int i = 0; i < ne; ++i) {
            const Element &el = VolumeElement(i + 1);

            if (vol_partition[i] != index + 1)
                continue;

            auto ids = vtkSmartPointer<vtkIdList>::New();
            for (int j = 0; j < el.GetNP(); ++j)
                ids->InsertNextId(pointMap[el.PNum(j + 1)]);

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

        // Create and fill ProcId array
        auto procIdArray = vtkSmartPointer<vtkIntArray>::New();
        procIdArray->SetName("ProcId");
        procIdArray->SetNumberOfComponents(1);
        procIdArray->SetNumberOfTuples(grid->GetNumberOfCells());

        for (vtkIdType cid = 0; cid < grid->GetNumberOfCells(); ++cid) {
            procIdArray->SetValue(cid, index);
        }

        // Attach array to cell data
        grid->GetCellData()->AddArray(procIdArray);
        // Set it as active scalars so ParaView uses it by default
        grid->GetCellData()->SetScalars(procIdArray);

        std::string fileName = "meshPart_" + std::to_string(index) + ".vtu";
        fs::path filePath = cwd.empty() ? fs::path(fileName) : fs::path(cwd) / fileName;
        const auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
        writer->SetFileName(filePath.c_str());
        writer->SetInputData(grid);
        writer->SetDataModeToAscii();
        writer->Write();

        std::cout << "Writing partition " << index << ": "
                << points->GetNumberOfPoints() << " points, "
                << grid->GetNumberOfCells() << " elements" << std::endl;
    }

    WritePvtuFile(cwd);
}

void MeshObject::WritePvtuFile(const std::string &cwd, const std::string &fileName) const {
    fs::path pvtuFile = cwd.empty() ? fs::path("mesh") : fs::path(cwd) / "mesh";
    pvtuFile.replace_extension(".pvtu");

    std::ofstream file(pvtuFile);
    if (!file.is_open()) {
        std::cerr << "Cannot write pvtu file at " << pvtuFile << std::endl;
        return;
    }

    file << "<?xml version=\"1.0\"?>\n";
    file << "<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
    file << "  <PUnstructuredGrid GhostLevel=\"0\">\n";

    file << "    <PCellData Scalars=\"ProcId\">\n";
    file << "      <PDataArray type=\"Int32\" Name=\"ProcId\" format=\"ascii\"/>\n";
    file << "    </PCellData>\n";

    file << "    <PPoints>\n";
    file << "      <PDataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\"/>\n";
    file << "    </PPoints>\n";

    for (int i = 0; i < _procNumber + 0; ++i) {
        file << "    <Piece Source=\"" << fileName << "_" << i << ".vtu\"/>\n";
    }

    file << "  </PUnstructuredGrid>\n";
    file << "</VTKFile>\n";

    std::cout << "Writing parallel VTK file: " << pvtuFile << std::endl;
}
