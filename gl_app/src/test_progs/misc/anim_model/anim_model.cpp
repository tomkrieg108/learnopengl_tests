#include "pch.h"
#include <GL/glew.h>

#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image/stb_image.h>

#include "shader.h"
#include "camera.h"
#include "anim_model.h"


/*
	Animated model
	Possible optimisations:
		- in FindScalingIndex() etc - loops through each keyframe to find the current one).  Does this also for position and rotation every render frame.  Should be able to setup to avoid this looping

		- calls CalculateBoneTransforms() in OnUpdate() i.e. every render frame.  this traverses the node tree which presumably is strewn throughout heap.  To improve data locality / cache, setup this traversal to iterate over contigupus memory block 

		- final bone transforms could be precalculated for every frame and pre-loaded into shader (at expense of more memory use)

		To Fix
		 - for models with no animation, this crashes!
*/

namespace me
{
	/****************************************************************************************
	Convert math structures - assimp tp glm. Print matrices
	*****************************************************************************************/
	
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}

	static void PrintMat4(const glm::mat4& m)
	{
		std::cout.precision(5);
		std::cout << "\t" << m[0][0] << "\t" << m[1][0] << "\t" << m[2][0] << "\t" << m[3][0] << "\n";
		std::cout << "\t" << m[0][1] << "\t" << m[1][1] << "\t" << m[2][1] << "\t" << m[3][1] << "\n";
		std::cout << "\t" << m[0][2] << "\t" << m[1][2] << "\t" << m[2][2] << "\t" << m[3][2] << "\n";
		std::cout << "\t" << m[0][3] << "\t" << m[1][3] << "\t" << m[2][3] << "\t" << m[3][3] << "\n";
	}

	static void PrintMat4Flat(const glm::mat4& m)
	{
		std::cout.precision(2);
		std::cout << "\t" << m[0][0] << "," << m[1][0] << "," << m[2][0] << "," << m[3][0] << "|";
		std::cout << m[0][1] << "," << m[1][1] << "," << m[2][1] << "," << m[3][1] << "|";
		std::cout << m[0][2] << "," << m[1][2] << "," << m[2][2] << "," << m[3][2] << "|";
		std::cout << m[0][3] << "," << m[1][3] << "," << m[2][3] << "," << m[3][3] << "\n ";
	}

	/****************************************************************************************
	Load a texture, setup buffer for texture, set parameters, return render id of texture
	*****************************************************************************************/
	static uint32_t TextureFromFile(const char* path, const std::string& directory, bool gamma)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	/****************************************************************************************
	Functions to print info about scene / model / meshes etc
	*****************************************************************************************/
	static void PrintSceneInfo(const aiScene* ai_scene)
	{
		std::cout << "------------------------------------------------\n";
		std::cout << "Scene Info\n";
		std::cout << "------------------------------------------------\n";
		std::cout << "Scene Name: " << ai_scene->mName.C_Str() << std::endl;
		std::cout << std::boolalpha;
		std::cout << "Has Animations:\t" << ai_scene->HasAnimations() << std::endl;
		std::cout << "Has Cameras:\t" << ai_scene->HasCameras() << std::endl;
		std::cout << "Has Lights:\t" << ai_scene->HasLights() << std::endl;
		std::cout << "Has Materials:\t" << ai_scene->HasMaterials() << std::endl;
		std::cout << "Has Meshes:\t" << ai_scene->HasMeshes() << std::endl;
		std::cout << "Has Textures:\t" << ai_scene->HasTextures() << std::endl;
		std::cout << std::noboolalpha;
		std::cout << "Num Animations:\t" << ai_scene->mNumAnimations << std::endl;
		std::cout << "Num Cameras:\t" << ai_scene->mNumCameras << std::endl;
		std::cout << "Num Lights:\t" << ai_scene->mNumLights << std::endl;
		std::cout << "Num Materials:\t" << ai_scene->mNumMaterials << std::endl;
		std::cout << "Num Meshes:\t" << ai_scene->mNumMeshes << std::endl;
		std::cout << "Num Textures:\t" << ai_scene->mNumTextures << std::endl;
		std::cout << "Num Skeletons:\t" << ai_scene->mNumSkeletons << "\n";
	
		uint32_t tot_vertices = 0, tot_indices = 0, tot_faces = 0, tot_bones = 0;
		for (uint32_t mesh_idx = 0; mesh_idx < ai_scene->mNumMeshes; mesh_idx++)
		{
			aiMesh* ai_mesh = ai_scene->mMeshes[mesh_idx];
			tot_vertices += ai_mesh->mNumVertices;
			tot_bones += ai_mesh->mNumBones;
			tot_faces += ai_mesh->mNumFaces;
			for (uint32_t face_idx = 0; face_idx < ai_mesh->mNumFaces; ++face_idx)
				tot_indices += ai_mesh->mFaces[face_idx].mNumIndices;
		}
		std::cout
			<< "\nTot Vertices: " << tot_vertices
			<< "\tTot Faces: " << tot_faces
			<< "\tTot Indices: " << tot_indices
			<< "\tTot Bones: " << tot_bones
			<< "\n\n";
	}

	//only called in PrintAllMeshInfo()
	static void PrintMeshInfo(aiMesh* ai_mesh, uint32_t mesh_index)
	{
		std::cout << "------------------------------------------------\n";
		std::cout << "Mesh Info\t" << "Idx: " << mesh_index << "\n";
		std::cout << "------------------------------------------------\n";
		std::cout << "Mesh Name: " << ai_mesh->mName.C_Str() << std::endl;
		std::cout << std::boolalpha;
		std::cout << "Has Bones:\t" << ai_mesh->HasBones() << std::endl;
		std::cout << "Has Faces:\t" << ai_mesh->HasFaces() << std::endl;
		std::cout << "Has Normals:\t" << ai_mesh->HasNormals() << std::endl;
		std::cout << "Has Positions:\t" << ai_mesh->HasPositions() << std::endl;
		std::cout << "Has Tang,Bitang:\t" << ai_mesh->HasTangentsAndBitangents() << std::endl;
		std::cout << "Has Text Coords:\t" << ai_mesh->HasTextureCoords(0) << std::endl;
		std::cout << "Has Vertex Colours:\t" << ai_mesh->HasVertexColors(0) << std::endl;
		std::cout << std::noboolalpha;
		std::cout << "Num Colour Channels:\t" << ai_mesh->GetNumColorChannels() << std::endl;
		std::cout << "Num UV Channels:\t" << ai_mesh->GetNumUVChannels() << std::endl;
		std::cout << "Num Faces:\t" << ai_mesh->mNumFaces << std::endl;
		std::cout << "Num Vertices:\t" << ai_mesh->mNumVertices << std::endl;
		
		std::cout << "Num Bones:\t" << ai_mesh->mNumBones << std::endl;
		std::cout << "Num AnimMeshes:\t" << ai_mesh->mNumAnimMeshes << std::endl;
		std::cout << "Material Index:\t" << ai_mesh->mMaterialIndex << std::endl;

		uint32_t num_indices = 0;
		for (uint32_t face_idx = 0; face_idx < ai_mesh->mNumFaces; ++face_idx)
			num_indices += ai_mesh->mFaces[face_idx].mNumIndices;
		std::cout << "Num Indices:\t" << num_indices << "\n\n";
	}
	
	static void PrintAllMeshInfo(const aiScene* ai_scene)
	{
		for (uint32_t i = 0; i < ai_scene->mNumMeshes; i++)
			PrintMeshInfo(ai_scene->mMeshes[i], i);
	}

	//only called in PrintAnimationInfo()
	static void PrintNodeAnimInfo(aiNodeAnim* ai_node_anim, uint32_t index)
	{
		std::cout << index << ": Node:" << ai_node_anim->mNodeName.C_Str()
			<< " Pos Keys: " << ai_node_anim->mNumPositionKeys 
			<< " Rot Keys: " << ai_node_anim->mNumRotationKeys
			<< " Scale Keys: " << ai_node_anim->mNumScalingKeys << "\n";
	}

	static void PrintAnimationInfo(const aiScene* ai_scene)
	{
		for (uint32_t i = 0; i < ai_scene->mNumAnimations; i++)
		{
			aiAnimation* ai_anim = ai_scene->mAnimations[i];

			std::cout << "------------------------------------------------\n";
			std::cout << "Animation Info\t" << "Idx: " << i << "\n";
			std::cout << "------------------------------------------------\n";
			std::cout << "Animation Name: " << ai_anim->mName.C_Str() << std::endl;
			std::cout << "Duration (ticks):\t" << ai_anim->mDuration << std::endl; 
			std::cout << "Channels:\t" << ai_anim->mNumChannels << std::endl;
			std::cout << "Mesh Channels:\t" << ai_anim->mNumMeshChannels << std::endl;
			std::cout << "Morph Mesh Channels:\t" << ai_anim->mNumMorphMeshChannels << std::endl;
			std::cout << "Ticks per sec:\t" << ai_anim->mTicksPerSecond << std::endl;
			std::cout << "\n";

			for (uint32_t channel = 0; channel < ai_anim->mNumChannels; channel++)
			{
				aiNodeAnim* ai_node_anim = ai_anim->mChannels[channel];
				PrintNodeAnimInfo(ai_node_anim, channel);
			}
		}
	}

	static uint32_t tot_node_count = 0;
	static void PrintNodeTree(aiNode* ai_node, const aiScene* ai_scene, uint32_t level)
	{
		if (ai_node->mNumChildren > 0)
			std::cout << ++tot_node_count;

		std::cout << "\t";
		for (uint32_t i = 0; i < level; i++)
			std::cout << " ";

		std::cout << ai_node->mName.C_Str() << ":" <<
			" Level: " << level <<
			" Num Children: " << ai_node->mNumChildren <<
			" Num Meshes: " << ai_node->mNumMeshes << "\n";

		//PrintMat4Flat(ConvertMatrixToGLMFormat(ai_node->mTransformation));
		for (uint32_t i = 0; i < ai_node->mNumChildren; i++)
			PrintNodeTree(ai_node->mChildren[i], ai_scene, level + 1);
	}

	static void PrintNodeHeirarchy(const aiScene* ai_scene)
	{
		std::cout << "-----------------------------------------------------------------------\n";
		std::cout << "Node Hierachy\n";
		std::cout << "-----------------------------------------------------------------------\n";
		PrintNodeTree(ai_scene->mRootNode, ai_scene, 0);
		std::cout << "-----------------------------------------------------------------------\n";
	}


	//------------------------------------------------------------------------

	Model::Model(const std::string& file)
	{
		Load(file);
	}

	Model::~Model()
	{
	}

	void Model::Draw(Shader& shader)
	{
		for (uint32_t i = 0; i < m_meshes.size(); i++)
			m_meshes[i].Draw(shader);
	}

	bool Model::Load(const std::string& file)
	{
		m_ai_scene = m_ai_importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

		if (m_ai_scene == nullptr)
		{
			std::cout << "me::Model::Load() Could Not Load: " << file << std::endl;
			return false;
		}

		m_directory = file.substr(0, file.find_last_of('/'));
		m_meshes.resize(m_ai_scene->mNumMeshes);

		for (uint32_t mesh_index = 0; mesh_index < m_ai_scene->mNumMeshes; ++mesh_index)
		{
			ParseMesh(m_ai_scene, mesh_index);
			ParseBones(m_ai_scene, mesh_index);
			m_meshes[mesh_index].SetupMesh();
			CheckBones(m_ai_scene, mesh_index);
		}

		for (uint32_t i = 0; i < m_ai_scene->mNumAnimations; i++)
		{
			aiAnimation* ai_anim = m_ai_scene->mAnimations[i];
			for (uint32_t channel = 0; channel < ai_anim->mNumChannels; channel++)
			{
				aiNodeAnim* ai_node_anim = ai_anim->mChannels[channel];
				std::string node_anim_name = ai_node_anim->mNodeName.C_Str();
				m_map_node_name_to_node_anim[node_anim_name] = ai_node_anim;
			}
		}

		m_global_transform = ConvertMatrixToGLMFormat(m_ai_scene->mRootNode->mTransformation);
		m_inverse_global_transform = glm::inverse(m_global_transform);

		//Printout model info for "research"purposes
		PrintSceneInfo(m_ai_scene);
		PrintAllMeshInfo(m_ai_scene);
		PrintAnimationInfo(m_ai_scene);
		PrintNodeHeirarchy(m_ai_scene);

		//aiAnimation* ai_anim = m_ai_scene->mAnimations[0];
		//CalculateBoneTransforms(ai_anim, m_ai_scene->mRootNode, glm::mat4(1.0f), 0.0f);
			
		return true;
	}

	static uint32_t FindScalingIndex(float anim_time_ticks, aiNodeAnim* ai_node_anim)
	{
		for (uint32_t i = 0; i < ai_node_anim->mNumScalingKeys - 1; i++)
		{
			//aiVectorKey vk = ai_node_anim->mScalingKeys[i + 1];
			float t = (float)ai_node_anim->mScalingKeys[i + 1].mTime;
			if (anim_time_ticks < t)
				return i;
		}
		return 0;
	}

	static uint32_t FindPositionIndex(float anim_time_ticks, aiNodeAnim* ai_node_anim)
	{
		for (uint32_t i = 0; i < ai_node_anim->mNumPositionKeys - 1; i++)
		{
			float t = (float)ai_node_anim->mPositionKeys[i + 1].mTime;
			if (anim_time_ticks < t)
				return i;
		}
		return 0;
	}

	static uint32_t FindRotationIndex(float anim_time_ticks, aiNodeAnim* ai_node_anim)
	{
		for (uint32_t i = 0; i < ai_node_anim->mNumRotationKeys - 1; i++)
		{
			float t = (float)ai_node_anim->mRotationKeys[i + 1].mTime;
			if (anim_time_ticks < t)
				return i;
		}
		return 0;
	}

	static glm::vec3 GetInterpolatedScaling(float anim_time_ticks, aiNodeAnim* ai_node_anim)
	{
		if (ai_node_anim->mNumScalingKeys == 1)
			return GetGLMVec(ai_node_anim->mScalingKeys[0].mValue);
		
		uint32_t cur_index = FindScalingIndex(anim_time_ticks, ai_node_anim);
		uint32_t next_index = cur_index + 1;
		assert(next_index < ai_node_anim->mNumScalingKeys);
		float t1 = (float)ai_node_anim->mScalingKeys[cur_index].mTime;
		float t2 = (float)ai_node_anim->mScalingKeys[next_index].mTime;

		float factor = (anim_time_ticks - t1) / (t2 - t1);
		assert(factor >= 0.0f);
		assert(factor <= 1.0f);

		glm::vec3 scale_t1 = GetGLMVec(ai_node_anim->mScalingKeys[cur_index].mValue);
		glm::vec3 scale_t2 = GetGLMVec(ai_node_anim->mScalingKeys[next_index].mValue);

		glm::vec3 scale_interpolated = scale_t1 + (scale_t2 - scale_t1) * factor;
		return scale_interpolated;
	}

	static glm::vec3 GetInterpolatedPosition(float anim_time_ticks, aiNodeAnim* ai_node_anim)
	{
		if (ai_node_anim->mNumPositionKeys == 1)
			return GetGLMVec(ai_node_anim->mPositionKeys[0].mValue);

		uint32_t cur_index = FindPositionIndex(anim_time_ticks, ai_node_anim);
		uint32_t next_index = cur_index + 1;
		assert(next_index < ai_node_anim->mNumPositionKeys);
		float t1 = (float)ai_node_anim->mPositionKeys[cur_index].mTime;
		float t2 = (float)ai_node_anim->mPositionKeys[next_index].mTime;

		float factor = (anim_time_ticks - t1) / (t2 - t1);
		assert(factor >= 0.0f);
		assert(factor <= 1.0f);

		glm::vec3 pos_t1 = GetGLMVec(ai_node_anim->mPositionKeys[cur_index].mValue);
		glm::vec3 pos_t2 = GetGLMVec(ai_node_anim->mPositionKeys[next_index].mValue);

		glm::vec3 pos_interpolated = pos_t1 + (pos_t2 - pos_t1) * factor;
		return pos_interpolated;
	}

	static glm::quat GetInterpolatedRotation(float anim_time_ticks, aiNodeAnim* ai_node_anim)
	{
		if (ai_node_anim->mNumRotationKeys == 1)
			return GetGLMQuat(ai_node_anim->mRotationKeys[0].mValue);

		uint32_t cur_index = FindRotationIndex(anim_time_ticks, ai_node_anim);
		uint32_t next_index = cur_index + 1;
		assert(next_index < ai_node_anim->mNumRotationKeys);
		float t1 = (float)ai_node_anim->mRotationKeys[cur_index].mTime;
		float t2 = (float)ai_node_anim->mRotationKeys[next_index].mTime;

		float factor = (anim_time_ticks - t1) / (t2 - t1);
		assert(factor >= 0.0f);
		assert(factor <= 1.0f);

		aiQuaternion rot_t1 = ai_node_anim->mRotationKeys[cur_index].mValue;
		aiQuaternion rot_t2 = ai_node_anim->mRotationKeys[next_index].mValue;

		aiQuaternion rot_interpolated;
		aiQuaternion::Interpolate(rot_interpolated, rot_t1, rot_t2, factor);
		rot_interpolated.Normalize();
		return GetGLMQuat(rot_interpolated);
	}

	void Model::CalculateBoneTransforms(aiAnimation* ai_anim, aiNode* ai_node, const glm::mat4 parent_transform, float time_secs)
	{
		float ticks_per_sec = 0.0f;
		float time_in_ticks = 0.0f;
		float animation_time_ticks = 0.0f;
		if (ai_anim != nullptr)
		{
			ticks_per_sec = (ai_anim->mTicksPerSecond == 0 ? 25.0f : ai_anim->mTicksPerSecond);
			time_in_ticks = time_secs * ticks_per_sec;
			animation_time_ticks = std::fmod(time_in_ticks, ai_anim->mDuration);
		}

		std::string node_name = ai_node->mName.C_Str();
		glm::mat4 node_transform = glm::mat4(1.0f);
		
		aiNodeAnim* ai_node_anim = nullptr;
		if(m_map_node_name_to_node_anim.find(node_name) != m_map_node_name_to_node_anim.end())
			ai_node_anim = m_map_node_name_to_node_anim[node_name];

		if (ai_node_anim != nullptr)
		{
			glm::vec3 scaling = GetInterpolatedScaling(animation_time_ticks, ai_node_anim);
			glm::mat4 mat_scale = glm::scale(glm::mat4(1.0f), scaling);

			glm::vec3 translation = GetInterpolatedPosition(animation_time_ticks, ai_node_anim);
			glm::mat4 mat_translation = glm::translate(glm::mat4(1.0f), translation);

			glm::quat rotation_q = GetInterpolatedRotation(animation_time_ticks, ai_node_anim);
			glm::mat4 mat_rotation = glm::toMat4(rotation_q);
			node_transform = mat_translation * mat_rotation * mat_scale;
		}

		/*
			mRootNode has ai_node->mTransformation which scales right down - so all child nodes get scaled right down too.
			m_global_transform is set to this down scaling, and m_inverse_global_transform will scale it back up again.
			since mRootNode is not a bone name, ai_node_anim will be nullptr so node_transform stays at identity when animation is running, so all child nodes are not scaled down.
		*/
		glm::mat4 node_transform_pose = ConvertMatrixToGLMFormat(ai_node->mTransformation);
		glm::mat4 cumulative_transform = parent_transform * node_transform;
		//cumulative_transform = parent_transform * node_transform_pose;

		/*std::cout << "Pose -------------------------------------------------" << node_name << "\n";
		PrintMat4(node_transform_pose);
		std::cout << "Animated ---------------------------------------------" << node_name << "\n";
		PrintMat4(node_transform);
		std::cout << "Diff ---------------------------------------------" << node_name << "\n";
		PrintMat4(node_transform_pose - node_transform);
		std::cout << "#######################################################\n";*/

		if (m_map_bone_name_to_id.find(node_name) != m_map_bone_name_to_id.end())
		{
			uint32_t bone_index = m_map_bone_name_to_id[node_name];
			
			assert(m_bone_transforms_soa.bone_id[bone_index] == bone_index);
			assert(m_bone_transforms_soa.bone_names[bone_index] == node_name);

			glm::mat4 offset_transform = m_bone_transforms_soa.offset_transforms[bone_index];

			/*m_bone_transforms_soa.final_transforms[bone_index] = m_inverse_global_transform * cumulative_transform * offset_transform;*/

			m_bone_transforms_soa.final_transforms[bone_index] =  cumulative_transform * offset_transform;
		}

		for (uint32_t i = 0; i < ai_node->mNumChildren; ++i)
		{
			CalculateBoneTransforms(ai_anim, ai_node->mChildren[i], cumulative_transform, time_secs);
		}
	}


	void Model::ParseMesh(const aiScene* ai_scene, uint32_t mesh_index)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[mesh_index];
	
		m_meshes[mesh_index].m_vertices.resize(ai_mesh->mNumVertices);
		uint32_t num_indices = 0;
		for (uint32_t face_idx = 0; face_idx < ai_mesh->mNumFaces; ++face_idx)
			num_indices += ai_mesh->mFaces[face_idx].mNumIndices;
		m_meshes[mesh_index].m_indices.reserve(num_indices);

		for (uint32_t i = 0; i < ai_mesh->mNumVertices; ++i)
		{
			Vertex vertex;
			glm::vec3 vec3;

			vertex.Position.x = ai_mesh->mVertices[i].x;
			vertex.Position.y = ai_mesh->mVertices[i].y;
			vertex.Position.z = ai_mesh->mVertices[i].z;

			if (ai_mesh->HasNormals())
			{
				vertex.Normal.x = ai_mesh->mNormals[i].x;
				vertex.Normal.y = ai_mesh->mNormals[i].y;
				vertex.Normal.z = ai_mesh->mNormals[i].z;
			}
			if (ai_mesh->HasTextureCoords(0))
			{
				// a vertex can contain up to 8 different texture coordinates. We make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vertex.TexCoords.x = ai_mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = ai_mesh->mTextureCoords[0][i].y;
				// tangent
				vertex.Tangent.x = ai_mesh->mTangents[i].x;
				vertex.Tangent.y = ai_mesh->mTangents[i].y;
				vertex.Tangent.z = ai_mesh->mTangents[i].z;
				// bitangent
				vertex.Bitangent.x = ai_mesh->mBitangents[i].x;
				vertex.Bitangent.y = ai_mesh->mBitangents[i].y;
				vertex.Bitangent.z = ai_mesh->mBitangents[i].z;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			m_meshes[mesh_index].m_vertices[i] = vertex;
		}

		// setup m_indices 
		for (uint32_t face_idx = 0; face_idx < ai_mesh->mNumFaces; face_idx++)
		{
			aiFace face = ai_mesh->mFaces[face_idx];
			// retrieve all indices of the face and store them in the indices vector
			for (uint32_t vert_idx = 0; vert_idx < face.mNumIndices; vert_idx++)
				m_meshes[mesh_index].m_indices.push_back(face.mIndices[vert_idx]);
			
		}

		//process materials
		aiMaterial* material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
		loadMaterialTextures(mesh_index, material, aiTextureType_DIFFUSE, "texture_diffuse");
		loadMaterialTextures(mesh_index, material, aiTextureType_SPECULAR, "texture_specular");
		loadMaterialTextures(mesh_index, material, aiTextureType_AMBIENT, "texture_ambient");
		loadMaterialTextures(mesh_index, material, aiTextureType_NORMALS, "texture_normal");
		loadMaterialTextures(mesh_index, material, aiTextureType_EMISSIVE, "texture_emissive");
		//loadMaterialTextures(mesh_index, material, aiTextureType_HEIGHT, "texture_normal");
		//loadMaterialTextures(mesh_index, material, aiTextureType_AMBIENT, "texture_height");
	}

	void Model::ParseBones(const aiScene* ai_scene, uint32_t mesh_index)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[mesh_index];

		if (ai_mesh->HasBones())
		{
			for (uint32_t bone_id = 0; bone_id < ai_mesh->mNumBones; ++bone_id)
			{
				aiBone* ai_bone = ai_mesh->mBones[bone_id];
				std::string bone_name = ai_bone->mName.C_Str();

				if (m_map_bone_name_to_id.find(bone_name) == m_map_bone_name_to_id.end())
				{
					m_map_bone_name_to_id[bone_name] = bone_id;
					m_bone_transforms_soa.bone_names.push_back(bone_name);
					m_bone_transforms_soa.bone_id.push_back(bone_id);
					m_bone_transforms_soa.offset_transforms.push_back(ConvertMatrixToGLMFormat(ai_bone->mOffsetMatrix));
					m_bone_transforms_soa.final_transforms.push_back(glm::mat4(1.0f));
				}

				//if (bone_id == 12 || bone_id == 31)
				//	std::cout << "\n***************************************************************\n";

				/*std::cout << ai_bone->mName.C_Str() << "\tBone Idx:\t" << bone_id << "\tVerts affected:\t" << ai_bone->mNumWeights << "\n";*/

				for (uint32_t i = 0; i < ai_bone->mNumWeights; ++i)
				{
					aiVertexWeight ai_weight = ai_bone->mWeights[i];

					//if (bone_id == 12 || bone_id == 31)
					//	std::cout << "vertID: " << ai_weight.mVertexId << "\tWeight: " << ai_weight.mWeight << "\n";
					
					Vertex vertex = m_meshes[mesh_index].m_vertices[ai_weight.mVertexId];

					for (uint32_t j = 0; j < MAX_BONE_INFLUENCE; ++j)
					{
						if (vertex.BoneWeight[j] == 0)
						{
							vertex.BoneId[j] = bone_id;
							vertex.BoneWeight[j] = ai_weight.mWeight;
							m_meshes[mesh_index].m_vertices[ai_weight.mVertexId] = vertex;
							break;
						}
					}
				}
			}
			//std::cout << "\n\n";
		}
	}

	void Model::CheckBones(const aiScene* ai_scene, uint32_t mesh_index)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[mesh_index];
		if (ai_mesh->HasBones())
		{
			bool bones_valid = true;
			for (uint32_t i = 0; i < ai_mesh->mNumVertices; ++i)
			{
				Vertex vertex = m_meshes[mesh_index].m_vertices[i];
				uint32_t bone_count = 0;
				float tot_weight = 0;
				bool ok = true;
				for (uint32_t j = 0; j < MAX_BONE_INFLUENCE; ++j)
				{
					if (vertex.BoneWeight[j] > 0)
					{
						bone_count++;
						tot_weight += vertex.BoneWeight[j];
					}
				}
				ok = (bone_count > 0) && (std::abs(tot_weight - 1.0f) < 0.00001f);

				if (!ok)
				{
					std::cout << "Bone Data error: Vert ID: " << i << " bone count: " << bone_count << " bone weight: " << tot_weight << "\n\n";
					bones_valid = false;
				}
			}
			if (bones_valid)
				std::cout << "Bone data *seems* valid!\n\n";
		}
	}

	void Model::loadMaterialTextures(uint32_t mesh_idx, aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<TextureStruct> textures;
		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			std::string texture_name = str.C_Str();
			if (m_loaded_textures.find(texture_name) == m_loaded_textures.end())
			{
				TextureStruct texture;
				texture.id = TextureFromFile(str.C_Str(), this->m_directory, false);
				texture.type = typeName;
				texture.path = texture_name;
				m_loaded_textures[texture_name] = texture;
			}

			m_meshes[mesh_idx].m_textures.push_back(m_loaded_textures[texture_name]);
		}
	}
	//---------------------------------------------------------------------------------


	// initializes all the buffer objects/arrays
	void MeshAOS::SetupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneId));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneWeight));
		glBindVertexArray(0);
	}

	void MeshAOS::Draw(Shader& shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = m_textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to string
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to string
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to string

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.GetProgramID(), (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		}

		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	//---------------------------------------------------------------------------------
	BasicModelTest::BasicModelTest(Window& window, Camera& camera) :
		m_window{ window }, m_camera{ camera }
	{
	}

	void BasicModelTest::Startup()
	{
		glEnable(GL_DEPTH_TEST); 
		glDepthFunc(GL_LESS);

		//comment out this for the backpack model
		stbi_set_flip_vertically_on_load(true);

		//m_model = new me::Model{ "assets/models/backpack/backpack.obj" };
		//m_model = new me::Model{ "assets/models/nanosuit/nanosuit.obj" };
		m_model = new me::Model{ "assets/models/Hip Hop Dancing/Hip Hop Dancing.dae" };

		const aiScene* ai_scene = m_model->GetAIScene();
		assert(ai_scene != nullptr);
		if (ai_scene->HasAnimations())
		{
			ShaderBuilder shader_builder("src/test_progs/misc/anim_model/");
			m_shader = shader_builder.Vert("animated_model.vs").Frag("animated_model.fs").Build("AnimatedModelShader");
		}
		else
		{
			ShaderBuilder shader_builder("src/test_progs/learn_opengl/3.model_loading/");
			m_shader = shader_builder.Vert("1.model_loading.vs").Frag("1.model_loading.fs").Build("StaticModelShader");
		}
		//m_shader->OutputInfo();
	}

	void BasicModelTest::Shutdown()
	{
	}

	void BasicModelTest::OnUpdate(double now, double time_step)
	{
		// render the loaded model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
			
		m_shader->Bind();
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		
		const aiScene* ai_scene = m_model->GetAIScene();
		if (ai_scene->HasAnimations())
		{
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f)); //needed for the animated model
			aiAnimation* ai_anim = ai_scene->mAnimations[0];
			m_model->CalculateBoneTransforms(ai_anim, ai_scene->mRootNode, glm::mat4(1.0f), now);
			const BoneTransformSOA& bone_transforms = m_model->GetBoneTransforms();
			auto num_transforms = bone_transforms.bone_names.size();
			assert(num_transforms <= 100);
			for (auto i = 0; i < num_transforms; ++i)
				m_shader->SetUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", bone_transforms.final_transforms[i]);
		}

		m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
		m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
		m_shader->SetUniformMat4f("model", model);
		
		m_model->Draw(*m_shader);
	}

	void BasicModelTest::ImGuiUpdate()
	{
	}

}
