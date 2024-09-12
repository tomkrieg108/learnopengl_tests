#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "layer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Shader;
//class Camera;
class Window;
//struct aiScene;
//struct aiMesh;
//struct aiMaterial;
//struct aiNode;

namespace me 
{
//--------------------------------------------------------------
	constexpr uint32_t MAX_BONE_INFLUENCE = 4;

	struct Vertex
	{
		glm::vec3 Position { 0.0f };
		glm::vec3 Normal{ 0.0f };
		glm::vec2 TexCoords{ 0.0f };
		glm::vec3 Tangent{ 0.0f };
		glm::vec3 Bitangent{ 0.0f };
		std::array<uint32_t, MAX_BONE_INFLUENCE> BoneId { 0 };
		std::array<float, MAX_BONE_INFLUENCE> BoneWeight { 0.0f };
	};

	struct TextureStruct
	{
		uint32_t id;
		std::string type;
		std::string path;
	};

	struct BoneTransformSOA
	{
		std::vector<std::string> bone_names;
		std::vector<uint32_t> bone_id;
		std::vector<glm::mat4> offset_transforms;
		std::vector<glm::mat4> final_transforms;
	};
	
	//Array or structures rather than struct or arrays
	class MeshAOS
	{
		friend class Model;

	public:
		MeshAOS() = default;

	private:
		void SetupMesh();
		void Draw(Shader& shader);

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<TextureStruct> m_textures;

		// render data 
		uint32_t VBO, EBO, VAO;
	};

	//--------------------------------------------------------------------------

	class Model
	{
	public:

		explicit Model(const std::string& file);
		~Model();

		bool Load(const std::string& file);
		void Draw(Shader& shader);
		void CalculateBoneTransforms(aiAnimation* ai_anim, aiNode* ai_node, const glm::mat4 parent_transform, float time_secs);
		const BoneTransformSOA& GetBoneTransforms() const { return m_bone_transforms_soa; }
		const aiScene* GetAIScene() const { return m_ai_scene; };

	private:
		
		void ParseMesh(const aiScene* ai_scene, uint32_t mesh_index);
		void ParseBones(const aiScene* ai_scene, uint32_t mesh_index);
		void loadMaterialTextures(uint32_t mesh_idx, aiMaterial* mat, aiTextureType type, std::string typeName);
		void CheckBones(const aiScene* ai_scene, uint32_t mesh_index);
		
		const aiScene* m_ai_scene = nullptr;
		Assimp::Importer m_ai_importer;

		//These are mRootNode->mTransformation and it's inverse - not actually used
		glm::mat4 m_global_transform{ 1.0f };
		glm::mat4 m_inverse_global_transform{ 1.0f };

		std::vector<MeshAOS> m_meshes;
		std::unordered_map<std::string, TextureStruct> m_loaded_textures;
		std::string m_directory{ "" };

		BoneTransformSOA m_bone_transforms_soa;

		std::unordered_map<std::string, uint32_t> m_map_bone_name_to_id;
		std::unordered_map<std::string, aiNodeAnim*> m_map_node_name_to_node_anim;
	};


//--------------------------------------------------------------
	class BasicModelTest : public Layer
	{
	public:
		BasicModelTest(Window& window, Camera& camera);
		void Startup() override;
		void Shutdown() override;
		void OnUpdate(double now, double time_step) override;
		void ImGuiUpdate() override;

	private:
		Camera& m_camera;
		Window& m_window;

		std::unique_ptr<Shader> m_shader = nullptr;
		std::unique_ptr<Shader> m_shader_animated_model = nullptr;
		std::unique_ptr<Shader> m_shader_static_model = nullptr;
		me::Model* m_model = nullptr;
	};
}




