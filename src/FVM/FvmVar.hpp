#ifndef FVMVARIABLES_HPP
#define FVMVARIABLES_HPP

#include <memory>

#include "petscksp.h"

class FvmMeshContainer;

class FvmVar {
public:
    explicit FvmVar(const std::shared_ptr<FvmMeshContainer> &fvmMesh);

    ~FvmVar() = default;

    static void Deallocate();

    static Vec cex, cey, cez; // Cell centers components
    static Vec cexl, ceyl, cezl;

    static Vec Co; // Courant number
    static Vec Col;

    static Vec uf; // Face flux velocity

    static Vec dens, spheat, thcond; // Density, Thermal conductivity, Specific heat
    static Vec visc; // Dynamic viscosity

    static Vec densl, viscl, spheatl, thcondl;

    static Vec xu0, xv0, xw0, xp0, xT0, xs0; // Values at a cell centre (previous time step)
    static Vec xu0l, xv0l, xw0l, xp0l, xT0l, xs0l;

    static Vec xu, xv, xw, xp, xT, xs; // Values at cell centres
    static Vec xul, xvl, xwl, xpl, xTl, xsl;

    static Vec xuf, xvf, xwf, xpf, xTf, xsf; // Values at face center

    static Mat Am, Ac, Ae, As;

    static Vec bu, bv, bw, bp, bT, bs;

    static Vec hu, hv, hw; // Momentum matrix source components without pressure
    static Vec hul, hvl, hwl;

    static Vec ap; // Momentum matrix diagonal
    static Vec apl;

    static Vec xpp, xTp;

    static Vec betaf;

    static Vec temp1, temp2; // Temporary vectors

    static Vec xsm; // Smoothed gamma at cell center
    static Vec xsml;
    static Vec xsmf; // Smoothed gamma  at face center

private:
    std::shared_ptr<FvmMeshContainer> _fvmMesh;
};


#endif
