#pragma once

///https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "lgl_mesh.h"

class Shader;

namespace lgl
{
    uint32_t TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

    class Model
    {
    public:
        // model data 
        std::vector<lgl::TextureStruct> m_textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<lgl::Mesh>    m_meshes;
        std::string m_directory{ "" };
        
        bool m_gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false);

        // draws the model, and thus all its meshes
        void Draw(Shader& shader);

    private:
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(std::string const& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene);

        lgl::Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<lgl::TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}


