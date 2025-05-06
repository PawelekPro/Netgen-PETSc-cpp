#ifndef FVMPARAM_HPP
#define FVMPARAM_HPP

#include <array>
#include <string>

class FvmParameter {
public:
    std::string ulength = "m";
    std::string umass = "kg";
    std::string utime = "s";
    std::string uenergy = "J";
    std::string utemperature = "K";

    int inertia = 1;

    std::array<float, 6> ef{1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float dfactor = 1.0f;
    float st = 1.0f;

    std::array<int, 6> timemethod{1, 1, 1, 1, 1, 1};
    std::array<int, 6> scheme{1, 1, 1, 1, 1, 1};

    int steady = 0;
    std::array<float, 6> ftol{1e-6f, 1e-6f, 1e-6f, 1e-6f, 1e-6f, 1e-6f};

    int wbinary = 0;
    int nsav = 1;
    std::array<int, 6> calc{0, 0, 0, 0, 0, 0};
    int savflux = 0;
    std::array<int, 6> fsav{0, 0, 0, 0, 0, 0};
    std::array<int, 6> csav{0, 0, 0, 0, 0, 0};
    std::array<int, 6> probe{0, 0, 0, 0, 0, 0};

    int smooth = 1;
    std::array<int, 3> vortex{0, 0, 0};
    int streamf = 0;

    int fvec = 0;
    int cvec = 0;

    float kq = 2.0f;
    int ncicsamcor = 2;

    std::array<float, 3> g{0.0f, 0.0f, 0.0f};

    std::array<int, 6> msolver{8, 8, 8, 8, 8, 3};
    std::array<int, 6> mprecond{4, 4, 4, 4, 4, 4};

    int northocor = 10;
    float orthof = 1.0f;

    std::array<float, 6> mtol{1e-8f, 1e-8f, 1e-8f, 1e-8f, 1e-8f, 1e-8f};
    std::array<int, 6> miter{500, 500, 500, 500, 500, 500};

    int restart = 10000;
    int adjdt = 0;
    float maxCp = 0.25f;

    float t0 = 0.0f;
    float t1 = 0.001f;
    float dt = 0.001f;

    int fill = 0;
    float pf = 99.5f;

    int intbcphysreg = -1;
};

extern FvmParameter fvmParameter;

#endif
