//
//  SkyboxCore.cpp
//  uebung4
//  Erweiterte CubeMapCore Klasse
//  Skybox benötigt zwei CubeMaps für Tag / Nacht
//
//  Created by Sebastian Meyer on 23.05.19.
//  Copyright © 2019 Sebastian Meyer. All rights reserved.
//

#include <cassert>
#include "SkyboxCore.hpp"
#include "RenderState.h"
#include "scg_utilities.h"
#include "ShaderCore.h"
#include <iostream>

extern float gloabal_time_for_cg1_project_with_ahlers;
namespace scg {
    
    
    SkyboxCore::SkyboxCore()
    : TextureCore(), blendFactor(0.f) {
    }
    
    
    SkyboxCore::~SkyboxCore() {
        if (isGLContextActive()) {
            glActiveTexture(GL_TEXTURE0);
            glDeleteTextures(1, &tex_);
            glActiveTexture(GL_TEXTURE1);
            glDeleteTextures(1, &tex2_);
            glActiveTexture(GL_TEXTURE0);
        }
    }
    
    
    SkyboxCoreSP SkyboxCore::create() {
        return std::make_shared<SkyboxCore>();
    }

    void SkyboxCore::setCubeMap(GLsizei width, GLsizei height, const std::vector<unsigned char*>& rgbaData, GLsizei width2, GLsizei height2, const std::vector<unsigned char*>& rgbaData2) {
        assert(rgbaData.size() == 6);
        assert(rgbaData2.size() == 6);
        
    // Texture1 in openGL laden
        glActiveTexture(GL_TEXTURE0);
        glDeleteTextures(1, &tex_);
        glGenTextures(1, &tex_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_);
        assert(glIsTexture(tex_));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // use anisotropic filtering
        GLfloat maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        
        // transfer textures to GPU memory
        const GLenum cubeMapTexNames[] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        };
        for (int i = 0; i < 6; ++i) {
            assert(rgbaData[i]);
            glTexImage2D(cubeMapTexNames[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData[i]);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        
        assert(!checkGLError());

    //Texture2 in openGL laden
        glActiveTexture(GL_TEXTURE1);
        glDeleteTextures(1, &tex2_);
        glGenTextures(1, &tex2_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex2_);
        assert(glIsTexture(tex2_));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // transfer textures to GPU memory
        for (int i = 0; i < 6; ++i) {
            assert(rgbaData2[i]);
            glTexImage2D(cubeMapTexNames[i], 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData2[i]);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glActiveTexture(GL_TEXTURE0);
        assert(!checkGLError());
    }
    
    void SkyboxCore::render(RenderState* renderState) {
        // multiply current texture matrix by local texture matrix
        TextureCore::render(renderState);
    //Texture1 save texture binding
        glActiveTexture(GL_TEXTURE0);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &texOld_);
        // bind texture
        assert(glIsTexture(tex_));
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_);
        
    //Texture 1 save texture binding
        glActiveTexture(GL_TEXTURE1);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &texOld2_);
        // bind texture
        assert(glIsTexture(tex2_));
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex2_);
        
        // pass inverse view matrix and skybox matrix (i.e., model-view-projection matrix
        // without camera translation) to shader program
        glm::mat4 viewMatrix = renderState->getViewTransform();
        renderState->getShader()->setUniformMatrix4fv("invViewMatrix", 1, glm::value_ptr(glm::inverse(viewMatrix)));
        viewMatrix[3] = glm::vec4(0.f, 0.f, 0.f, 1.f);
        glm::mat4 skyboxMatrix = renderState->projectionStack.getMatrix() * viewMatrix * renderState->getModelMatrix();
        renderState->getShader()->setUniformMatrix4fv("skyboxMatrix", 1, glm::value_ptr(skyboxMatrix));
    
        //Day Night Time simple
        /*
        if(blendFactor >= 12){
            day = false;
        }else if(blendFactor <= 0){
            day = true;
        }
        
        if(day){
            blendFactor+=0.001f;
        }else{
            blendFactor-=0.001f;
        }
        */
        
        //Day Night Time 24h Sonnen Auf- Untergang
//        float time = fmodf(glfwGetTime(), 24);
        float time = gloabal_time_for_cg1_project_with_ahlers;
        //Aufgang 7 - 10
        //Untergang 20-23
        if(time > 6 && time < 10){
            if(blendFactor<1){
             blendFactor+=0.002f;
            }else{
                blendFactor=1;
            }
        }else if(time > 18 && time < 23){
            if(blendFactor>0){
                blendFactor-=0.002f;
            }else{
                blendFactor=0;
            }
            
        }
        
        std::cout << "Zeit aktuell: " << time << " BlendFactor: " << blendFactor << std::endl;
        
        renderState->getShader()->setUniform1f("blendFactor",  glm::float32(blendFactor));
        assert(!checkGLError());
        glActiveTexture(GL_TEXTURE0);
    }
    
    
    void SkyboxCore::renderPost(RenderState* renderState) {
        // restore texture binding
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texOld_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texOld2_);
        glActiveTexture(GL_TEXTURE0);
        // restore texture matrix
        TextureCore::renderPost(renderState);
        
        assert(!checkGLError());
    }
    
    
} /* namespace scg */
