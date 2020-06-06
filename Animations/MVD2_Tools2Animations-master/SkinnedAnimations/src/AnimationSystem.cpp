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

void AnimationSystem::incrementJointFrame_(Joint* joint) {
    
    //avoid updating joints that don't have any keyframes
    if (joint->num_keyframes > 0) {
        
        joint->current_keyframe++;
        if (joint->current_keyframe == joint->num_keyframes)
            joint->current_keyframe = 0;
        
        joint->matrix = joint->keyframes[joint->current_keyframe] ;
    }
    for (auto& c : joint->children) {
        incrementJointFrame_(c);
    }
}

void AnimationSystem::update(float dt) {
    //frame counter
    
    bool trigger_frame = false;
    //increment millisecond counter
    ms_counter_ += dt * 1000;
    //if counter above threshold
    if (ms_counter_ >= ms_per_frame_) {
        trigger_frame = true;
        ms_counter_ = 0;
    }
    
    //animation component
    auto& anims = ECS.getAllComponents<Animation>();
    for (auto& anim : anims) {
        //get transform
        Transform& transform = ECS.getComponentFromEntity<Transform>(anim.owner);
        
        //if new frame
        if (trigger_frame) {
            //set positions to current frame
            transform.set(anim.keyframes[anim.curr_frame]);
            //advance frame
            anim.curr_frame++;
            //loop if required
            if (anim.curr_frame == anim.num_frames)
                anim.curr_frame = 0;
        }
    }

    //skinned mesh joints
    auto& bonetrackers = ECS.getAllComponents<BoneTracker>();
    for (auto& track : bonetrackers) {
        track.update(dt);
    }
    
    //skinned mesh joints
    auto& skinnedmeshes = ECS.getAllComponents<SkinnedMesh>();
    for (auto& sm : skinnedmeshes) {
        if (!sm.root) continue; //only if mesh has a joint chain!
        if (trigger_frame) {
            incrementJointFrame_(sm.root);
        }
    }
}
