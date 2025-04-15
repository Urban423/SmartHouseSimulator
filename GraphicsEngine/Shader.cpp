#include "Shader.h"
#include "GraphicConfig.h"

Shader::Shader(const ShaderDesc& desc)
{
	init(desc);
}

void Shader::init(const ShaderDesc& desc)
{
	programID = glCreateProgram();
	attach(desc.vertexShaderBuffer, VertexShader);
	attach(desc.fragmentShaderBuffer, FragmentShader);
	link();
}

Shader::~Shader()
{
	#ifdef glad
	for(unsigned int i = 0; i < 2; i++)
	{
		glDetachShader(programID, attachedShaders[i]);
		glDeleteShader(attachedShaders[i]);
	}
	
	glDeleteProgram(programID);
	#endif
}


void Shader::attach(char* buffer, const ShaderType& type)
{
	
	unsigned int shaderID = 0;
	if(type == VertexShader) {
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	}
	else {
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	}
	glShaderSource(shaderID, 1, &buffer, 0);
	glCompileShader(shaderID);
	
	glAttachShader(programID, shaderID);
	attachedShaders[type] = shaderID;
}

void Shader::link()
{
	glLinkProgram(programID);
}

unsigned int Shader::getID()
{
	return programID;
}