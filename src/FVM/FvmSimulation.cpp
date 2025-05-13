#include "FvmSimulation.hpp"
#include "Globals.hpp"

#include <petscsys.h>

#include <array>
#include <fstream>
#include <iostream>

using namespace FvmSimulation;

int FvmSimulation::Start(const std::string &filepath) {
    constexpr int size = ToInt(FieldIndex::Size);

    std::array<char, size> var = {'u', 'v', 'w', 'p', 'T', 's'};
    std::array<double, size> fres = {0.0};
    std::array<int, size> fiter = {0};

    int iter = 0;
    int irestart = 0;

    double startTime = 0.0, endTime = 0.0, curTime = 0.0, dt = 0.0;
    double wtime = 0.0, wdt = 0.0;
    double maxCp = 0.0;

    double Vc = 0.0, Vt = 0.0, Vcp = 0.0, Vtp = 0.0;
    double pf = 0.0;

    const std::string residualsFile = filepath + "/residuals";
    const std::string resultsFile = filepath + "/results";

    std::ofstream fpresults(resultsFile);
    std::ofstream fpresiduals(residualsFile);

    if (!fpresults || !fpresiduals) {
        std::cerr << "Failed to open one or more simulation files in path: " << filepath << "\n";
        return LOGICAL_ERROR;
    }

    PetscPrintf(PETSC_COMM_WORLD, "\n");
    PetscPrintf(PETSC_COMM_WORLD, "Allocating memory...\n");

    return LOGICAL_TRUE;
}
