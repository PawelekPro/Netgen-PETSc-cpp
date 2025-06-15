#include "Application.hpp"
#include "MeshAlgorithm.hpp"
#include "Model.hpp"

#include <petscsys.h>
#include <petscviewer.h>

#include "MeshObject.hpp"

static std::string description = "Mesh generator - NETGEN plugin\nAuthor: Paweł Gilewicz\n";

int main(int argc, char *argv[]) {
	char stepFileName[PETSC_MAX_PATH_LEN] = "";
	int procNumber = 1;
	PetscBool stepFileFound, procFound, helpRequested = PETSC_FALSE;

	PetscInitialize(&argc, &argv, nullptr, description.c_str());
	PetscOptionsHasName(nullptr, nullptr, "-help", &helpRequested);
	if (helpRequested) {
		PetscOptionsView(nullptr, PETSC_VIEWER_STDOUT_WORLD);
		PetscFinalize();
		return EXIT_SUCCESS;
	}

	PetscOptionsGetString(nullptr, nullptr, "-stepfile", stepFileName, sizeof(stepFileName), &stepFileFound);
	PetscOptionsGetInt(nullptr, nullptr, "-procnumber", &procNumber, &procFound);
	Application::PrintBanner("OpenFVM++ v2512");

	const auto model = std::make_unique<Model>();
	model->ImportSTEP(std::string(stepFileName));

	const auto meshAlgorithm = std::make_shared<MeshAlgorithm>();

	// ToDo: set algorithm properties (probably through an XML file)
	meshAlgorithm->SetDim(MeshAlgorithm::ALG_3D);
	meshAlgorithm->quadAllowed = false;
	meshAlgorithm->maxSize = 2;

	model->SetMeshAlgorithm(meshAlgorithm);
	model->GenerateMesh();

	model->GetMeshObject()->DecomposeMesh(procNumber);
	model->GetMeshObject()->SaveDecomposedVtk();

	PetscFinalize();
	return EXIT_SUCCESS;
}
