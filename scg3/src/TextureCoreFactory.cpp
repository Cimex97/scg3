/**
 * \file TextureCoreFactory.cpp
 *
 * \author Volker Ahlers\n
 *         volker.ahlers@hs-hannover.de
 */

/*
 * Copyright 2014 Volker Ahlers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cassert>
#include <stdexcept>
#include "BumpMapCore.h"
#include "CubeMapCore.h"
#include "scg_stb_image.h"
#include "scg_utilities.h"
#include "Texture2DCore.h"
#include "TextureCoreFactory.h"
#include "SkyboxCore.hpp"
#include "Core.h"

namespace scg {


TextureCoreFactory::TextureCoreFactory() {
}


TextureCoreFactory::TextureCoreFactory(const std::string& filePath) {
  addFilePath(filePath);
}


TextureCoreFactory::~TextureCoreFactory() {
}


void TextureCoreFactory::addFilePath(const std::string& filePath) {
  assert(!filePath.empty());
  splitFilePath(filePath, filePaths_);
}


Texture2DCoreSP TextureCoreFactory::create2DTextureFromFile(const std::string& fileName,
    GLenum wrapModeS, GLenum wrapModeT, GLenum minFilter, GLenum magFilter) {

  // try to find file
  std::string fullFileName = getFullFileName(filePaths_, fileName);
  if (fullFileName.empty()) {
    throw std::runtime_error("Cannot open file" + fileName
        + " [TextureCoreFactory::create2DTextureFromFile()]");
  }

  // load image and create array with 4 components (RGBA)
  int width, height, dummy;
  unsigned char* rgbaData = stbi_load(fullFileName.c_str(), &width, &height, &dummy, 4);
  if (!rgbaData) {
    throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason())
        + " [TextureCoreFactory::create2DTextureFromFile()]");
  }

  // create texture core
  auto core = Texture2DCore::create();
  core->setTexture(width, height, rgbaData, wrapModeS, wrapModeT, minFilter, magFilter);

  // free image memory and return texture core
  stbi_image_free(rgbaData);
  return core;
}


BumpMapCoreSP TextureCoreFactory::createBumpMapFromFiles(const std::string& texFileName,
    const std::string& normalFileName, GLenum wrapModeS, GLenum wrapModeT,
    GLenum minFilter, GLenum magFilter) {

  // create bump map core
  auto core = BumpMapCore::create();

  if (!texFileName.empty()) {
    // try to find texture file
    std::string fullFileName = getFullFileName(filePaths_, texFileName);
    if (fullFileName.empty()) {
      throw std::runtime_error("Cannot open file" + texFileName
          + " [TextureCoreFactory::createBumpMapFromFiles()]");
    }

    // load texture image, create array with 4 components (RGBA), and set texture
    int width, height, dummy;
    unsigned char* rgbaData = stbi_load(fullFileName.c_str(), &width, &height, &dummy, 4);
    if (!rgbaData) {
      throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason())
          + " [TextureCoreFactory::createBumpMapFromFiles()]");
    }
    core->setTexture(width, height, rgbaData, wrapModeS, wrapModeT, minFilter, magFilter);

    // free image memory
    stbi_image_free(rgbaData);
  }

  // try to find normal map file
  std::string fullFileName = getFullFileName(filePaths_, normalFileName);
  if (fullFileName.empty()) {
    throw std::runtime_error("Cannot open file" + normalFileName
        + " [TextureCoreFactory::createBumpMapFromFiles()]");
  }

  // load normal map image, create array with 4 components (RGBA), and set normal map
  int width, height, dummy;
  unsigned char* rgbaData = stbi_load(fullFileName.c_str(), &width, &height, &dummy, 4);
  if (!rgbaData) {
    throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason())
        + " [TextureCoreFactory::createBumpMapFromFiles()]");
  }
  core->setNormalMap(width, height, rgbaData, wrapModeS, wrapModeT, minFilter, magFilter);

  // free image memory and return bump map core
  stbi_image_free(rgbaData);
  return core;
}


CubeMapCoreSP TextureCoreFactory::createCubeMapFromFiles(const std::vector<std::string>& fileNames) {

  assert(fileNames.size() == 6);

  int width, height, dummy;
  std::vector<unsigned char*> rgbaData;
  for (int i = 0; i < 6; ++i) {
    // try to find file
    std::string fullFileName = getFullFileName(filePaths_, fileNames[i]);
    if (fullFileName.empty()) {
      throw std::runtime_error("Cannot open file" + fileNames[i]
          + " [TextureCoreFactory::createCubeMapFromFile()]");
    }

    // load image and create array with 4 components (RGBA)
    rgbaData.push_back(stbi_load(fullFileName.c_str(), &width, &height, &dummy, 4));
    if (!rgbaData.back()) {
      throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason())
          + " [TextureCoreFactory::createCubeMapFromFile()]");
    }
  }

  // create texture core
  auto core = CubeMapCore::create();
  core->setCubeMap(width, height, rgbaData);

  // free image memory and return texture core
  for (int i = 0; i < 6; ++i) {
    stbi_image_free(rgbaData[i]);
  }
  return core;
}

SkyboxCoreSP TextureCoreFactory::createSkyboxFromFiles(const std::vector<std::string>& fileNames1, const std::vector<std::string>& fileNames2){
        
        assert(fileNames1.size() == 6);
        assert(fileNames2.size() == 6);
    
    // Fucking wichtig eigene für zweite Texture, selben verwenden führt zu Darstellungsfehlern
        int width, width2, height, height2, dummy, dummy2;
    //load RGBA Data from filenames1
        std::vector<unsigned char*> rgbaData1;
        std::vector<unsigned char*> rgbaData2;
    
        for (int i = 0; i < 6; ++i) {
            
            // try to find file
            std::string fullFileName = getFullFileName(filePaths_, fileNames1[i]);
            if (fullFileName.empty()) {
                throw std::runtime_error("Cannot open file" + fileNames1[i]
                                         + " [TextureCoreFactory::createSkyboxFromFiles()]");
            }
            
            // load image and create array with 4 components (RGBA)
            rgbaData1.push_back(stbi_load(fullFileName.c_str(), &width, &height, &dummy, 4));
            if (!rgbaData1.back()) {
                throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason())
                                         + " [TextureCoreFactory::createSkyboxFromFiles()]");
            }
            
            // try to find file2
            std::string fullFileName2 = getFullFileName(filePaths_, fileNames2[i]);
            if (fullFileName2.empty()) {
                throw std::runtime_error("Cannot open file" + fileNames2[i]
                                         + " [TextureCoreFactory::createSkyboxFromFiles()]");
            }
            
            // load image2 and create array with 4 components (RGBA)
            rgbaData2.push_back(stbi_load(fullFileName2.c_str(), &width2, &height2, &dummy2, 4));
            if (!rgbaData2.back()) {
                throw std::runtime_error("stb_image error: " + std::string(stbi_failure_reason())
                                         + " [TextureCoreFactory::createSkyboxFromFiles()]");
            }
           
        }
    
    // create texture core
        auto core = SkyboxCore::create();
        core->setCubeMap(width, height, rgbaData1, width2, height2, rgbaData2);
        
        // free image memory and return texture core
        for (int i = 0; i < 6; ++i) {
            stbi_image_free(rgbaData1[i]);
            stbi_image_free(rgbaData2[i]);
        }
        return core;
    }

CubeMapCoreSP TextureCoreFactory::createCubeMapFromFiles(std::vector<std::string>&& fileNames) {
  std::vector<std::string> fileNamesVec = std::move(fileNames);
  return createCubeMapFromFiles(fileNamesVec);
}


} /* namespace scg */
