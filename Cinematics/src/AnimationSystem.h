#pragma once
#include "includes.h"
#include "Shader.h"
#include "Components.h"

class AnimationSystem {
public:
    ~AnimationSystem();
    void init();
    void lateInit();
    void update(float dt);
    
    
};
