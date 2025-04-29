#ifndef NETGENTODMPLEX_HPP
#define NETGENTODMPLEX_HPP

#include <petscdmplex.h>
#include <petscsnes.h>
#include <petscts.h>
#include <petscds.h>
#include <petscdm.h>

#include <memory>

class MeshObject;
class MeshAlgorithm;

class NetgenToDMPlex {
public:
	NetgenToDMPlex(
		const std::shared_ptr<MeshObject> &mesh,
		const std::shared_ptr<MeshAlgorithm> &algorithm);

	~NetgenToDMPlex() = default;

	PetscErrorCode ConvertToDMPlex(MPI_Comm comm, DM *dm);

private:
	std::shared_ptr<MeshObject> _mesh;
	std::shared_ptr<MeshAlgorithm> _algorithm;
};


#endif
