#include "Model.hpp"
#include "MeshAlgorithm.hpp"
#include "NetgenToDMPlex.hpp"

#include "argparse/argparse.hpp"

#include <iostream>

#include <petscdm.h>
#include <petscsys.h>
#include <petscviewer.h>


int main(const int argc, char *argv[]) {
	argparse::ArgumentParser program(
		"NetgenPETScPlugin", "1.0.0 (29.04.2025)");
	program.add_description(
		"Plugin for using PETSc library coupled with Netgen facilities");

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
	meshAlgorithm->maxSize = 10;
	meshAlgorithm->SetDim(MeshAlgorithm::ALG_3D);
	meshAlgorithm->quadAllowed = true;

	model.SetMeshAlgorithm(meshAlgorithm);

	model.GenerateMesh();
	model.SaveMeshToFile("meshFile.vol");

	// int petscArgc = argc;
	// char **petscArgv = argv;
	// PetscInitialize(&petscArgc, &petscArgv, nullptr, nullptr);
	// NetgenToDMPlex converter(model.GetMeshObject(), meshAlgorithm);
	// DM dm;
	// PetscCall(converter.ConvertToDMPlex(PETSC_COMM_WORLD, &dm));
	// PetscCall(DMView(dm, PETSC_VIEWER_STDOUT_WORLD));
	//
	//
	// PetscViewer viewer;
	// PetscViewerVTKOpen(PETSC_COMM_WORLD, "mesh.vtk", FILE_MODE_WRITE, &viewer);
	// PetscCall(DMView(dm, viewer));
	// PetscViewerDestroy(&viewer);
	//
	// PetscCall(DMDestroy(&dm));
	// PetscFinalize();

	return EXIT_SUCCESS;
}

