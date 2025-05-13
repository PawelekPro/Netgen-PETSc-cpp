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

    double fu, fv, fw;
    double fp;
    double fT;
    double fs;
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

    [[nodiscard]] std::vector<BcdSurface> GetSurfaceRegions() const;

    [[nodiscard]] std::vector<BcdVolume> GetVolumeRegions() const;

    BoundaryConditions(const BoundaryConditions &) = delete;

    BoundaryConditions &operator=(const BoundaryConditions &) = delete;

    BoundaryConditions(BoundaryConditions &&) = delete;

    BoundaryConditions &operator=(BoundaryConditions &&) = delete;

private:
    std::vector<BcdSurface> _surfaceRegions;
    std::vector<BcdVolume> _volumeRegions;
};

#endif
