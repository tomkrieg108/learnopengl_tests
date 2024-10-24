#include "pch.h"
#include <GL/glew.h>
#include "shader.h"

ShaderBuilder::ShaderBuilder(std::string const& shader_directory = "assets/shaders/") :
	m_shader_directory(shader_directory)
{
}

void ShaderBuilder::SetShaderDirectory(std::string const& shader_directory)
{
	m_shader_directory = shader_directory;
}

ShaderBuilder& ShaderBuilder::Vert(const std::string& filename)
{
	auto filepath = m_shader_directory + filename;
	auto info = Compile(GL_VERTEX_SHADER, filepath);
	m_shader_list.push_back(info);
	return *this;
}

ShaderBuilder& ShaderBuilder::TessCtrl(const std::string& filename)
{
	auto filepath = m_shader_directory + filename;
	auto info = Compile(GL_TESS_CONTROL_SHADER, filepath);
	m_shader_list.push_back(info);
	return *this;
}

ShaderBuilder& ShaderBuilder::TessEval(const std::string& filename)
{
	auto filepath = m_shader_directory + filename;
	auto info = Compile(GL_TESS_EVALUATION_SHADER, filepath);
	m_shader_list.push_back(info);
	return *this;
}

ShaderBuilder& ShaderBuilder::Frag(const std::string& filename)
{
	auto filepath = m_shader_directory + filename;
	auto info = Compile(GL_FRAGMENT_SHADER, filepath);
	m_shader_list.push_back(info);
	return *this;
}

ShaderBuilder& ShaderBuilder::Geom(const std::string& filename)
{
	auto filepath = m_shader_directory + filename;
	auto info = Compile(GL_GEOMETRY_SHADER, filepath);
	m_shader_list.push_back(info);
	return *this;
}

ShaderBuilder& ShaderBuilder::Comp(const std::string& filename)
{
	auto filepath = m_shader_directory + filename;
	auto info = Compile(GL_COMPUTE_SHADER, filepath);
	m_shader_list.push_back(info);
	return *this;
}


std::string ShaderBuilder::ReadSource(const std::string& filepath)
{
	if (filepath.length() == 0)
		return "";
	std::ifstream stream(filepath);
	if (stream.fail())
	{
		std::cout << "Failed to open: " << filepath << " \n";
		return "";
	}
	std::string line;
	std::ostringstream ss;
	while (getline(stream, line))
	{
		ss << line << "\n";
	}
	return ss.str();
}

ShaderBuilder::ShaderInfo ShaderBuilder::Compile(uint32_t type, const std::string& filepath)
{
	std::string code = ReadSource(filepath);
	ShaderInfo info;
	info.id = glCreateShader(type);
	info.filepath = filepath;
	info.type = type;
	const char* src = code.c_str();
	glShaderSource(info.id, 1, &src, nullptr);
	glCompileShader(info.id);
	int32_t result;
	glGetShaderiv(info.id, GL_COMPILE_STATUS, &result);
	info.compile_success = (result == GL_TRUE ? true : false);
	return info;
}


void ShaderBuilder::OutputShaderInfoLog()
{
	for (auto& shader_info : m_shader_list)
	{
		std::cout << "---------------------------------\n";
		std::cout << "Shader info log: " << shader_info.filepath << "\n";
		int length;
		glGetShaderiv(shader_info.id, GL_INFO_LOG_LENGTH, &length);
		if (length > 0)
		{
			char* message = new char[length];
			glGetShaderInfoLog(shader_info.id, length, &length, message);
			std::cout << message << "\n";
		}
		else
		{
			std::cout << "Nil\n";
		}
	}
}

std::unique_ptr<Shader> ShaderBuilder::Build(const std::string& name)
{
	auto shader = std::make_unique<Shader>();
	uint32_t program = glCreateProgram();

	shader->m_name = name;
	shader->m_program_id = program;
	shader->m_is_valid = false;
	
	bool compile_errors = false;
	for (auto& shader_info : m_shader_list)
	{
		if (shader_info.compile_success)
			glAttachShader(program, shader_info.id);
		else
			compile_errors = true;
	}

	if (compile_errors)
		OutputShaderInfoLog();
	else
	{
		glLinkProgram(program);
		glValidateProgram(program);
		shader->m_is_valid = ValidationCheck(program);
	}

	for (auto& shader_info : m_shader_list)
		glDeleteShader(shader_info.id);
	
	m_shader_list.clear();
	return shader;
}

bool ShaderBuilder::ValidationCheck(uint32_t program)
{
	int32_t result = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Error linking shader program: \n";
	}
	result = GL_FALSE;

	glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Program validation failure: \n";
	}
	return (bool)result;
}

//-------------------------------------------------

