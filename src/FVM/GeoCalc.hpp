#ifndef GEOCALC_HPP
#define GEOCALC_HPP

namespace FvmMesh {
    class Vector3;
}

using namespace FvmMesh;

Vector3 GeoCrossVector(const Vector3 &v1, const Vector3 &v2);

Vector3 GeoCalcNormal(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3);

Vector3 GeoSubVectorVector(const Vector3 &n2, const Vector3 &n1);

Vector3 GeoNormalizeVector(const Vector3 &v1);

Vector3 GeoCalcCentroid3(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3);

Vector3 GeoCalcCentroid4(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3, const Vector3 &n4);

Vector3 GeoCalcCentroid6(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6);

Vector3 GeoCalcCentroid8(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6,
    const Vector3 &n7, const Vector3 &n8);

double GeoMagVector(const Vector3 &v1);

double GeoCalcTriArea(const Vector3 &n1, const Vector3 &n2, const Vector3 &n3);

double GeoCalcQuadArea(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3, const Vector3 &n4);

double GeoCalcTetraVolume(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3, const Vector3 &n4);

double GeoCalcHexaVolume(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6,
    const Vector3 &n7, const Vector3 &n8);

double GeoCalcPrismVolume(
    const Vector3 &n1, const Vector3 &n2, const Vector3 &n3,
    const Vector3 &n4, const Vector3 &n5, const Vector3 &n6);

#endif
