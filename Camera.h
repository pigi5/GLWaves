/****************************************************************************

Name: Ford Hash
CSI 4341: Assignment 2: Camera

****************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include "Algebra.h"

class Camera {
private:
    Point eye;
    Vector look;
    Vector up;
    double viewAngle;
    double nearPlane;
    double farPlane;
    double screenWidth;
    double screenHeight;
    Matrix projectionMatrix;
    Matrix parallelProjectionMatrix;
    Matrix modelViewMatrix;
    Matrix inverseNormalizationMatrix;
    Vector wVector, uVector, vVector;
    double wRot, uRot, vRot;
public:
    Camera();
    ~Camera();
    void Orient(Point& eye, Point& focus, Vector& up);
    void Orient(Point& eye, Vector& look, Vector& up);
    void SetViewAngle (double viewAngle);
    void SetNearPlane (double nearPlane);
    void SetFarPlane (double farPlane);
    void SetScreenSize (int screenWidth, int screenHeight);

    Matrix GetProjectionMatrix();
    Matrix GetModelViewMatrix();
    Matrix getInverseNormalizationMatrix();
    void computeInverseNormalizationMatrix();
    
    void CreateProjectionMatrix();
    void CreateModelViewMatrix();
    
    void RotateV(double angle);
    void RotateU(double angle);
    void RotateW(double angle);
    void Rotate(Point p, Vector axis, double degree);
    void Translate(const Vector &v);

    void Reset();

    Point GetEyePoint();
    Vector GetLookVector();
    Vector GetUpVector();
    double GetViewAngle();
    double GetNearPlane();
    double GetFarPlane();
    int GetScreenWidth();
    int GetScreenHeight();

    double GetFilmPlanDepth();
    double GetScreenWidthRatio();
};
#endif

