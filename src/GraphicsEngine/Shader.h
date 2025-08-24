#pragma once
//#include <memory>

enum ShaderType
{
	VertexShader = 0,
	FragmentShader
};

typedef struct ShaderDesc
{
	char* vertexShaderBuffer;
	char* fragmentShaderBuffer;
}ShaderDesc;



class Shader
{
public:
	Shader(const ShaderDesc& desc);
	void init(const ShaderDesc& desc);
	~Shader();
	
	unsigned int getID();
private:
	void attach(char* buffer, const ShaderType& type);
	void link();
private:
	unsigned int programID;
	unsigned int attachedShaders[2] = {};
};

//typedef std::shared_ptr<Shader> ShaderPtr;