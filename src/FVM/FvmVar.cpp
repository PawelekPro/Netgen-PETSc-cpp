#include "FvmVar.hpp"
#include "FvmVector.hpp"
#include "FvmMesh.hpp"

#define DESTROY_MAT(m) if ((m) != nullptr) { MatDestroy(&(m)); (m) = nullptr; }
#define DESTROY_VEC(v) if ((v) != nullptr) { VecDestroy(&(v)); (v) = nullptr; }

Vec FvmVar::cex;
Vec FvmVar::cey;
Vec FvmVar::cez;
Vec FvmVar::cexl;
Vec FvmVar::ceyl;
Vec FvmVar::cezl;

Vec FvmVar::Co;
Vec FvmVar::Col;

Vec FvmVar::uf;

Vec FvmVar::dens;
Vec FvmVar::visc;
Vec FvmVar::spheat;
Vec FvmVar::thcond;

Vec FvmVar::densl;
Vec FvmVar::viscl;
Vec FvmVar::spheatl;
Vec FvmVar::thcondl;

Vec FvmVar::xu0;
Vec FvmVar::xv0;
Vec FvmVar::xw0;
Vec FvmVar::xp0;
Vec FvmVar::xT0;
Vec FvmVar::xs0;

Vec FvmVar::xu0l;
Vec FvmVar::xv0l;
Vec FvmVar::xw0l;
Vec FvmVar::xp0l;
Vec FvmVar::xT0l;
Vec FvmVar::xs0l;

Vec FvmVar::xu;
Vec FvmVar::xv;
Vec FvmVar::xw;
Vec FvmVar::xp;
Vec FvmVar::xT;
Vec FvmVar::xs;

Vec FvmVar::xul;
Vec FvmVar::xvl;
Vec FvmVar::xwl;
Vec FvmVar::xpl;
Vec FvmVar::xTl;
Vec FvmVar::xsl;

Vec FvmVar::xuf;
Vec FvmVar::xvf;
Vec FvmVar::xwf;
Vec FvmVar::xpf;
Vec FvmVar::xTf;
Vec FvmVar::xsf;

Mat FvmVar::Am;
Mat FvmVar::Ac;
Mat FvmVar::Ae;
Mat FvmVar::As;

Vec FvmVar::bu;
Vec FvmVar::bv;
Vec FvmVar::bw;
Vec FvmVar::bp;
Vec FvmVar::bT;
Vec FvmVar::bs;

Vec FvmVar::hu;
Vec FvmVar::hv;
Vec FvmVar::hw;

Vec FvmVar::hul;
Vec FvmVar::hvl;
Vec FvmVar::hwl;

Vec FvmVar::ap;
Vec FvmVar::apl;

Vec FvmVar::xpp;
Vec FvmVar::xTp;

Vec FvmVar::betaf;

Vec FvmVar::temp1;
Vec FvmVar::temp2;

Vec FvmVar::xsm;
Vec FvmVar::xsml;
Vec FvmVar::xsmf;

