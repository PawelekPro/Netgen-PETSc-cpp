#include "MeshWriter.hpp"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLPartitionedDataSetWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <filesystem>
#include <vtkPartitionedDataSet.h>

namespace fs = std::filesystem;

//----------------------------------------------------------------------------
MeshWriter::MeshWriter() { _dataFormat = ASCII; }

//----------------------------------------------------------------------------
void MeshWriter::SetDataFormat(const DataFormat dataFormat) {
	_dataFormat = dataFormat;
}

//----------------------------------------------------------------------------
void MeshWriter::WritePvtuFile(
	const std::string& cwd, const std::string& fileName, const int procNumber) {
	fs::path pvtuFile = cwd.empty() ? fs::path("mesh") : fs::path(cwd) / "mesh";
	pvtuFile.replace_extension(".pvtu");

	std::ofstream file(pvtuFile);
	if (!file.is_open()) {
		std::cerr << "Cannot write pvtu file at " << pvtuFile << std::endl;
		return;
	}

	file << "<?xml version=\"1.0\"?>\n";
	file << "<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" "
			"byte_order=\"LittleEndian\">\n";
	file << "  <PUnstructuredGrid GhostLevel=\"0\">\n";

	file << "    <PCellData Scalars=\"ProcId\">\n";
	file << "      <PDataArray type=\"Int32\" Name=\"ProcId\" "
			"format=\"ascii\"/>\n";
	file << "    </PCellData>\n";

	file << "    <PPoints>\n";
	file << "      <PDataArray type=\"Float32\" NumberOfComponents=\"3\" "
			"format=\"ascii\"/>\n";
	file << "    </PPoints>\n";

	for (int i = 0; i < procNumber + 0; ++i) {
		file << "    <Piece Source=\"" << fileName << "_" << i << ".vtu\"/>\n";
	}

	file << "  </PUnstructuredGrid>\n";
	file << "</VTKFile>\n";

	std::cout << "Writing parallel VTK file: " << pvtuFile << std::endl;
}

//----------------------------------------------------------------------------
void MeshWriter::SetInputData(
	const vtkSmartPointer<vtkUnstructuredGrid>& data) {
	_genericData = data;
}

//----------------------------------------------------------------------------
void MeshWriter::SetInputData(
	const vtkSmartPointer<vtkPartitionedDataSet>& data) {
	_genericData = data;
}

//----------------------------------------------------------------------------
void MeshWriter::WriteVtuFile(const std::string& filePath) const {
	if (!_genericData) {
		std::cerr << "Input data is null" << std::endl;
		return;
	}
	const auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
	writer->SetFileName(filePath.c_str());

	const auto grid = vtkUnstructuredGrid::SafeDownCast(_genericData);

	writer->SetInputData(grid);
	_dataFormat == ASCII ? writer->SetDataModeToAscii()
						 : writer->SetDataModeToBinary();
	writer->Write();
}

void MeshWriter::WriteVtpdFile(const std::string& filePath) const {
	if (!_genericData) {
		std::cerr << "Input data is null" << std::endl;
		return;
	}
	const auto writer = vtkSmartPointer<vtkXMLPartitionedDataSetWriter>::New();
	writer->SetFileName(filePath.c_str());

	const auto partData = vtkPartitionedDataSet::SafeDownCast(_genericData);
	writer->SetInputData(partData);
	_dataFormat == ASCII ? writer->SetDataModeToAscii()
						 : writer->SetDataModeToBinary();
	writer->Write();
}
