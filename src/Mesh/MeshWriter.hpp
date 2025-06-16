#ifndef MESHWRITER_HPP
#define MESHWRITER_HPP
#include "MeshTypes.hpp"

#include <string>
#include <vtkSmartPointer.h>

class vtkUnstructuredGrid;
class vtkPartitionedDataSet;
class vtkPartitionedDataSetCollection;
class vtkDataObject;

class MeshWriter {
public:
	MeshWriter();
	~MeshWriter() = default;

	void SetDataFormat(DataFormat dataFormat);

	void SetInputData(const vtkSmartPointer<vtkUnstructuredGrid>& data);
	void SetInputData(const vtkSmartPointer<vtkPartitionedDataSet>& data);
	void SetInputData(
		const vtkSmartPointer<vtkPartitionedDataSetCollection>& data);

	void WriteVtuFile(const std::string& filePath) const;
	void WriteVtpdFile(const std::string& filePath) const;
	void WriteVtpcFile(const std::string& filePath) const;

private:
	vtkSmartPointer<vtkDataObject> _genericData;

	DataFormat _dataFormat;
};

#endif
