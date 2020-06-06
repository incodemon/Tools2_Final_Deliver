#include "CameraSystem.h"
#include "Parsers.h"
#include "extern.h"
#include "linmath.h"
#include "Game.h"

CameraSystem::CameraSystem()
{}

bool CameraSystem::init()
{
    int camera_test_id = ECS.getEntity("camera_track3");
    return true;
}

// Retrieve default and output camera to the system
bool CameraSystem::lateInit()
{
    // Set default camera, the one to be used.
    int flyover_id = ECS.getEntity("camera_flyover");
    Entity& flyover = ECS.entities[flyover_id];
    SetDefaultCamera(flyover);

    // Output camera, where eerything is output too it.
    int main_camera_id = ECS.getEntity("camera_main");
    Entity& main_camera = ECS.entities[main_camera_id];
    SetOutputCamera(main_camera);

    ECS.main_camera = ECS.getComponentID<Camera>(flyover_id);

    return true;
}

bool CameraSystem::stop()
{
    return true;
}

// Update function, used to blend between gameplay cameras
void CameraSystem::update(float delta)
{
    updateTest(delta);

    Camera resultCamera;

    // Get default camera and set its transform
    if (_defaultCamera.isValid())
    {
        Entity e = _defaultCamera;
        Camera & c_camera = ECS.getComponentFromEntity<Camera>(e.name);
        blendCameras(&c_camera, &c_camera, 1.f, &resultCamera);

        Transform& c_trans = ECS.getComponentFromEntity<Transform>(e.name);
        c_trans.position(resultCamera.position);
    }

    // Get all the cameras in priority order, so gameplay is first.
    for (int i = 0; i < NUM_PRIORITIES; ++i)
    {
        for (auto& mc : _mixedCameras)
        {
            if (mc.type != i)
                continue;

            mc.time += delta;
            if (mc.state == CameraMixed::ST_BLENDING_IN)
            {
                mc.weight = lm::Utils::clamp(mc.time / mc.blendInTime, 0.f, 1.f);
                if (mc.weight >= 1.f)
                {
                    mc.state = CameraMixed::ST_IDLE;
                    mc.time = 0.f;
                }
            }
            else if (mc.state == CameraMixed::ST_BLENDING_OUT)
            {
                mc.weight = 1.f - lm::Utils::clamp(mc.time / mc.blendOutTime, 0.f, 1.f);
            }

            // Blend camera with the mixed camera in case it hasn't finished yet.
            if (mc.weight > 0.f)
            {
                float ratio = mc.weight;
                if (mc.interpolator)
                    ratio = mc.interpolator->blend(0.f, 1.f, ratio);

                // blend them
                Entity e = mc.camera;
                Camera& c_camera = ECS.getComponentFromEntity<Camera>(e.name);
                blendCameras(&resultCamera, &c_camera, ratio, &resultCamera);

                Transform& c_trans = ECS.getComponentFromEntity<Transform>(e.name);
                c_trans.position(resultCamera.position);
            }
        }
    }

    // Remove temporal mixed cameras
    checkDeprecated();

    // remove deprecated ones
    {
        std::vector<CameraMixed>::iterator it = _mixedCameras.begin();
        while (it != _mixedCameras.end())
        {
            if (it->weight <= 0.f)
                it = _mixedCameras.erase(it);
            else
                ++it;
        }
    }

    // save the result into the main camera (output camera)
    if (_outputCamera.isValid())
    {
        Entity e = _outputCamera;
        Camera& c_camera = ECS.getComponentFromEntity<Camera>(e.name);
        blendCameras(&resultCamera, &resultCamera, 1.f, &c_camera);

        Transform& c_trans = ECS.getComponentFromEntity<Transform>(e.name);
        c_trans.position(c_camera.position);
        
    }
}

// Check if a camera is already done.
void CameraSystem::checkDeprecated()
{
    // checks if there are cameras that should be removed
    for (int i = _mixedCameras.size() - 1; i >= 0; --i)
    {
        CameraMixed& mc = _mixedCameras[i];
        if (mc.type == GAMEPLAY && mc.weight >= 1.f)
        {
            if (mc.type == GAMEPLAY)
            {
                mc.weight = 0.f;

                Entity e1 = _outputCamera;
                Camera& c_camera1 = ECS.getComponentFromEntity<Camera>(e1.name);

                Entity e2 = _defaultCamera;
                Camera & c_camera2 = ECS.getComponentFromEntity<Camera>(e2.name);

                Transform& c_trans = ECS.getComponentFromEntity<Transform>(e2.name);
                c_trans.position(c_camera1.position);

                blendCameras(&c_camera1, &c_camera1, 1.f, &c_camera2);
            }
        }
    }
}

