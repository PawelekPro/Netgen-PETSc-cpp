#include "Application.hpp"

#include <petscsys.h>

static std::string appName = "meshGen";
static std::string version = "1.0.0 (09.06.2025)";
static std::string description = "Mesh generator - NETGEN plugin";
static std::string author = "Author: Paweł Gilewicz";

int main(int argc, char* argv[]) {
	Application app(appName, version, description, author);

	if (!app.parse(argc, argv))
		return EXIT_FAILURE;

	PetscInitialize(&argc, &argv, nullptr, description.c_str());

	int processor, processorsNb;
	MPI_Comm_size(PETSC_COMM_WORLD, &processorsNb);
	MPI_Comm_rank(PETSC_COMM_WORLD, &processor);

	app.printBanner("NetPet-Fvm v1.0");
}