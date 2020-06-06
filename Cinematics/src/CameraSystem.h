#pragma once
#include "includes.h"
#include "Components.h"
#include "interpolators.h"

// Camera module, used to handle cameras and its actions
class CameraSystem {

public:

    // Cameras have priorities, so a camera with higher priority must always be displayed in top of another.
    enum EPriority { DEFAULT = 0, GAMEPLAY, TEMPORARY, DEBUG, NUM_PRIORITIES };

    CameraSystem();

    bool init();
    bool lateInit();
    bool stop();
    void update(float dt);
    void updateTest(float dt);
    void render();

    void checkDeprecated(); // remove temporal cameras that are finished

    // Getters and setters for default and output cameras.
    void SetDefaultCamera(Entity & camera);
    void SetOutputCamera(Entity & camera);

    int  GetOutputCamera();
    int  GetDefaultCamera();
    int  GetOutputCameraEntity();
    int  GetDefaultCameraEntity();

    // Blending functions, in and out.
    void blendCameras(const Camera* camera1, const Camera* camera2, float ratio, Camera* output) const;
    void blendInCamera(Entity camera, float blendTime = 0.f, EPriority priority = EPriority::DEFAULT, Interpolator::IInterpolator* interpolator = nullptr);
    void blendOutCamera(Entity camera, float blendTime = 0.f);

    void renderInMenu();

private:

    Entity _defaultCamera;
    Entity _outputCamera;

    // Camera type for transitions, temporal dummy camera
    struct CameraMixed
    {
        enum EState { ST_BLENDING_IN, ST_IDLE, ST_BLENDING_OUT };

        Entity camera;
        EState state = EState::ST_IDLE;
        EPriority type = EPriority::DEFAULT;

        float blendInTime = 0.f; // gets to full ratio (1.f) in n seconds
        float blendOutTime = 0.f; // gets to full ratio (1.f) in n seconds
        float weight = 0.f;  // blend weight ratio
        float time = 0.f; // current blending time

        // Type of interpolation for the camera
        Interpolator::IInterpolator* interpolator = nullptr;

        void blendIn(float duration);
        void blendOut(float duration);
    };

    std::vector<CameraMixed> _mixedCameras;

    CameraMixed* getMixedCamera(Entity camera);
};

