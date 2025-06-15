#ifndef MESHWRITER_HPP
#define MESHWRITER_HPP
#include "MeshTypes.hpp"

#include <string>
#include <vtkSmartPointer.h>

class vtkUnstructuredGrid;
class vtkPartitionedDataSet;
class vtkDataObject;

class MeshWriter {
public:
	MeshWriter();
	~MeshWriter() = default;

	void SetDataFormat(DataFormat dataFormat);

	void SetInputData(const vtkSmartPointer<vtkUnstructuredGrid>& data);
	void SetInputData(const vtkSmartPointer<vtkPartitionedDataSet>& data);

	void WriteVtuFile(const std::string& filePath) const;
	void WriteVtpdFile(const std::string& filePath) const;
	static void WritePvtuFile(
		const std::string& cwd, const std::string& fileName, int procNumber);

private:
	vtkSmartPointer<vtkDataObject> _genericData;

	DataFormat _dataFormat;
};

#endif
