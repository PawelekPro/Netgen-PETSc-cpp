#ifndef FVMVECTOR_HPP
#define FVMVECTOR_HPP

#include <vector>

#include "petscksp.h"

class FvmVector {
public:
    FvmVector(int ghostsNb, const std::vector<int> &ghostsVec);

    // static void V_Constr(Vec *v, int n, int sequential);

    static void V_SetCmp(const Vec *v, int ind, double value);

    // static void V_SetAllCmp(Vec *v, double value);

    // static double V_GetCmp(Vec *v, int ind);

private:
    int _ghostsNb = 0;
    std::vector<int> _ghostsVec;
};


#endif
