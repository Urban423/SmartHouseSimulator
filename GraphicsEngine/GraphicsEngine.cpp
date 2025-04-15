#include "GraphicsEngine.h"
#include "GraphicConfig.h"
#include <malloc.h>



bool GraphicsEngine::init()
{
#ifdef glad
	gladLoadGL();
#endif
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDepthRangef(0.0f, 1.0f);
	return true;
}


void GraphicsEngine::enable3D() 	{ glEnable(GL_DEPTH_TEST);  glDepthFunc(GL_LESS);};
void GraphicsEngine::disable3D() 	{ glDisable(GL_DEPTH_TEST); glDepthFunc(GL_ALWAYS); };

void GraphicsEngine::clear(const Color& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void GraphicsEngine::clearDepthBuffer() 		{ glClear(GL_DEPTH_BUFFER_BIT); }
void GraphicsEngine::clearColorDepthBuffer() 	{ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void GraphicsEngine::setViewPort(const unsigned int xOffset, const unsigned int yOffset, const unsigned int width, const unsigned int height) { glViewport(xOffset, yOffset, width - xOffset, height - yOffset); }

void GraphicsEngine::drawTriangles(unsigned int vertexCount, void* offset)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, offset);
}

void GraphicsEngine::setVertexArrayObject(VertexArrayObject* vao)
{
	
	 glBindBuffer(GL_ARRAY_BUFFER, vao->getID());
	 
	 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3));
    glEnableVertexAttribArray(1);

    // UV attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
}


void GraphicsEngine::setIndexArrayObject(IndexArrayObject* iao)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iao->getID());
}

void GraphicsEngine::setShaderProgram(Shader* program)
{
	glUseProgram(program->getID());
}

void GraphicsEngine::setTexture(Texture* texture, Shader* shader)
{
	unsigned int transformLoc = glGetUniformLocation(shader->getID(), "_MainTex");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->getID()); 
	glUniform1i(transformLoc, 0);
}

void GraphicsEngine::setVector4(Shader* shader, Vector4 vector4) {
	unsigned int transformLoc = glGetUniformLocation(shader->getID(), "diff_color");
	glUniform4f(transformLoc, vector4.x, vector4.y, vector4.z, vector4.w);
	glUniform1i(transformLoc, 0);
}

void GraphicsEngine::setMatrix(Shader* shader, Matrix4x4& matrix)
{
	unsigned int transformLoc = glGetUniformLocation(shader->getID(), "_transform");
	glUniform4fv(transformLoc, 4, matrix.getPtr());
	//glUniformMatrix4fv(transformLoc, 1, false, matrix.getPtr());
}

void GraphicsEngine::setProjectionMatrix(Shader* shader, Matrix4x4& matrix)
{
	unsigned int transformLoc = glGetUniformLocation(shader->getID(), "_projection");
	glUniform4fv(transformLoc, 4, matrix.getPtr());
}

void GraphicsEngine::setCameraViewMatrix(Shader* shader, Matrix4x4& matrix)
{
	unsigned int transformLoc = glGetUniformLocation(shader->getID(), "_camView");
	glUniform4fv(transformLoc, 4, matrix.getPtr());
}

void GraphicsEngine::setCullMode(const CullMode& mode)
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	switch(mode)
	{
		case(BackFace):
		{
			glCullFace(GL_BACK);
			break;
		}
		case(FrontFace):
		{
			glCullFace(GL_FRONT);
			break;
		}
		case(Both):
		{
			glCullFace(GL_FRONT_AND_BACK);
			break;
		}
	}
}

VertexArrayObject* GraphicsEngine::createVertexArrayObject(const VertexBufferDesc& desc)
{
	VertexArrayObject* m = (VertexArrayObject*)malloc(sizeof(VertexArrayObject));
	m->init(desc);
	return m;
}

Shader* GraphicsEngine::createShaderProgram(const ShaderDesc& desc)
{
	Shader* m = (Shader*)malloc(sizeof(Shader));
	m->init(desc);
	return m;
}

IndexArrayObject* GraphicsEngine::createIndexArrayObject(const IndexArrayDesc& desc)
{
	IndexArrayObject* m = (IndexArrayObject*)malloc(sizeof(IndexArrayObject));
	m->init(desc);
	return m;
}

Texture* GraphicsEngine::createTexture(const TextureStruct desc)
{
	Texture* m = (Texture*)malloc(sizeof(Texture));
	m->init(desc);
	return m;
}