static char help[] = "Stokes Problem discretized with finite elements,\n\
using a parallel unstructured mesh (DMPLEX) to represent the domain.\n\n\n";

/*
For the isoviscous Stokes problem, which we discretize using the finite
element method on an unstructured mesh, the weak form equations are

< \nabla v, \nabla u + {\nabla u}^T > - < \nabla\cdot v, p > - < v, f > = 0
< q, -\nabla\cdot u >
*/

#include <petscdmplex.h>
#include <petscsnes.h>
#include <petscds.h>
#include <petscbag.h>

typedef enum {
    SOL_QUADRATIC,
    SOL_TRIG,
    SOL_UNKNOWN
} SolType;

const char *SolTypes[] = {"quadratic", "trig", "unknown", "SolType", "SOL_", nullptr};

typedef struct {
    PetscScalar mu; /* dynamic shear viscosity */
} Parameter;

typedef struct {
    PetscBag bag; /* Problem parameters */
    SolType sol; /* MMS solution */
} AppCtx;

static void f1_u(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                 const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[], const PetscInt aOff[],
                 const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
                 PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[],
                 PetscScalar f1[]) {
    const PetscReal mu = PetscRealPart(constants[0]);
    const PetscInt Nc = uOff[1] - uOff[0];
    PetscInt c, d;

    for (c = 0; c < Nc; ++c) {
        for (d = 0; d < dim; ++d) f1[c * dim + d] = mu * (u_x[c * dim + d] + u_x[d * dim + c]);
        f1[c * dim + c] -= u[uOff[1]];
    }
}

static void f0_p(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                 const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[], const PetscInt aOff[],
                 const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
                 PetscReal t, const PetscReal x[], PetscInt numConstants, const PetscScalar constants[],
                 PetscScalar f0[]) {
    PetscInt d;
    for (d = 0, f0[0] = 0.0; d < dim; ++d) f0[0] -= u_x[d * dim + d];
}

static void g1_pu(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                  const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[], const PetscInt aOff[],
                  const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
                  PetscReal t, PetscReal u_tShift, const PetscReal x[], PetscInt numConstants,
                  const PetscScalar constants[], PetscScalar g1[]) {
    PetscInt d;
    for (d = 0; d < dim; ++d) g1[d * dim + d] = -1.0; /* < q, -\nabla\cdot u > */
}

static void g2_up(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                  const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[], const PetscInt aOff[],
                  const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
                  PetscReal t, PetscReal u_tShift, const PetscReal x[], PetscInt numConstants,
                  const PetscScalar constants[], PetscScalar g2[]) {
    PetscInt d;
    for (d = 0; d < dim; ++d) g2[d * dim + d] = -1.0; /* -< \nabla\cdot v, p > */
}

static void g3_uu(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                  const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[], const PetscInt aOff[],
                  const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
                  PetscReal t, PetscReal u_tShift, const PetscReal x[], PetscInt numConstants,
                  const PetscScalar constants[], PetscScalar g3[]) {
    const PetscReal mu = PetscRealPart(constants[0]);
    const PetscInt Nc = uOff[1] - uOff[0];
    PetscInt c, d;

    for (c = 0; c < Nc; ++c) {
        for (d = 0; d < dim; ++d) {
            g3[((c * Nc + c) * dim + d) * dim + d] += mu; /* < \nabla v, \nabla u > */
            g3[((c * Nc + d) * dim + d) * dim + c] += mu; /* < \nabla v, {\nabla u}^T > */
        }
    }
}


static void g0_pp(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                  const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[], const PetscInt aOff[],
                  const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
                  PetscReal t, PetscReal u_tShift, const PetscReal x[], PetscInt numConstants,
                  const PetscScalar constants[], PetscScalar g0[]) {
    const PetscReal mu = PetscRealPart(constants[0]);

    g0[0] = 1.0 / mu;
}

/* Quadratic MMS Solution
   2D:

     u = x^2 + y^2
     v = 2 x^2 - 2xy
     p = x + y - 1
     f = <1 - 4 mu, 1 - 4 mu>

   so that

     e(u) = (grad u + grad u^T) = / 4x  4x \
                                  \ 4x -4x /
     div mu e(u) - \nabla p + f = mu <4, 4> - <1, 1> + <1 - 4 mu, 1 - 4 mu> = 0
     \nabla \cdot u             = 2x - 2x = 0

   3D:

     u = 2 x^2 + y^2 + z^2
     v = 2 x^2 - 2xy
     w = 2 x^2 - 2xz
     p = x + y + z - 3/2
     f = <1 - 8 mu, 1 - 4 mu, 1 - 4 mu>

   so that

     e(u) = (grad u + grad u^T) = / 8x  4x  4x \
                                  | 4x -4x  0  |
                                  \ 4x  0  -4x /
     div mu e(u) - \nabla p + f = mu <8, 4, 4> - <1, 1, 1> + <1 - 8 mu, 1 - 4 mu, 1 - 4 mu> = 0
     \nabla \cdot u             = 4x - 2x - 2x = 0
*/