void Shader::Bind()
{
	glUseProgram(m_program_id);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
{
	//todo - use glm::value_ptr instead of &matrix[0][0]
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	//todo - use glm::value_ptr instead of &matrix[0][0]
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniform2f(const std::string& name, glm::vec2& data)
{
	glUniform2f(GetUniformLocation(name), data[0], data[1]);
}

void Shader::SetUniform3f(const std::string& name, glm::vec3& data)
{
	glUniform3f(GetUniformLocation(name), data[0], data[1], data[2]);
}

void Shader::SetUniform4f(const std::string& name, glm::vec4& data)
{
	glUniform4f(GetUniformLocation(name), data[0], data[1], data[2], data[3]);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
	glUniform1i(GetUniformLocation(name), v0);
}

int32_t Shader::GetUniformLocation(const std::string& name)
{
	if (m_uniform_location_cache.find(name) != m_uniform_location_cache.end())
		return m_uniform_location_cache[name];
	int location = glGetUniformLocation(m_program_id, name.c_str());
	if (location == -1)
	{
		std::cout << m_name << " Warning: Uniform " << name << " doesn't exist\n";
	}
		
	m_uniform_location_cache[name] = location;
	return location;
}


void Shader::ReadUniforms()
{
	int32_t params = -1;
	const int kMaxLength = 100;
	glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORMS, &params);
	for (int32_t i = 0; i < params; i++)
	{
		Shader::DataItem item;
		char name[kMaxLength];
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveUniform(m_program_id, i, kMaxLength, &actual_length, &size, &type, name);
		item.gl_type = type;
		for (int32_t element = 0; element < size; element++)
		{
			if (size == 1)
			{
				item.location = glGetUniformLocation(m_program_id, name);
				item.name = std::string(name);
			}
			else
			{
				std::string item_name = std::string{ name };
				item_name = item_name.substr(0, item_name.size() - 3) + std::string("[") + std::to_string(element) + "]";
				item.location = glGetUniformLocation(m_program_id, item_name.c_str());
				item.name = item_name;
			}
			m_uniforms.push_back(item);
		}
	}
}

void Shader::ReadAttributes()
{
	int32_t params = -1;
	const int kMaxLength = 100;
	glGetProgramiv(m_program_id, GL_ACTIVE_ATTRIBUTES, &params);
	for (int32_t i = 0; i < params; i++)
	{
		Shader::DataItem item;
		char name[kMaxLength];
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveAttrib(m_program_id, i, kMaxLength, &actual_length, &size, &type, name);
		item.gl_type = type;
		for (int32_t element = 0; element < size; element++)
		{
			if (size == 1)
			{
				item.location = glGetAttribLocation(m_program_id, name);
				item.name = std::string(name);
			}
			else
			{
				std::string item_name = std::string{ name };
				item_name = item_name.substr(0, item_name.size() - 3) + std::string("[") + std::to_string(element) + "]";
				item.location = glGetAttribLocation(m_program_id, item_name.c_str());
				item.name = item_name;
			}
			m_attributes.push_back(item);
		}
	}
}

void Shader::OutputInfo()
{
	ReadAttributes();
	ReadUniforms();

	int32_t params = -1;

	std::cout << "SHADER PROHRAM NAME: " << m_name << "\n";
	std::cout << "PROGRAM ID: " << m_program_id << "\n";

	glGetProgramiv(m_program_id, GL_LINK_STATUS, &params);
	std::cout << "GL_LINK_STATUS:" << params << "\n";
	glGetProgramiv(m_program_id, GL_ATTACHED_SHADERS, &params);
	std::cout << "GL_ATTACHED_SHADERS:" << params << "\n\n";
	
	std::cout << "PROGRAM INFO LOG:\n";
	int length = 0;
	glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &length);
	if (length > 0)
	{
		char* message = new char[length];
		glGetProgramInfoLog(m_program_id, length, &length, message);
		std::cout << message << "\n";
	}
	else
	{
		std::cout << "Nil\n\n";
	}

	std::cout << "GL_ACTIVE_ATTRIBUTES: " << m_attributes.size() << "\n";
	for (auto& item : m_attributes)
	{
		std::cout << item.name << " - " << GLTypeToString(item.gl_type) << ", loc: "
			<< item.location << "\n";
	}

	std::cout << "\nGL_ACTIVE_UNIFORMS: " << m_uniforms.size() << "\n";
	for (auto& item : m_uniforms)
	{
		std::cout << item.name << " - " << GLTypeToString(item.gl_type) << ", loc: "
			<< item.location << "\n";
	}
	std::cout << "----------------------------------------------------\n\n";
}


const std::string GLTypeToString(GLenum type) 
{
	switch (type) 
	{
		case GL_BOOL: return "GL_BOOL";
		case GL_INT: return "GL_INT";
		case GL_FLOAT: return "GL_FLOAT";
		case GL_FLOAT_VEC2: return std::string("GL_FLOAT_VEC2"); break;
		case GL_FLOAT_VEC3: return std::string("GL_FLOAT_VEC3"); break;
		case GL_FLOAT_VEC4: return std::string("GL_FLOAT_VEC4"); break;
		case GL_FLOAT_MAT2: return std::string("GL_FLOAT_MAT2"); break;
		case GL_FLOAT_MAT3: return std::string("GL_FLOAT_MAT3"); break;
		case GL_FLOAT_MAT4: return std::string("GL_FLOAT_MAT4"); break;
		case GL_SAMPLER_2D: return std::string("GL_SAMPLER_2D"); break;
		case GL_SAMPLER_3D: return std::string("GL_SAMPLER_3D"); break;
		case GL_SAMPLER_CUBE: return std::string("GL_SAMPLER_CUBE"); break;
		case GL_SAMPLER_2D_SHADOW: return std::string("GL_SAMPLER_2D_SHADOW"); break;
		default: break;
	}
	return std::string("OTHER");
}

void DisplayUniformValue(uint32_t program, int location, GLenum type)
{
	if (type == GL_INT)
	{
		int val = 0;
		glGetUniformiv(program, location, &val);
		std::cout << " Val = " << val;
	}
	if (type == GL_SAMPLER_2D)
	{
		int val = 0;
		glGetUniformiv(program, location, &val);
		std::cout << " Val = " << val;
	}
	if (type == GL_FLOAT)
	{
		float val = 0;
		glGetUniformfv(program, location, &val);
		std::cout << " Val = " << val;
	}
}