FvmVar::FvmVar(const std::shared_ptr<FvmMeshContainer> &fvmMesh)
    : _fvmMesh(fvmMesh) {
    const int elementsNb = fvmMesh->elementsNb;
    const int facesNb = fvmMesh->facesNb;

    FvmVector::V_Constr(&cex, elementsNb, 0);
    FvmVector::V_Constr(&cey, elementsNb, 0);
    FvmVector::V_Constr(&cez, elementsNb, 0);

    FvmVector::V_Constr(&Co, elementsNb, 0);
    FvmVector::V_Constr(&uf, facesNb, 1);

    FvmVector::V_Constr(&dens, elementsNb, 0);
    FvmVector::V_Constr(&visc, elementsNb, 0);
    FvmVector::V_Constr(&thcond, elementsNb, 0);
    FvmVector::V_Constr(&spheat, elementsNb, 0);

    FvmVector::V_Constr(&xu0, elementsNb, 0);
    FvmVector::V_Constr(&xv0, elementsNb, 0);
    FvmVector::V_Constr(&xw0, elementsNb, 0);
    FvmVector::V_Constr(&xp0, elementsNb, 0);
    FvmVector::V_Constr(&xT0, elementsNb, 0);
    FvmVector::V_Constr(&xs0, elementsNb, 0);

    FvmVector::V_Constr(&xu, elementsNb, 0);
    FvmVector::V_Constr(&xv, elementsNb, 0);
    FvmVector::V_Constr(&xw, elementsNb, 0);
    FvmVector::V_Constr(&xp, elementsNb, 0);
    FvmVector::V_Constr(&xT, elementsNb, 0);
    FvmVector::V_Constr(&xs, elementsNb, 0);

    FvmVector::V_Constr(&xuf, facesNb, 1);
    FvmVector::V_Constr(&xvf, facesNb, 1);
    FvmVector::V_Constr(&xwf, facesNb, 1);
    FvmVector::V_Constr(&xpf, facesNb, 1);
    FvmVector::V_Constr(&xTf, facesNb, 1);
    FvmVector::V_Constr(&xsf, facesNb, 1);

    FvmVector::V_Constr(&ap, elementsNb, 0);
    FvmVector::V_Constr(&hu, elementsNb, 0);
    FvmVector::V_Constr(&hv, elementsNb, 0);
    FvmVector::V_Constr(&hw, elementsNb, 0);

    FvmVector::V_Constr(&temp1, elementsNb, 0);
    FvmVector::V_Constr(&temp2, elementsNb, 0);

    FvmVector::V_Constr(&xsm, elementsNb, 0);
    FvmVector::V_Constr(&xsmf, facesNb, 0);
}

void FvmVar::Deallocate() {
    DESTROY_VEC(cex);
    DESTROY_VEC(cey);
    DESTROY_VEC(cez);
    DESTROY_VEC(cexl);
    DESTROY_VEC(ceyl);
    DESTROY_VEC(cezl);

    DESTROY_VEC(Co);
    DESTROY_VEC(Col);

    DESTROY_VEC(uf);

    DESTROY_VEC(dens);
    DESTROY_VEC(visc);
    DESTROY_VEC(spheat);
    DESTROY_VEC(thcond);
    DESTROY_VEC(densl);
    DESTROY_VEC(viscl);
    DESTROY_VEC(spheatl);
    DESTROY_VEC(thcondl);

    DESTROY_VEC(xu0);
    DESTROY_VEC(xv0);
    DESTROY_VEC(xw0);
    DESTROY_VEC(xp0);
    DESTROY_VEC(xT0);
    DESTROY_VEC(xs0);
    DESTROY_VEC(xu0l);
    DESTROY_VEC(xv0l);
    DESTROY_VEC(xw0l);
    DESTROY_VEC(xp0l);
    DESTROY_VEC(xT0l);
    DESTROY_VEC(xs0l);

    DESTROY_VEC(xu);
    DESTROY_VEC(xv);
    DESTROY_VEC(xw);
    DESTROY_VEC(xp);
    DESTROY_VEC(xT);
    DESTROY_VEC(xs);
    DESTROY_VEC(xul);
    DESTROY_VEC(xvl);
    DESTROY_VEC(xwl);
    DESTROY_VEC(xpl);
    DESTROY_VEC(xTl);
    DESTROY_VEC(xsl);

    DESTROY_VEC(xuf);
    DESTROY_VEC(xvf);
    DESTROY_VEC(xwf);
    DESTROY_VEC(xpf);
    DESTROY_VEC(xTf);
    DESTROY_VEC(xsf);

    DESTROY_VEC(bu);
    DESTROY_VEC(bv);
    DESTROY_VEC(bw);
    DESTROY_VEC(bp);
    DESTROY_VEC(bT);
    DESTROY_VEC(bs);

    DESTROY_VEC(hu);
    DESTROY_VEC(hv);
    DESTROY_VEC(hw);
    DESTROY_VEC(hul);
    DESTROY_VEC(hvl);
    DESTROY_VEC(hwl);

    DESTROY_VEC(ap);
    DESTROY_VEC(apl);

    DESTROY_VEC(xpp);
    DESTROY_VEC(xTp);

    DESTROY_VEC(betaf);

    DESTROY_VEC(temp1);
    DESTROY_VEC(temp2);

    DESTROY_VEC(xsm);
    DESTROY_VEC(xsml);
    DESTROY_VEC(xsmf);

    // Mats
    DESTROY_MAT(Am);
    DESTROY_MAT(Ac);
    DESTROY_MAT(Ae);
    DESTROY_MAT(As);
}
