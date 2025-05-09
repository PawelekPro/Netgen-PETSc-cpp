#ifndef BOUNDARYCONDITIONS_HPP
#define BOUNDARYCONDITIONS_HPP

#include <string>
#include <vector>

enum class BndCondType {
    NONE = 0,
    EMPTY,
    CYCLIC,
    PROCESSOR,
    OPEN,
    INLET,
    PRESSUREINLET,
    OUTLET,
    ADIABATICWALL,
    MOVINGWALL,
    WALL,
    SLIP,
    PERMEABLE,
    CONSTRAINTU,
    CONSTRAINTV,
    CONSTRAINTW,
    CONSTRAINT,
    PRESSURE,
    SURFACE,
    VOLUME
};

struct BcdVector {
    double x = 0.0, y = 0.0, z = 0.0;
};

struct BcdSurface {
    int physReg;

    int bc;

    std::string fu, fv, fw;
    std::string fp;
    std::string fT;
    std::string fs;
};

struct BcdVolume {
    int physReg;

    int bc;

    std::string fu, fv, fw;
    std::string fp;
    std::string fT;
    std::string fs;
};

class BoundaryConditions {
public:
    std::vector<BcdSurface> surfaceRegions;
    std::vector<BcdVolume> volumeRegions;

    bool ConstructBD();
};

#endif
