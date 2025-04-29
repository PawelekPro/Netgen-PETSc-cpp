#include "MeshInfo.hpp"
#include "MeshObject.hpp"

//----------------------------------------------------------------------------
MeshInfo::MeshInfo(std::shared_ptr<MeshObject> mesh) : _mesh(mesh) {}

void MeshInfo::PrintSelf() {
	if (!_mesh) {
		std::cerr << "MeshInfo::PrintSelf: null mesh" << std::endl;
	}

	std::cout << "\nMesh Info:" << std::endl;
	std::cout << "Number Of Nodes: " << _mesh->GetNP() << std::endl;
    std::cout << "Number Of Segments: " << _mesh->GetNSeg() << std::endl;
    std::cout << "Number Of Faces: " << _mesh->GetNSE() << std::endl;
    std::cout << "Number Of Elements: " << _mesh->GetNE() << std::endl;
}