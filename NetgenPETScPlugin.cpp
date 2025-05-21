#include "Model.hpp"
#include "MeshAlgorithm.hpp"
#include "FvmMesh.hpp"
#include "FvmMaterial.hpp"
#include "FvmMeshToVtk.hpp"

#include "argparse/argparse.hpp"

#include <iostream>

#include <petscdm.h>
#include <petscsys.h>
#include <petscviewer.h>


#include "FvmSetup.hpp"
#include "FvmVar.hpp"
#include "FvmVector.hpp"
#include "Globals.hpp"


int main(const int argc, char *argv[]) {
	int petscArgc = argc;
	char **petscArgv = argv;
	static char help[] =
			"Three-dimensional unstructured finite-volume implicit flow solver.\n";
	PetscInitialize(&petscArgc, &petscArgv, nullptr, help);

	PetscPrintf(PETSC_COMM_WORLD, "\n");
	PetscPrintf(PETSC_COMM_WORLD, "*****************************************\n");
	PetscPrintf(PETSC_COMM_WORLD, "*                                       *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*			NetPet-Fvm v1.0			   *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*                                       *\n");
	PetscPrintf(PETSC_COMM_WORLD, "*****************************************\n");
	PetscPrintf(PETSC_COMM_WORLD, "\n");

	argparse::ArgumentParser program(
		"NetPet-Fvm", "1.0.0 (29.04.2025)");
	program.add_description(
		"Plugin for using PETSc library coupled with Netgen facilities and FVM solver.");

	program.add_argument("stepFile")
			.help("STEP CAD file (.stp .step .STP .STEP)")
			.metavar("STEP_FILE");

	program.add_epilog("Done by: Paweł Gilewicz");

	try {
		program.parse_args(argc, argv);
	} catch (const std::exception &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return EXIT_FAILURE;
	}

	const auto stepFile = program.get<std::string>("stepFile");

	Model model;
	model.ImportSTEP(stepFile);

	const auto meshAlgorithm = std::make_shared<MeshAlgorithm>();
	meshAlgorithm->maxSize = 2;
	meshAlgorithm->SetDim(MeshAlgorithm::ALG_3D);
	meshAlgorithm->quadAllowed = false;

	model.SetMeshAlgorithm(meshAlgorithm);
	model.GenerateMesh();

	// Decomposing mesh to nProc domains
	auto mesh = model.GetMeshObject();
	mesh->DecomposeMesh(10);

	model.SaveMeshToFile("meshFile.vol");

	std::shared_ptr<FvmMeshContainer> fvmMesh;
	try {
		fvmMesh = std::make_shared<FvmMeshContainer>(mesh);
		auto fvmToVtk = FvmMeshToVtk(fvmMesh);
		fvmToVtk.ConvertFvmMeshToVtk();
		fvmToVtk.SaveVtkMeshToFile("vtkMeshFile.vtm");
	} catch (const FvmException &ex) {
		std::cerr << "Caught MeshException: " << ex.what()
				<< ", code: " << ex.code() << std::endl;
	}


	const std::string materialsPath = std::string(ASSETS_DIR) + "/materials.xml";
	auto matReg = std::make_shared<MaterialsBase>(materialsPath);
	matReg->PrintSelf();

	auto bndCndBase = std::make_shared<BoundaryConditions>();

	FvmSetup fvmSetup(fvmMesh, bndCndBase, matReg);
	fvmSetup.SetGhosts();

	FvmVector::Init(fvmMesh);

	auto fvmVariables = FvmVar(fvmMesh);

	fvmSetup.SetCenters();

	VecGhostGetLocalForm(FvmVar::cex, &FvmVar::cexl);
	VecGhostGetLocalForm(FvmVar::cey, &FvmVar::ceyl);
	VecGhostGetLocalForm(FvmVar::cez, &FvmVar::cezl);

	// Set initial conditions
	fvmSetup.SetInitialConditions();

	// Set initial flux
	fvmSetup.SetInitialFlux();

	// Set boundary velocity and pressure
	fvmSetup.SetBoundary();

	FvmMaterial mat1 = matReg->GetMaterial("air");
	FvmMaterial mat2 = matReg->GetMaterial("air");
	std::pair materials{mat1, mat2};
	fvmSetup.SetMaterialProperties(materials);

	// FvmVar::Deallocate();
	PetscFinalize();
	return EXIT_SUCCESS;
}

