//
//  main.cpp
//
//  Copyright © 2018 Alun Evans. All rights reserved.
//
#include "components.h"
#include "extern.h"
#include "Game.h"
#include "Curve.h"

ViewTrack::ViewTrack()
{
    ratio = 0;
}

// Generate a mesh made of lines
void ViewTrack::GenerateMesh()
{
    // Get our viewtracks and set a VBO 
    std::vector<float> track_vertices;
    std::vector<GLuint> track_indices;

    int index = 0;
    auto& view_tracks = ECS.getAllComponents<ViewTrack>();

    for (auto& cc : view_tracks) {

        // We have a path track
        if (cc.curve._knots.size() > 1)
        {
            for (unsigned int i = 0; i < cc.curve._knots.size(); i++)
            {
                track_vertices.push_back(cc.curve._knots[i].x);
                track_vertices.push_back(cc.curve._knots[i].y);
                track_vertices.push_back(cc.curve._knots[i].z);

                //if (i < cc.knots.size() - 1)
                {
                    track_indices.push_back(index + 3 * i);
                    track_indices.push_back(index + 3 * i + 1);
                    track_indices.push_back(index + 3 * i + 1);
                    track_indices.push_back(index + 3 * i + 2);
                    track_indices.push_back(index + 3 * i + 2);
                    track_indices.push_back(index + 3 * i + 3);
                }
            }
        }

        index += cc.curve._knots.size() - 1;

        GLuint* track_line_indices = &track_indices[0]; // Hardcoded size, we might have some limitations here...

        //gl buffers
        glGenVertexArrays(1, &curve_vao_);
        glBindVertexArray(curve_vao_);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (track_indices.size()) * sizeof(float), &(track_vertices[0]), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        //indices
        GLuint ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, track_indices.size() * sizeof(GLuint), &(track_line_indices[0]), GL_STATIC_DRAW);

        //unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void ViewTrack::update(float dt)
{
    if (active)
    {
        // Incrementing the value (lerping)
        ratio += dt * speed;
        lm::vec3 new_pos = curve.evaluateAsCatmull(ratio);

        // Update the camera position
        Camera & cam = ECS.getComponentFromEntity<Camera>(owner);
        Transform & trans = ECS.getComponentFromEntity<Transform>(owner);

        {
            trans.position(new_pos);
            cam.position = new_pos;
            cam.lookAt(new_pos, cam.target);
            ratio = ratio >= 1 ? 0 : ratio;
        }
    }
}

void ViewTrack::render(GLuint prog)
{
    lm::mat4 vp = ECS.getComponentInArray<Camera>(Game::instance->camera_system_.GetOutputCamera()).view_projection;

    glUseProgram(prog);
    GLint u_mvp = glGetUniformLocation(prog, "u_mvp");
    GLint u_color = glGetUniformLocation(prog, "u_color");
    GLint u_color_mod = glGetUniformLocation(prog, "u_color_mod");
    GLint u_size_scale = glGetUniformLocation(prog, "u_size_scale");
    GLint u_center_mod = glGetUniformLocation(prog, "u_center_mod");

    float grid_colors[12] = {
    0.7f, 0.7f, 0.7f, //grey
    1.0f, 0.5f, 0.5f, //red
    0.5f, 1.0f, 0.5f, //green
    0.5f, 0.5f, 1.0f }; //blue

    {
        //set uniforms and draw grid
        glUniformMatrix4fv(u_mvp, 1, GL_FALSE, vp.m);
        glUniform3fv(u_color, 4, grid_colors);
        glUniform3f(u_size_scale, 1.0, 1.0, 1.0);
        glUniform3f(u_center_mod, 0.0, 0.0, 0.0);
        glUniform1i(u_color_mod, 3);
        glBindVertexArray(curve_vao_); //TRACKS
        glDrawElements(GL_LINES, 33, GL_UNSIGNED_INT, 0);
    }
}