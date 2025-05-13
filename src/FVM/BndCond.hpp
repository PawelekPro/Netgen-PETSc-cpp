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
    std::string label;
    int physReg;

    BndCondType bc;

    std::string fu, fv, fw;
    std::string fp;
    std::string fT;
    std::string fs;
};

struct BcdVolume : BcdSurface {
};

class BoundaryConditions {
public:
    BoundaryConditions();

    static BoundaryConditions &Instance() {
        static BoundaryConditions instance;
        return instance;
    }

    void RegisterBoundaryCondition(const BcdSurface &bcs);

    void RegisterBoundaryCondition(const BcdVolume &bcs);

    void Clear();

private:
    std::vector<BcdSurface> _surfaceRegions;
    std::vector<BcdVolume> _volumeRegions;

    BoundaryConditions(const BoundaryConditions &) = delete;

    BoundaryConditions &operator=(const BoundaryConditions &) = delete;

    BoundaryConditions(BoundaryConditions &&) = delete;

    BoundaryConditions &operator=(BoundaryConditions &&) = delete;
};

#endif
