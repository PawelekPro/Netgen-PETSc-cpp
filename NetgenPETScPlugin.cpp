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
	// mesh->DecomposeMesh(11);

	// for (int i = 0; i < mesh->GetNE(); ++i) {
	// 	std::cout << mesh->vol_partition[i] << std::endl;
	// }

	model.SaveMeshToFile("meshFile.vol");


	try {
		auto fvmMesh = std::make_shared<FvmMeshContainer>(model.GetMeshObject());
		auto fvmToVtk = FvmMeshToVtk(fvmMesh);
		fvmToVtk.ConvertFvmMeshToVtk();
		fvmToVtk.SaveVtkMeshToFile("vtkMeshFile.vtm");
	} catch (const FvmException &ex) {
		std::cerr << "Caught MeshException: " << ex.what()
				<< ", code: " << ex.code() << std::endl;
	}


	// const std::string materialsPath = std::string(ASSETS_DIR) + "/materials.xml";
	// auto matReg = MaterialsBase(materialsPath);
	// matReg.PrintSelf();


	PetscFinalize();
	return EXIT_SUCCESS;
}

