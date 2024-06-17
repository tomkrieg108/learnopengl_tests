#pragma once

//based on
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h


class Shader;
static constexpr uint32_t MAX_BONE_INFLUENCE = 4;

namespace lgl
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        //bone indexes which will influence this vertex
        int32_t m_BoneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE];
    };

    struct TextureStruct 
    {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class Mesh 
    {
    public:
        // mesh Data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;
        std::vector<TextureStruct>      textures;
        unsigned int VAO;

        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureStruct> textures);

        // render the mesh
        void Draw(Shader& shader);

    private:
        // render data 
        unsigned int VBO, EBO;

        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}