static PetscErrorCode quadratic_u(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u,
                                  void *ctx) {
    PetscInt c;

    u[0] = (dim - 1) * PetscSqr(x[0]);
    for (c = 1; c < Nc; ++c) {
        u[0] += PetscSqr(x[c]);
        u[c] = 2.0 * PetscSqr(x[0]) - 2.0 * x[0] * x[c];
    }
    return PETSC_SUCCESS;
}

static PetscErrorCode quadratic_p(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u,
                                  void *ctx) {
    PetscInt d;

    u[0] = -0.5 * dim;
    for (d = 0; d < dim; ++d) u[0] += x[d];
    return PETSC_SUCCESS;
}

static void f0_quadratic_u(PetscInt dim, PetscInt Nf, PetscInt NfAux, const PetscInt uOff[], const PetscInt uOff_x[],
                           const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
                           const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[],
                           const PetscScalar a_t[], const PetscScalar a_x[], PetscReal t, const PetscReal x[],
                           PetscInt numConstants, const PetscScalar constants[], PetscScalar f0[]) {
    const PetscReal mu = PetscRealPart(constants[0]);
    PetscInt d;

    f0[0] = (dim - 1) * 4.0 * mu - 1.0;
    for (d = 1; d < dim; ++d) f0[d] = 4.0 * mu - 1.0;
}

static PetscErrorCode ProcessOptions(MPI_Comm comm, AppCtx *options) {
    PetscInt sol;

    PetscFunctionBeginUser;
    options->sol = SOL_QUADRATIC;
    PetscOptionsBegin(comm, "", "Stokes Problem Options", "DMPLEX");
        sol = options->sol;
        PetscCall(
            PetscOptionsEList("-sol", "The MMS solution", "ex62.c", SolTypes, PETSC_STATIC_ARRAY_LENGTH(SolTypes) - 3,
                SolTypes[options->sol], &sol, NULL));
        options->sol = (SolType) sol;
    PetscOptionsEnd();
    PetscFunctionReturn(PETSC_SUCCESS);
}

static PetscErrorCode CreateMesh(MPI_Comm comm, AppCtx *user, DM *dm) {
    PetscFunctionBeginUser;
    PetscCall(DMCreate(comm, dm));
    PetscCall(DMSetType(*dm, DMPLEX));
    PetscCall(DMSetFromOptions(*dm));
    PetscCall(DMViewFromOptions(*dm, NULL, "-dm_view"));
    PetscFunctionReturn(PETSC_SUCCESS);
}


static PetscErrorCode SetupParameters(MPI_Comm comm, AppCtx *ctx) {
    Parameter *p;

    PetscFunctionBeginUser;
    /* setup PETSc parameter bag */
    PetscCall(PetscBagCreate(PETSC_COMM_SELF, sizeof(Parameter), &ctx->bag));
    PetscCall(PetscBagGetData(ctx->bag, (void **)&p));
    PetscCall(PetscBagSetName(ctx->bag, "par", "Stokes Parameters"));
    PetscCall(PetscBagRegisterScalar(ctx->bag, &p->mu, 1.0, "mu", "Dynamic Shear Viscosity, Pa s"));
    PetscCall(PetscBagSetFromOptions(ctx->bag)); {
        PetscViewer viewer;
        PetscViewerFormat format;
        PetscBool flg;

        PetscCall(PetscOptionsCreateViewer(comm, NULL, NULL, "-param_view", &viewer, &format, &flg));
        if (flg) {
            PetscCall(PetscViewerPushFormat(viewer, format));
            PetscCall(PetscBagView(ctx->bag, viewer));
            PetscCall(PetscViewerFlush(viewer));
            PetscCall(PetscViewerPopFormat(viewer));
            PetscCall(PetscViewerDestroy(&viewer));
        }
    }
    PetscFunctionReturn(PETSC_SUCCESS);
}


