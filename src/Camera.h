#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Bezier.h"

#define ZOOM_SENSITIVITY 3
#define YAW_ANGLE_SENSITIVITY 0.008
#define PITCH_ANGLE_SENSITIVITY 0.016
#define CAMERA_SPRING 3
#define MAX_DISTANCE 12
#define MIN_DISTANCE 2.2
#define MAX_PITCH -100
#define MIN_PITCH 100

class Camera
{
public:  
    bool zoomIn;
    bool zoomOut;
    bool inAnimation;
    bool w,a,s,d;
    float t;
    float targetYaw; 
    float yawAngle;
    float targetDistance;
    float distance;
    float pitchOffSet;
    float targetPitchOffset;
    int controlDir;
    glm::vec3 position;

    Camera()
    {
        w = a = s = d = false;
        controlDir = -1;
        targetYaw = yawAngle = 0.0;
        targetDistance = distance = 5.0;
        inAnimation = false;
        t = 0.0f;
    }

    void startPullbackAnimation();

    glm::mat4 update
    (     
        glm::vec3 lookat,
        double frametime, 
        int mousex, 
        int mousey,
        int width, 
        int height
    );

    glm::vec3 getPosition();
};

#endif