#ifndef FVMVARIABLES_HPP
#define FVMVARIABLES_HPP

#include "petscksp.h"

class FvmVar {
public:
    static Vec cex, cey, cez;
    static Vec cexl, ceyl, cezl;

    static Vec Co;
    static Vec Col;

    static Vec uf;

    static Vec dens, visc, spheat, thcond;
    static Vec densl, viscl, spheatl, thcondl;

    static Vec xu0, xv0, xw0, xp0, xT0, xs0;
    static Vec xu0l, xv0l, xw0l, xp0l, xT0l, xs0l;

    static Vec xu, xv, xw, xp, xT, xs;
    static Vec xul, xvl, xwl, xpl, xTl, xsl;

    static Vec xuf, xvf, xwf, xpf, xTf, xsf;

    static Mat Am, Ac, Ae, As;

    static Vec bu, bv, bw, bp, bT, bs;

    static Vec hu, hv, hw;
    static Vec hul, hvl, hwl;

    static Vec ap;
    static Vec apl;

    static Vec xpp, xTp;

    static Vec betaf;

    static Vec temp1, temp2;

    static Vec xsm;
    static Vec xsml;
    static Vec xsmf;
};


#endif
