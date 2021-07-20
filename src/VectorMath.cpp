#include "VectorMath.h"

namespace Monte {

float Vector2D::getMagnitude(){
    return sqrtf(dx*dx + dy*dy);
}

float Vector2D::getMagnitudeSquared(){
    return dx*dx + dy*dy;
}

float Vector3D::getMagnitude(){
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

float Vector3D::getMagnitudeSquared(){
    // TODO: could this be optimised by doing return Vector2D::getMagnitudeSquared() + dz*dz; ?
    return dx*dx + dy*dy + dz*dz;
}

Vector2D getUnitVector2D(Vector2D u){
    return u / u.getMagnitude();
}

Vector3D getUnitVector3D(Vector3D u){
    return u / u.getMagnitude();
}

// gets a point in the direction of a vector with a specified magnitude and initial point
sc2::Point2D getPoint2D(sc2::Point2D initial, Vector2D direction, float magnitude){
    Vector2D unit = getUnitVector2D(direction);
    sc2::Point2D output;
    output.x = initial.x + unit.dx * magnitude;
    output.y = initial.y + unit.dy * magnitude;
    return output;
}

sc2::Point3D getPoint3D(sc2::Point3D initial, Vector3D direction, float magnitude){
    Vector3D unit = getUnitVector3D(direction);
    sc2::Point3D output;
    output.x = initial.x + unit.dx * magnitude;
    output.y = initial.y + unit.dy * magnitude;
    output.z = initial.y + unit.dz * magnitude;
    return output;
}

} // end namespace Monte