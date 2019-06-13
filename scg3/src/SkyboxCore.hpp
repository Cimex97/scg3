//
//  SkyboxCore.hpp
//  uebung4
//  Erweiterte CubeMapCore Klasse
//  Skybox benötigt zwei CubeMaps für Tag / Nacht
//
//  Created by Sebastian Meyer on 23.05.19.
//  Copyright © 2019 Sebastian Meyer. All rights reserved.
//

#ifndef SkyboxCore_hpp
#define SkyboxCore_hpp

#include <vector>
#include "scg_internals.h"
#include "Sunlight.h"
#include "TextureCore.h"
SCG_DECLARE_CLASS(SkyboxCore);

namespace scg {
    /**
     * \brief Advanced Cube maps textues core.
     *
     * To be used in conjunction with cube map or skybox shader.
     */
    class SkyboxCore: public TextureCore {
        
    public:
        
        /**
         * Constructor.
         */
        SkyboxCore();
        
        /**
         * Destructor.
         */
        virtual ~SkyboxCore();
        
        /**
         * Create shared pointer.
         */
        static SkyboxCoreSP create();
        
        /**
         * Create 2 cube map textures from RGBA images with given parameters.
         *
         * \param width texture width
         * \param height texture height
         * \param rgbaData vector of 6 arrays of RGBA values for textures in directions
         *     +x, -x, +y, -y, +z, -z
         * \param width2 texture2 width
         * \param height2 texture2 height
         * \param rgbaData2 vector of 6 arrays of RGBA values for textures in directions
         *     +x, -x, +y, -y, +z, -z
         */
        void setCubeMap(GLsizei width, GLsizei height, const std::vector<unsigned char*>& rgbaData, GLsizei width2, GLsizei height2, const std::vector<unsigned char*>& rgbaData2);
        
        // Hilfsklasse zum laden der Texturen
        void loadTexture(GLuint id, GLsizei width, GLsizei height, const std::vector<unsigned char*>& rgbaData);
        
        /**
         * Render core, i.e., bind texture and post-multiply current texture matrix
         * by local texture matrix.
         * Pass inverse view matrix and skybox matrix (i.e., model-view-projection matrix
         * without camera translation) to shader program.
         */
        virtual void render(RenderState* renderState);
        
        /**
         * Render core after traversing sub-tree, i.e., restore previous texture matrix
         * and bind previous texture.
         */
        virtual void renderPost(RenderState* renderState);
        protected:
        
        float blendFactor;
        bool day;

    };
    
    
} /* namespace scg */

#endif /* SkyboxCore_hpp */
