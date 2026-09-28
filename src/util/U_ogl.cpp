#include "U_ogl.h"

GLuint attach_shader( const GLuint program_id, GLenum shader_type, const std::filesystem::path& file_name ) {
	// load shader code
	std::string shader_code = "";

	std::ifstream f_stream( file_name );
	if ( !f_stream.is_open() ) {
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, "Error occured trying to open shader file: %s", file_name.string().c_str() );
		return 1;
	}

	std::string line = "";
	while ( std::getline( f_stream, line ) ) {
		shader_code += line + '\n';
	}

	f_stream.close();

	// attach shader code
	if ( program_id == 0 ) {
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, "Error occured during shader attachment: program_id was 0." );
		return 1;
	}

	GLuint shader_id = glCreateShader( shader_type );

	const char* source_ptr = shader_code.data();
	GLint source_len = static_cast<GLint>( shader_code.length() );
	
	glShaderSource( shader_id, 1, &source_ptr, &source_len );
	glCompileShader( shader_id );

	GLint result = GL_FALSE;
	glGetShaderiv( shader_id, GL_COMPILE_STATUS, &result );

	int info_len;
	glGetShaderiv( shader_id, GL_INFO_LOG_LENGTH, &info_len );

	if ( result == GL_FALSE || info_len != 0 ) {
		std::string error_msg( info_len, '\0' );
		glGetShaderInfoLog( shader_id, info_len, NULL, error_msg.data() );
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, ( result ) ? SDL_LOG_PRIORITY_WARN : SDL_LOG_PRIORITY_ERROR, "Error occured in shader compilation: %s", error_msg.data() );
	}

	glAttachShader( program_id, shader_id );

	return shader_id;
}
void link_program( const GLuint program_id ) {
	glLinkProgram( program_id );

	GLint result = GL_FALSE;
	glGetProgramiv( program_id, GL_LINK_STATUS, &result );

	int info_len;
	glGetProgramiv( program_id, GL_INFO_LOG_LENGTH, &info_len );

	if ( result == GL_FALSE || info_len != 0 ) {
		std::string error_msg( info_len, '\0' );
		glGetProgramInfoLog( program_id, info_len, NULL, error_msg.data() );
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, ( result ) ? SDL_LOG_PRIORITY_WARN : SDL_LOG_PRIORITY_ERROR, "Error occured in shader program linking: %s", error_msg.data() );
	}

	GLint attached_shaders = 0;
	glGetProgramiv( program_id, GL_ATTACHED_SHADERS, &attached_shaders );
	std::vector<GLuint> shaders( attached_shaders );

	glGetAttachedShaders( program_id, attached_shaders, nullptr, shaders.data() );
	for ( GLuint shader : shaders ) {
		glDeleteShader( shader );
	}
}
OGL_obj create_obj_from_mesh( const Mesh& mesh ) {
	OGL_obj gpu_obj = { 0 };

	glCreateBuffers( 1, &gpu_obj.vbo_id );
	glNamedBufferData( gpu_obj.vbo_id, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW );

	glCreateBuffers( 1, &gpu_obj.ibo_id );
	glNamedBufferData( gpu_obj.ibo_id, mesh.indices.size() * sizeof(GLuint), mesh.indices.data(), GL_STATIC_DRAW );

	gpu_obj.count = static_cast<GLsizei>( mesh.indices.size() );

	glCreateVertexArrays( 1, &gpu_obj.vao_id );
	glVertexArrayVertexBuffer( gpu_obj.vao_id, 0, gpu_obj.vbo_id, 0, sizeof(Vertex) );

	// glVertexArrayAttribFormat( gpu_obj.vao_id, attrib_index, size (float count), type: GL_FLOAT, normalized: GL_FALSE, stride bytes: offsetof(Vertex, pos) );
	// Vertex.pos
	glEnableVertexArrayAttrib( gpu_obj.vao_id, 0 );
	glVertexArrayAttribBinding( gpu_obj.vao_id, 0, 0 );
	glVertexArrayAttribFormat( gpu_obj.vao_id, 0, 3, GL_FALSE, GL_FALSE, offsetof(Vertex, pos) );

	// Vertex.normal
	glEnableVertexArrayAttrib( gpu_obj.vao_id, 1 );
	glVertexArrayAttribBinding( gpu_obj.vao_id, 1, 0 );
	glVertexArrayAttribFormat( gpu_obj.vao_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal) );

	// Vertex.uv
	glEnableVertexArrayAttrib( gpu_obj.vao_id, 2 );
	glVertexArrayAttribBinding( gpu_obj.vao_id, 2, 0 );
	glVertexArrayAttribFormat( gpu_obj.vao_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv) );


	glVertexArrayElementBuffer( gpu_obj.vao_id, gpu_obj.ibo_id );

	return gpu_obj;
}
void clean_OGL_obj( OGL_obj& ogl_obj ) {
	glDeleteBuffers( 1, &ogl_obj.ibo_id );
	glDeleteBuffers( 1, &ogl_obj.vbo_id );
	glDeleteBuffers( 1, &ogl_obj.vao_id );
	ogl_obj.ibo_id = 0;
	ogl_obj.vbo_id = 0;
	ogl_obj.vao_id = 0;
}
