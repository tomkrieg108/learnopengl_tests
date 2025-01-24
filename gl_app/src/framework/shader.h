#pragma once

#include <glm\glm.hpp>
//#include <glm\gtc\matrix_transform.hpp>

class Shader;

class ShaderBuilder
{
public:

	ShaderBuilder(std::string const& shader_directory);
	void SetShaderDirectory(std::string const& shader_directory);
	ShaderBuilder& Vert(const std::string& filename);
	ShaderBuilder& TessCtrl(const std::string& filename);
	ShaderBuilder& TessEval(const std::string& filename);
	ShaderBuilder& Frag(const std::string& filename);
	ShaderBuilder& Geom(const std::string& filename);
	ShaderBuilder& Comp(const std::string& filename);
	std::unique_ptr<Shader> Build(const std::string& name);

private:

	struct ShaderInfo
	{
		std::string filepath = "";
		uint32_t type = 0;
		uint32_t id = 0;
		bool compile_success = false;
	};

private:
	std::string ReadSource(const std::string& filepath);
	ShaderInfo Compile(uint32_t type, const std::string& filepath);
	bool ValidationCheck(uint32_t program);
	void OutputShaderInfoLog();
	void OutputProgramInfoLog(uint32_t program);
	std::string m_shader_directory;

private:
	std::vector<ShaderInfo> m_shader_list;
	static const std::string SHADER_DIRECTORY;
};


class Shader
{
	friend class ShaderBuilder;
public:
	
	Shader() {}
	void Bind();
	void Unbind();
	
	//Set uniforms
	void SetUniform1f(const std::string& name, float v0);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform2f(const std::string& name, glm::vec2& data);
	void SetUniform3f(const std::string& name, glm::vec3& data);
	void SetUniform4f(const std::string& name, glm::vec4& data);
	void SetUniform1i(const std::string& name, int v0);
	void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

	uint32_t GetProgramID() const { return m_program_id; }
	std::string GetName() { return m_name; }
	void Validate();
	bool IsValid() const { return m_is_valid; }
	void OutputInfo();
	
private:
	
	int32_t GetUniformLocation(const std::string& name);

	struct DataItem
	{
		std::string name;
		int location;
		uint32_t gl_type;
	};
	
private:
	std::string m_name = "";
	bool m_is_valid = true;
	uint32_t m_program_id = 0;
	std::unordered_map<std::string, int> m_uniform_location_cache;

	std::vector<DataItem> m_attributes;
	std::vector<DataItem> m_uniforms;

	void ReadUniforms();
	void ReadAttributes();
};

const std::string GLTypeToString(GLenum type);
void DisplayUniformValue(uint32_t program, int location, GLenum type);
