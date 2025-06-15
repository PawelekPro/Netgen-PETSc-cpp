#ifndef MESHWRITER_HPP
#define MESHWRITER_HPP
#include "MeshTypes.hpp"

#include <string>
#include <vtkSmartPointer.h>

class vtkUnstructuredGrid;

class MeshWriter {
public:
	MeshWriter();

	~MeshWriter() = default;

	void SetDataFormat(DataFormat dataFormat);

	void SetInputData(const vtkSmartPointer<vtkUnstructuredGrid>& data);

	void WriteVtuFile(const std::string& filePath) const;

	static void WritePvtuFile(
		const std::string& cwd, const std::string& fileName, int procNumber);

private:
	vtkSmartPointer<vtkUnstructuredGrid> _data;
	DataFormat _dataFormat;
};

#endif
