#include "MeshWriter.hpp"

#include <vtkPartitionedDataSetCollection.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPartitionedDataSetCollectionWriter.h>
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
void MeshWriter::SetInputData(
	const vtkSmartPointer<vtkUnstructuredGrid>& data) {
	_genericData = data;
}

//----------------------------------------------------------------------------
void MeshWriter::SetInputData(
	const vtkSmartPointer<vtkPartitionedDataSet>& data) {
	_genericData = data;
}

void MeshWriter::SetInputData(
	const vtkSmartPointer<vtkPartitionedDataSetCollection>& data) {
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

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
void MeshWriter::WriteVtpcFile(const std::string& filePath) const {
	if (!_genericData) {
		std::cerr << "Input data is null" << std::endl;
		return;
	}

	const auto writer
		= vtkSmartPointer<vtkXMLPartitionedDataSetCollectionWriter>::New();
	writer->SetFileName(filePath.c_str());

	const auto partData
		= vtkPartitionedDataSetCollection::SafeDownCast(_genericData);
	writer->SetInputData(partData);
	_dataFormat == ASCII ? writer->SetDataModeToAscii()
						 : writer->SetDataModeToBinary();
	writer->Write();
}
