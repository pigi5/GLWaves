/**
 * Author: Ford Hash
 * Last Modified: 12/7/2017
 */

#include "Camera.h"
#include <iostream>

Camera::Camera() {
    Reset();
}

Camera::~Camera() {
}

void Camera::Reset() {
    eye = Point(0,0,2);
    look = Vector(0,0,-1.0);
    up = Vector(0,1.0,0);
    nearPlane = 0.001;
    farPlane = 30;
    viewAngle = 0;
    SetViewAngle(45);
    vRot = 0;
    uRot = 0;
    wRot = 0;
    CreateProjectionMatrix();
    CreateModelViewMatrix();
}

void Camera::Orient(Point& eye, Point& focus, Vector& up) {
    if (this->eye != eye || this->look != focus - eye || this->up != up) {
        this->eye = eye;
        this->look = focus - eye;
        this->up = up;
        CreateModelViewMatrix();
    }
}


void Camera::Orient(Point& eye, Vector& look, Vector& up) {
    if (this->eye != eye || this->look != look || this->up != up) {
        this->eye = eye;
        this->look = look;
        this->up = up;
        CreateModelViewMatrix();
    }
}

Matrix Camera::GetProjectionMatrix() {
	return projectionMatrix;
}

void Camera::CreateProjectionMatrix() {
    double c = -nearPlane/farPlane;
    Matrix unhinge(1, 0, 0, 0,
                   0, 1, 0, 0,
                   0, 0, -1/(c+1), c/(c+1),
                   0, 0, -1, 0);
    double widthAngle = atan((screenWidth/2.0)/nearPlane)*2.0;
    Matrix scale(1/(tan(widthAngle/2.0)*farPlane), 0, 0, 0,
                 0, 1/(tan(viewAngle/2.0)*farPlane), 0, 0,
                 0, 0, 1/farPlane, 0,
                 0, 0, 0, 1);
    projectionMatrix = unhinge * scale;

    parallelProjectionMatrix = Matrix(2/screenWidth, 0, 0, 0,
                              0, 2/screenHeight, 0, 0,
                              0, 0, 1/farPlane, 0,
                              0, 0, 0, 1);
}

void Camera::computeInverseNormalizationMatrix() {
    double widthAngle = atan((screenWidth/2.0)/nearPlane)*2.0;
    Matrix scale(1/(tan(widthAngle/2.0)*farPlane), 0, 0, 0,
                 0, 1/(tan(viewAngle/2.0)*farPlane), 0, 0,
                 0, 0, 1/farPlane, 0,
                 0, 0, 0, 1);
    inverseNormalizationMatrix = invert(scale * GetModelViewMatrix());
}

Matrix Camera::getInverseNormalizationMatrix() {
    return inverseNormalizationMatrix;
}

void Camera::SetViewAngle (double viewAngle) {
    if (this->viewAngle != DEG_TO_RAD(viewAngle)) {
        double aspectRatio = GetScreenWidthRatio();
        this->viewAngle = DEG_TO_RAD(viewAngle);
        screenHeight = tan(this->viewAngle / 2.0) * nearPlane * 2.0;
        screenWidth = aspectRatio * screenHeight;
        CreateProjectionMatrix();
    }
}

void Camera::SetNearPlane (double nearPlane) {
    if (this->nearPlane != nearPlane) {
        this->nearPlane = nearPlane;
        double aspectRatio = GetScreenWidthRatio();
        screenHeight = tan(this->viewAngle / 2.0) * nearPlane * 2.0;
        screenWidth = aspectRatio * screenHeight;
        CreateProjectionMatrix();
    }
}

void Camera::SetFarPlane (double farPlane) {
    if (this->farPlane != farPlane) {
        this->farPlane = farPlane;
        CreateProjectionMatrix();
    }
}

void Camera::SetScreenSize (int screenWidth, int screenHeight) {
    if (this->screenWidth != screenWidth || this->screenHeight != screenHeight) {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        nearPlane = (screenHeight / 2.0) / tan(viewAngle / 2.0);
        CreateProjectionMatrix();
    }
}

Matrix Camera::GetModelViewMatrix() {
	return modelViewMatrix;
}

void Camera::CreateModelViewMatrix() {
    wVector = normalize(-look);
    uVector = normalize(cross(up, wVector));
    vVector = cross(wVector, uVector);

    Matrix rot = rot_mat(vVector, vRot) * rot_mat(uVector, uRot) * rot_mat(wVector, wRot);

    wVector = rot * wVector;
    uVector = rot * uVector;
    vVector = rot * vVector;

    Matrix rotate(uVector[0], uVector[1], uVector[2], 0,
                  vVector[0], vVector[1], vVector[2], 0,
                  wVector[0], wVector[1], wVector[2], 0,
                  0, 0, 0, 1);
    modelViewMatrix = rotate * trans_mat(Point(0,0,0) - eye);
}

void Camera::RotateV(double angle) {
    if (vRot != DEG_TO_RAD(angle)) {
        vRot = DEG_TO_RAD(angle);
        CreateModelViewMatrix();
    }
}

void Camera::RotateU(double angle) {
    if (uRot != DEG_TO_RAD(angle)) {
        uRot = DEG_TO_RAD(angle);
        CreateModelViewMatrix();
    }
}

void Camera::RotateW(double angle) {
    if (wRot != DEG_TO_RAD(angle)) {
        wRot = DEG_TO_RAD(angle);
        CreateModelViewMatrix();
    }
}

void Camera::Translate(const Vector &v) {
    if (eye != Point(0, 0, 0) + v) {
        eye = Point(0, 0, 0) + v;
        CreateModelViewMatrix();
    }
}


void Camera::Rotate(Point p, Vector axis, double degrees) {
    Matrix rot = rot_mat(p, axis, DEG_TO_RAD(degrees));

    eye = rot * eye;
    look = rot * look;
    up = rot * up;
    CreateProjectionMatrix();
    CreateModelViewMatrix();
}


Point Camera::GetEyePoint() {
	return eye;
}

Vector Camera::GetLookVector() {
	return look;
}

Vector Camera::GetUpVector() {
	return up;
}

double Camera::GetViewAngle() {
	return RAD_TO_DEG(viewAngle);
}

double Camera::GetNearPlane() {
	return nearPlane;
}

double Camera::GetFarPlane() {
	return farPlane;
}

int Camera::GetScreenWidth() {
	return screenWidth;
}

int Camera::GetScreenHeight() {
	return screenHeight;
}

double Camera::GetFilmPlanDepth() {
	return farPlane - nearPlane;
}

double Camera::GetScreenWidthRatio() {
	return screenWidth / screenHeight;
}