static PetscErrorCode SetupEqn(DM dm, AppCtx *user) {
    PetscErrorCode (*exactFuncs[2])(PetscInt, PetscReal, const PetscReal [], PetscInt, PetscScalar *, void *);
    PetscDS ds;
    DMLabel label;
    const PetscInt id = 1;

    PetscFunctionBeginUser;
    PetscCall(DMGetDS(dm, &ds));
    switch (user->sol) {
        case SOL_QUADRATIC:
            PetscCall(PetscDSSetResidual(ds, 0, f0_quadratic_u, f1_u));
            exactFuncs[0] = quadratic_u;
            exactFuncs[1] = quadratic_p;
            break;
        default:
            SETERRQ(PetscObjectComm(reinterpret_cast<PetscObject>(dm)), PETSC_ERR_ARG_WRONG,
                    "Unsupported solution type: %s (%d)",
                    SolTypes[PetscMin(user->sol, SOL_UNKNOWN)], user->sol);
    }
    PetscCall(PetscDSSetResidual(ds, 1, f0_p, nullptr));
    PetscCall(PetscDSSetJacobian(ds, 0, 0, nullptr, nullptr, nullptr, g3_uu));
    PetscCall(PetscDSSetJacobian(ds, 0, 1, nullptr, nullptr, g2_up, nullptr));
    PetscCall(PetscDSSetJacobian(ds, 1, 0, nullptr, g1_pu, nullptr, nullptr));
    PetscCall(PetscDSSetJacobianPreconditioner(
        ds, 0, 0, nullptr, nullptr, nullptr, g3_uu));
    PetscCall(PetscDSSetJacobianPreconditioner(
        ds, 1, 1, g0_pp, nullptr, nullptr, nullptr));

    PetscCall(PetscDSSetExactSolution(ds, 0, exactFuncs[0], user));
    PetscCall(PetscDSSetExactSolution(ds, 1, exactFuncs[1], user));

    PetscCall(DMGetLabel(dm, "marker", &label));
    PetscCall(
        DMAddBoundary(
            dm, DM_BC_ESSENTIAL, "wall", label, 1, &id, 0, 0, nullptr,
            reinterpret_cast<void (*)()>(exactFuncs[0]), nullptr, user, nullptr));

    /* Make constant values available to pointwise functions */
    {
        Parameter *param;
        PetscScalar constants[1];

        PetscCall(PetscBagGetData(user->bag, (void **)&param));
        constants[0] = param->mu; /* dynamic shear viscosity, Pa s */
        PetscCall(PetscDSSetConstants(ds, 1, constants));
    }
    PetscFunctionReturn(PETSC_SUCCESS);
}

static PetscErrorCode zero(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u, void *ctx) {
    PetscInt c;
    for (c = 0; c < Nc; ++c) u[c] = 0.0;
    return PETSC_SUCCESS;
}

static PetscErrorCode one(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nc, PetscScalar *u, void *ctx) {
    PetscInt c;
    for (c = 0; c < Nc; ++c) u[c] = 1.0;
    return PETSC_SUCCESS;
}


static PetscErrorCode CreatePressureNullSpace(DM dm, PetscInt origField, PetscInt field, MatNullSpace *nullspace) {
    Vec vec;
    PetscErrorCode (*funcs[2])(PetscInt dim, PetscReal time, const PetscReal x[], PetscInt Nf, PetscScalar *u,
                               void *ctx) = {zero, one};

    PetscFunctionBeginUser;
    PetscCheck(origField == 1, PetscObjectComm((PetscObject)dm), PETSC_ERR_ARG_WRONG,
               "Field %" PetscInt_FMT " should be 1 for pressure", origField);
    funcs[field] = one; {
        PetscDS ds;
        PetscCall(DMGetDS(dm, &ds));
        PetscCall(PetscObjectViewFromOptions((PetscObject)ds, NULL, "-ds_view"));
    }
    PetscCall(DMCreateGlobalVector(dm, &vec));
    PetscCall(DMProjectFunction(dm, 0.0, funcs, NULL, INSERT_ALL_VALUES, vec));
    PetscCall(VecNormalize(vec, NULL));
    PetscCall(MatNullSpaceCreate(PetscObjectComm((PetscObject)dm), PETSC_FALSE, 1, &vec, nullspace));
    PetscCall(VecDestroy(&vec));
    /* New style for field null spaces */
    {
        PetscObject pressure;
        MatNullSpace nullspacePres;

        PetscCall(DMGetField(dm, field, NULL, &pressure));
        PetscCall(MatNullSpaceCreate(PetscObjectComm(pressure), PETSC_TRUE, 0, NULL, &nullspacePres));
        PetscCall(PetscObjectCompose(pressure, "nullspace", (PetscObject)nullspacePres));
        PetscCall(MatNullSpaceDestroy(&nullspacePres));
    }
    PetscFunctionReturn(PETSC_SUCCESS);
}

