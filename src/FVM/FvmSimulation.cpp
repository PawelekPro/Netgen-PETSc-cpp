#include "FvmSimulation.hpp"
#include "Globals.hpp"
#include "FvmParam.hpp"
#include "Model.hpp"
#include "MeshAlgorithm.hpp"
#include "MeshObject.hpp"
#include "parallel.hpp"
#include "FvmMeshToVtk.hpp"
#include "FvmMesh.hpp"

#include <petscsys.h>

#include <array>
#include <iostream>


FvmSimulation::FvmSimulation()
    : _model(std::make_unique<Model>()) {
}

void FvmSimulation::GenerateMesh(const std::string &filepath) const {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        _model->ImportSTEP(filepath);

        const auto meshAlgorithm = std::make_shared<MeshAlgorithm>();
        meshAlgorithm->maxSize = 2;
        meshAlgorithm->SetDim(MeshAlgorithm::ALG_3D);
        meshAlgorithm->quadAllowed = false;

        _model->SetMeshAlgorithm(meshAlgorithm);
        _model->GenerateMesh();
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void FvmSimulation::DecomposeMesh() const {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        if (processorsNb == 1)
            return;

        _model->GetMeshObject()->DecomposeMesh(processorsNb);
    }

    MPI_Barrier(MPI_COMM_WORLD);
}

int FvmSimulation::ConstructGlobalFvmMesh() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        auto mesh = _model->GetMeshObject();
        // _model->SaveMeshToFile("meshFile.vol");

        try {
            _globalFvmMesh = std::make_shared<FvmMeshContainer>(mesh);
            // auto fvmToVtk = FvmMeshToVtk(_globalFvmMesh);
            // fvmToVtk.ConvertFvmMeshToVtk();
            // fvmToVtk.SaveVtkMeshToFile("vtkMeshFile.vtm");
        } catch (const FvmException &ex) {
            std::cerr << "Caught MeshException: " << ex.what()
                    << ", code: " << ex.code() << std::endl;
            return LOGICAL_ERROR;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    return LOGICAL_TRUE;
}

int FvmSimulation::Start(const std::string &filepath) {
    constexpr int size = ToInt(FieldIndex::Size);

    std::array<char, size> var = {'u', 'v', 'w', 'p', 'T', 's'};
    std::array<double, size> fres = {0.0};
    std::array<int, size> fiter = {0};

    int iter = 0;
    int irestart = 0;

    double wtime = 0.0, wdt = 0.0;
    double maxCp = 0.0;

    double Vc = 0.0, Vt = 0.0, Vcp = 0.0, Vtp = 0.0;
    double pf = 0.0;

    const std::string residualsFile = filepath + "/residuals";
    const std::string resultsFile = filepath + "/results";

    FILE *fpresults;
    FILE *fpresiduals;

    if (PetscFOpen(PETSC_COMM_WORLD, resultsFile.c_str(), "w", &fpresults) != 0) {
        std::cerr << "Failed to open results file: " << resultsFile << "\n";
        return LOGICAL_ERROR;
    }

    if (fvmParameter.steady == LOGICAL_TRUE) {
        if (PetscFOpen(PETSC_COMM_WORLD, residualsFile.c_str(), "w", &fpresiduals) != 0) {
            std::cerr << "Failed to open residuals file: " << residualsFile << "\n";
            PetscFClose(PETSC_COMM_WORLD, fpresults);
            return LOGICAL_ERROR;
        }
    }

    // PetscPrintf(PETSC_COMM_WORLD, "\n");
    // PetscPrintf(PETSC_COMM_WORLD, "Allocating memory...\n");

    double startTime, endTime, curTime, dt;
    startTime = fvmParameter.t0;
    endTime = fvmParameter.t1;
    curTime = fvmParameter.t0;
    dt = fvmParameter.dt;

    // Open the output file for results
    if (fvmParameter.wbinary == LOGICAL_TRUE) {
        fpresults = fopen(resultsFile.c_str(), "wb");
        fprintf(fpresults, "$PostFormat\n");
        fprintf(fpresults, "%g %d %lu\n", 1.0, 1, sizeof(double));
        fprintf(fpresults, "$EndPostFormat\n");
    } else {
        fpresults = fopen(resultsFile.c_str(), "w");
        fprintf(fpresults, "$PostFormat\n");
        fprintf(fpresults, "%g %d %lu\n", 1.0, 0, sizeof(double));
        fprintf(fpresults, "$EndPostFormat\n");
    }

    return LOGICAL_TRUE;
}
