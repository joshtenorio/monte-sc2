/**
 * @file VectorMath.h
 * @author Joshua Tenorio
 * 
 * Helper functions and implementations for vector math
 */
#pragma once

#include <sc2api/sc2_common.h>

namespace Monte {

typedef struct Vector2D_s_t {
    Vector2D_s_t() {};
    Vector2D_s_t(float dx_, float dy_) { dx = dx_; dy = dy_; };
    float dx, dy;

    virtual float getMagnitude();
    virtual float getMagnitudeSquared();

    // scalar multiplication
    Vector2D_s_t operator * (const float magnitude){
        Vector2D_s_t v;
        v.dx = this->dx * magnitude;
        v.dy = this->dy * magnitude;
        return v;
    }
    Vector2D_s_t operator * (const int magnitude){
        Vector2D_s_t v;
        v.dx = this->dx * magnitude;
        v.dy = this->dy * magnitude;
        return v;
    }
    Vector2D_s_t operator / (const float magnitude){
        Vector2D_s_t v;
        v.dx = this->dx / magnitude;
        v.dy = this->dy / magnitude;
        return v;
    }
    Vector2D_s_t operator / (const int magnitude){
        Vector2D_s_t v;
        v.dx = this->dx / magnitude;
        v.dy = this->dy / magnitude;
        return v;
    }

    // vector addition
    Vector2D_s_t operator + (const Vector2D_s_t& u){
        Vector2D_s_t v;
        v.dx = this->dx + u.dx;
        v.dy = this->dy + u.dy;
        return v;
    }

    // TODO: vector subtraction

    Vector2D_s_t& operator = (const Vector2D_s_t& u){
        dx = u.dx;
        dy = u.dy;
        return *this;
    }

    bool operator == (const Vector2D_s_t& u) const {
        if(
            dx == u.dx &&
            dy == u.dy
        ) return true;
        else return false;
    }
} Vector2D;

typedef struct Vector3D_s_t : public Vector2D_s_t {
    float dz;

    virtual float getMagnitude();
    virtual float getMagnitudeSquared();

    Vector3D_s_t operator * (const float magnitude){
        Vector3D_s_t v;
        v.dx = this->dx * magnitude;
        v.dy = this->dy * magnitude;
        v.dz = this->dz * magnitude;
        return v;
    }

    Vector3D_s_t operator * (const int magnitude){
        Vector3D_s_t v;
        v.dx = this->dx * magnitude;
        v.dy = this->dy * magnitude;
        v.dz = this->dz * magnitude;
        return v;
    }

    Vector3D_s_t operator / (const float magnitude){
        Vector3D_s_t v;
        v.dx = this->dx / magnitude;
        v.dy = this->dy / magnitude;
        v.dz = this->dz / magnitude;
        return v;
    }

    Vector3D_s_t operator / (const int magnitude){
        Vector3D_s_t v;
        v.dx = this->dx / magnitude;
        v.dy = this->dy / magnitude;
        v.dz = this->dz / magnitude;
        return v;
    }

    Vector3D_s_t& operator + (const Vector3D_s_t& u){
        dx = u.dx;
        dy = u.dy;
        return *this;
    }

    Vector3D_s_t& operator = (const Vector3D_s_t& u){
        Vector2D_s_t::operator=(u);
        dz = u.dz;
        return *this;
    }

    bool operator == (const Vector3D_s_t& u) const {
        if(
            Vector2D_s_t::operator==(u) &&
            dz == u.dz
        ) return true;
        else return false;
    }
} Vector3D;

// TODO: should this be in the vector struct ? if so it shouldnt change the vector struct
// returns the unit vector of a given vector u
Vector2D getUnitVector2D(Vector2D u);
Vector3D getUnitVector3D(Vector3D u);

// gets a point in the direction of a vector with a specified magnitude and initial point
sc2::Point2D getPoint2D(sc2::Point2D initial, Vector2D direction, float magnitude);
sc2::Point3D getPoint3D(sc2::Point3D initial, Vector3D direction, float magnitude);

// TODO: add support for dot and cross product



} // end namespace Monte