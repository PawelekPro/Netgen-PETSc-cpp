#ifndef MESHPARAMETERSCOMPUTE_HPP
#define MESHPARAMETERSCOMPUTE_HPP

class TopoDS_Shape;
class TopoDS_Edge;

class MeshParametersCompute {
public:
    static double GetDefaultMinSize(const TopoDS_Shape &geom, double maxSize);

    static double EdgeLength(const TopoDS_Edge &E);
};

#endif
