//
//  AnimationSystem.cpp
//

#include "AnimationSystem.h"
#include "extern.h"

//destructor
AnimationSystem::~AnimationSystem() {

}

//set initial state 
void AnimationSystem::init() {

}

//called after loading everything
void AnimationSystem::lateInit() {

    
}

void AnimationSystem::update(float dt) {
    auto& anims = ECS.getAllComponents<Animation>();
    for (auto& anim : anims) {
        Transform& transform = ECS.getComponentFromEntity<Transform>(anim.owner);
        //increment counter (dt is in seconds)
        anim.ms_counter += dt *1000;
        //if counter above threshold
        if (anim.ms_counter >= anim.ms_frame) {
            //reset it - careful to overflow valley to avoid "cutting" time
            anim.ms_counter = anim.ms_counter - anim.ms_frame;
            //set positions
            transform.set(anim.keyframes[anim.curr_frame]);
            //advance frame
            anim.curr_frame++;
            //loop if required
            if (anim.curr_frame == anim.num_frames)
                anim.curr_frame = 0;
        }
        
        
    }
}
