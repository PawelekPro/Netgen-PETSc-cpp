#include "MeshObject.hpp"

#include "petscksp.h"

#include <vtkCellData.h>
#include <vtkGhostCellsGenerator.h>
#include <vtkPartitionedDataSet.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <filesystem>

#include "MeshWriter.hpp"

using namespace netgen;
namespace fs = std::filesystem;

//----------------------------------------------------------------------------
MeshObject::MeshObject() { }

//----------------------------------------------------------------------------
void MeshObject::DecomposeMesh(const int nProc) {
	if (nProc <= 1) {
		throw std::runtime_error(
			"Error, number of processors must be greater than 1");
	}
	PetscPrintf(
		PETSC_COMM_WORLD, "Decomposing domain to %d partitions\n", nProc);

	this->ParallelMetis(nProc + 1);
	this->SetProcNumber(nProc);
}

//----------------------------------------------------------------------------
void MeshObject::SetPhysicalSurfaceRegionLabel(
	const int index, const std::string& label) {
	_physicalSurfaceRegions[index] = label;
}

//----------------------------------------------------------------------------
std::string MeshObject::GetPhysicalSurfaceRegionLabel(const int index) const {
	return _physicalSurfaceRegions.at(index);
}

//----------------------------------------------------------------------------
void MeshObject::SetPhysicalVolumeRegionLabel(
	const int index, const std::string& label) {
	_physicalVolumeRegions[index] = label;
}

//----------------------------------------------------------------------------
std::string MeshObject::GetPhysicalVolumeRegionLabel(const int index) const {
	return _physicalVolumeRegions.at(index);
}

//----------------------------------------------------------------------------
void MeshObject::SaveDecomposedVtk(const std::string& cwd) const {
	MeshWriter writer;
	std::vector<vtkSmartPointer<vtkUnstructuredGrid>> partitions;
	for (int index = 0; index < _procNumber; ++index) {
		const auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		std::map<int, vtkIdType> pointMap;

		const int ne = GetNE();
		for (int i = 0; i < ne; ++i) {
			const Element& el = VolumeElement(i + 1);

			if (vol_partition[i] != index + 1)
				continue;

			for (int j = +1; j < el.GetNP(); ++j) {
				const int pNum = el.PNum(j);
				if (!pointMap.contains(pNum)) {
					const Point3d& p = this->Point(pNum);
					const vtkIdType pid
						= points->InsertNextPoint(p.X(), p.Y(), p.Z());
					pointMap[pNum] = pid;
				}
			}
		}

		grid->SetPoints(points);

		for (int i = 0; i < ne; ++i) {
			const Element& el = VolumeElement(i + 1);

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

		auto procIdArray = vtkSmartPointer<vtkIntArray>::New();
		procIdArray->SetName("ProcId");
		procIdArray->SetNumberOfComponents(1);
		procIdArray->SetNumberOfTuples(grid->GetNumberOfCells());

		for (vtkIdType cid = 0; cid < grid->GetNumberOfCells(); ++cid) {
			procIdArray->SetValue(cid, index);
		}

		// Attach array to cell data
		grid->GetCellData()->AddArray(procIdArray);
		grid->GetCellData()->SetScalars(procIdArray);
		std::cout << "Partition: " << index << grid->GetNumberOfPoints()
				  << " points, " << grid->GetNumberOfCells() << " elements"
				  << std::endl;

		partitions.push_back(grid);
	}

	const auto partitionedData = vtkSmartPointer<vtkPartitionedDataSet>::New();
	partitionedData->SetNumberOfPartitions(partitions.size());

	for (size_t i = 0; i < partitions.size(); ++i) {
		partitionedData->SetPartition(i, partitions[i]);
	}

	const std::string fileName = "mesh.vtpd";
	fs::path filePath
		= cwd.empty() ? fs::path(fileName) : fs::path(cwd) / fileName;

	const auto dataset = this->GenerateGhostCells(partitionedData);

	if (!output || dataset->GetNumberOfPartitions() == 0) {
		std::cerr << "Ghost cell generation failed: empty output" << std::endl;
	}

	writer.SetInputData(dataset);
	writer.WriteVtpdFile(fileName);
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkPartitionedDataSet> MeshObject::GenerateGhostCells(
	const vtkSmartPointer<vtkPartitionedDataSet>& dataset) {
	const vtkNew<vtkGhostCellsGenerator> generator;
	generator->SetInputData(dataset);
	generator->SetNumberOfGhostLayers(1);
	generator->Update();

	return vtkPartitionedDataSet::SafeDownCast(generator->GetOutput());
}
