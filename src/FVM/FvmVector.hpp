#ifndef FVMVECTOR_HPP
#define FVMVECTOR_HPP

#include <vector>

#include "petscksp.h"


class FvmVector {
public:
    static void Init(const int ghostsNb, const std::vector<int> &ghostsVec) {
        if (!_instance) {
            _instance = new FvmVector(ghostsNb, ghostsVec);
        }
    }

    static FvmVector &Instance() {
        if (!_instance) {
            throw std::runtime_error("FvmVector not initialized. Call Init first.");
        }
        return *_instance;
    }

    static void V_Constr(Vec *v, int n, int sequential);

    static void V_SetCmp(const Vec *v, int ind, double value);

    // static void V_SetAllCmp(Vec *v, double value);

    static double V_GetCmp(const Vec *v, int ind);

private:
    FvmVector(const int ghostsNb, const std::vector<int> &ghostsVec);

private:
    int _ghostsNb = 0;
    std::vector<int> _ghostsVec;

    static FvmVector *_instance;
};


#endif