// Method for test purposes, make any necessary change here.
void CameraSystem::updateTest(float dt)
{
    int actual_camera_id;
    Camera resultCamera;
    string name = "camera_flyover";
    if (Game::instance->control_system_.input[GLFW_KEY_0]) {
        int camera_test_id = ECS.getEntity("camera_track3");
        Entity& main_camera = ECS.entities[camera_test_id];
        blendInCamera(main_camera, 2.f, CameraSystem::EPriority::GAMEPLAY);
        std::cout << "blending camera" << std::endl;
    }

    if (Game::instance->control_system_.input[GLFW_KEY_1])
    {
        int flyover_id = ECS.getEntity("camera_flyover");
        Entity& flyover = ECS.entities[flyover_id];
        SetDefaultCamera(flyover);
    }

    if (Game::instance->control_system_.input[GLFW_KEY_2]) 
    {
        //Start blending to the first camera, from the initial camera
       //Start transitioning on the first spline camera
       //Change the camera to the second spline and iterate through it
        actual_camera_id = GetDefaultCamera();
        Entity& actual_camera = ECS.entities[actual_camera_id];

        int first_camera_id = ECS.getEntity("camera_track2");
        Entity& first_camera = ECS.entities[first_camera_id];
        Camera& c_camera2 = ECS.getComponentFromEntity<Camera>(first_camera.name);
        blendInCamera(first_camera, 2.f, CameraSystem::EPriority::GAMEPLAY);
       
        blendOutCamera(actual_camera, 2.f);
        name = "camera_track2";


    }
    if (Game::instance->control_system_.input[GLFW_KEY_3])
    {
        //Start blending to the first camera, from the initial camera
       //Start transitioning on the first spline camera
       //Change the camera to the second spline and iterate through it
        name = "camera_track3";
        actual_camera_id = ECS.getEntity("camera_track3");
        Entity& first_camera = ECS.entities[actual_camera_id];
        blendInCamera(first_camera, 2.f, CameraSystem::EPriority::GAMEPLAY);
    }

    //SetDefaultCamera(ECS.entities[ECS.getEntity(name)]);

   
    // TO-DO
    // Add the blending camera to the spline
    

    // Lerp between both tracks.

    // Scripting LUA.
}

void CameraSystem::CameraMixed::blendIn(float duration)
{
    blendInTime = duration;
    state = blendInTime == 0.f ? ST_IDLE : ST_BLENDING_IN;
    time = 0.f;
}

void CameraSystem::CameraMixed::blendOut(float duration)
{
    blendOutTime = duration;
    state = ST_BLENDING_OUT;
    time = 0.f;
}

void CameraSystem::SetDefaultCamera(Entity & camera)
{
    _defaultCamera = camera;
}

void CameraSystem::SetOutputCamera(Entity & camera)
{
    _outputCamera = camera;
}

int CameraSystem::GetOutputCameraEntity()
{
    return ECS.getEntity(_outputCamera.name);
}

int CameraSystem::GetDefaultCameraEntity()
{
    return ECS.getEntity(_defaultCamera.name);
}

int CameraSystem::GetOutputCamera()
{
    int main_camera_id = ECS.getEntity(_outputCamera.name);
    return ECS.getComponentID<Camera>(main_camera_id);
}

int CameraSystem::GetDefaultCamera()
{
    int main_camera_id = ECS.getEntity(_defaultCamera.name);
    return ECS.getComponentID<Camera>(main_camera_id);
}

// Start blending camera
void CameraSystem::blendInCamera(Entity camera, float blendTime, EPriority priority, Interpolator::IInterpolator* interpolator)
{
    CameraMixed* mc = getMixedCamera(camera);

    if (!mc)
    {
        CameraMixed new_mc;
        new_mc.camera = camera;
        new_mc.type = priority;
        new_mc.interpolator = interpolator;
        new_mc.blendIn(blendTime);

        _mixedCameras.push_back(new_mc);
    }
    
}

// End blending camera
void CameraSystem::blendOutCamera(Entity camera, float blendTime)
{
    CameraMixed* mc = getMixedCamera(camera);
    if (mc)
    {
        mc->blendOut(blendTime);
    }
}