static PetscErrorCode SetupProblem(DM dm, PetscErrorCode (*setupEqn)(DM, AppCtx *), AppCtx *user) {
    DM cdm = dm;
    PetscQuadrature q = NULL;
    PetscBool simplex;
    PetscInt dim, Nf = 2, f, Nc[2];
    const char *name[2] = {"velocity", "pressure"};
    const char *prefix[2] = {"vel_", "pres_"};

    PetscFunctionBegin;
    PetscCall(DMGetDimension(dm, &dim));
    PetscCall(DMPlexIsSimplex(dm, &simplex));
    Nc[0] = dim;
    Nc[1] = 1;
    for (f = 0; f < Nf; ++f) {
        PetscFE fe;

        PetscCall(PetscFECreateDefault(PETSC_COMM_SELF, dim, Nc[f], simplex, prefix[f], -1, &fe));
        PetscCall(PetscObjectSetName((PetscObject)fe, name[f]));
        if (!q)
            PetscCall(PetscFEGetQuadrature(fe, &q));
        PetscCall(PetscFESetQuadrature(fe, q));
        PetscCall(DMSetField(dm, f, NULL, (PetscObject)fe));
        PetscCall(PetscFEDestroy(&fe));
    }
    PetscCall(DMCreateDS(dm));
    PetscCall((*setupEqn)(dm, user));
    while (cdm) {
        PetscCall(DMCopyDisc(dm, cdm));
        PetscCall(DMSetNullSpaceConstructor(cdm, 1, CreatePressureNullSpace));
        PetscCall(DMGetCoarseDM(cdm, &cdm));
    }
    PetscFunctionReturn(PETSC_SUCCESS);
}

int main(int argc, char **argv) {
    SNES snes;
    DM dm;
    Vec u;
    AppCtx user;

    PetscFunctionBeginUser;
    PetscCall(PetscInitialize(&argc, &argv, NULL, help));
    PetscCall(ProcessOptions(PETSC_COMM_WORLD, &user));
    PetscCall(CreateMesh(PETSC_COMM_WORLD, &user, &dm));
    PetscCall(SNESCreate(PetscObjectComm((PetscObject)dm), &snes));
    PetscCall(SNESSetDM(snes, dm));
    PetscCall(DMSetApplicationContext(dm, &user));

    PetscCall(SetupParameters(PETSC_COMM_WORLD, &user));
    PetscCall(SetupProblem(dm, SetupEqn, &user));
    PetscCall(DMPlexCreateClosureIndex(dm, NULL));

    PetscCall(DMCreateGlobalVector(dm, &u));
    PetscCall(DMPlexSetSNESLocalFEM(dm, PETSC_FALSE, &user));
    PetscCall(SNESSetFromOptions(snes));
    PetscCall(DMSNESCheckFromOptions(snes, u));
    PetscCall(PetscObjectSetName((PetscObject)u, "Solution")); {
        Mat J;
        MatNullSpace sp;

        PetscCall(SNESSetUp(snes));
        PetscCall(CreatePressureNullSpace(dm, 1, 1, &sp));
        PetscCall(SNESGetJacobian(snes, &J, NULL, NULL, NULL));
        PetscCall(MatSetNullSpace(J, sp));
        PetscCall(MatNullSpaceDestroy(&sp));
        PetscCall(PetscObjectSetName((PetscObject)J, "Jacobian"));
        PetscCall(MatViewFromOptions(J, NULL, "-J_view"));
    }
    PetscCall(SNESSolve(snes, NULL, u));

    PetscViewer viewer;
    PetscViewerVTKOpen(PETSC_COMM_WORLD, "solution.vtu", FILE_MODE_WRITE, &viewer);
    VecView(u, viewer);
    PetscViewerDestroy(&viewer);

    PetscCall(VecDestroy(&u));
    PetscCall(SNESDestroy(&snes));
    PetscCall(DMDestroy(&dm));
    PetscCall(PetscBagDestroy(&user.bag));
    PetscCall(PetscFinalize());
    return 0;
}

