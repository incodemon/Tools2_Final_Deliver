#include "Components.h"
#include "extern.h"

// Set the bone and entity data into the component
void BoneTracker::SetBone(const std::string & entity_name, const std::string & bone_name)
{
    target_entity = ECS.getEntity(entity_name);
    SkinnedMesh & sk_mesh = ECS.getComponentFromEntity<SkinnedMesh>(target_entity);
    bone = sk_mesh.GetBoneByName(bone_name);
}

// Update the tracker position each frame
void BoneTracker::update(float dt)
{
    if (bone)
    {
        // Current tracker entity (knife).      
        Entity & my_entity = ECS.entities[owner];
        Transform & my_transform = ECS.getComponentFromEntity<Transform>(owner);

        // Apply the bone transform to the target entity
        lm::mat4 tmp_matrix = getGlobalMatrix(bone);
        my_transform.set(tmp_matrix);
    }
}

// Get the global matrix of the current bone.
lm::mat4 BoneTracker::getGlobalMatrix(Joint * bone)
{
    if (bone->parent != NULL) {
        return getGlobalMatrix(bone->parent) * bone->matrix;
    }

    return bone->matrix;
}