// Retrieve mixed camera
CameraSystem::CameraMixed* CameraSystem::getMixedCamera(Entity camera)
{
    for (auto& mc : _mixedCameras)
    {
        if (mc.camera.name == camera.name)
        {
            return &mc;
        }
    }

    return nullptr;
}

// Blend the two cameras and output the result in the third one.
void CameraSystem::blendCameras(const Camera* camera1, const Camera* camera2, float ratio, Camera* output) const
{
    assert(camera1 && camera2 && output);

    lm::vec3 newPos = lm::Utils::lerp(camera1->position, camera2->position, ratio);
    lm::vec3 newFront = lm::Utils::lerp(camera1->forward, camera2->forward, ratio);
    float newFov = lm::Utils::lerp(camera1->fov, camera2->fov, ratio);
    float newZnear = lm::Utils::lerp(camera1->near, camera2->near, ratio);
    float newZfar = lm::Utils::lerp(camera1->far, camera2->far, ratio);

    output->setPerspective(newFov, (float)Game::instance->window_width_ / (float)Game::instance->window_height_, newZnear, newZfar);
    output->lookAt(newPos, newPos + newFront);
}

void CameraSystem::render()
{
    // Render all dolly tracks.
    // Get all components of dolly track type
    // Render them.
}

// Used to render splines in imgui
void renderInterpolator(const char* name, Interpolator::IInterpolator& interpolator)
{
    const int nsamples = 50;
    float values[nsamples];

    for (int i = 0; i < nsamples; ++i)
        values[i] = interpolator.blend(0.f, 1.f, (float)i / (float)nsamples);

    ImGui::PlotLines(name, values, nsamples, 0, 0,
        std::numeric_limits<float>::min(), std::numeric_limits<float>::max(),
        ImVec2(150, 80));
}


// Display the different ways to interpolate camera transforms.
void CameraSystem::renderInMenu()
{

    if (ImGui::TreeNode("Interpolators"))
    {
        renderInterpolator("Linear", Interpolator::TLinearInterpolator());
        renderInterpolator("Quad in", Interpolator::TQuadInInterpolator());
        renderInterpolator("Quad out", Interpolator::TQuadOutInterpolator());
        renderInterpolator("Quad in out", Interpolator::TQuadInOutInterpolator());
        renderInterpolator("Cubic in", Interpolator::TCubicInInterpolator());
        renderInterpolator("Cubic out", Interpolator::TCubicOutInterpolator());
        renderInterpolator("Cubic in out", Interpolator::TCubicInOutInterpolator());
        renderInterpolator("Quart in", Interpolator::TQuartInInterpolator());
        renderInterpolator("Quart out", Interpolator::TQuartOutInterpolator());
        renderInterpolator("Quart in out", Interpolator::TQuartInOutInterpolator());
        renderInterpolator("Quint in", Interpolator::TQuintInInterpolator());
        renderInterpolator("Quint out", Interpolator::TQuintOutInterpolator());
        renderInterpolator("Quint in out", Interpolator::TQuintInOutInterpolator());
        renderInterpolator("Back in", Interpolator::TBackInInterpolator());
        renderInterpolator("Back out", Interpolator::TBackOutInterpolator());
        renderInterpolator("Back in out", Interpolator::TBackInOutInterpolator());
        renderInterpolator("Elastic in", Interpolator::TElasticInInterpolator());
        renderInterpolator("Elastic out", Interpolator::TElasticOutInterpolator());
        renderInterpolator("Elastic in out", Interpolator::TElasticInOutInterpolator());
        renderInterpolator("Bounce in", Interpolator::TBounceInInterpolator());
        renderInterpolator("Bounce out", Interpolator::TBounceOutInterpolator());
        renderInterpolator("Bounce in out", Interpolator::TBounceInOutInterpolator());
        renderInterpolator("Circular in", Interpolator::TCircularInInterpolator());
        renderInterpolator("Circular out", Interpolator::TCircularOutInterpolator());
        renderInterpolator("Circular in out", Interpolator::TCircularInOutInterpolator());
        renderInterpolator("Expo in", Interpolator::TExpoInInterpolator());
        renderInterpolator("Expo out", Interpolator::TExpoOutInterpolator());
        renderInterpolator("Expo in out", Interpolator::TExpoInOutInterpolator());
        renderInterpolator("Sine in", Interpolator::TSineInInterpolator());
        renderInterpolator("Sine out", Interpolator::TSineOutInterpolator());
        renderInterpolator("Sine in out", Interpolator::TSineInOutInterpolator());

        ImGui::TreePop();
    }
}
