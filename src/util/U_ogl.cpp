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

void clean_ogl_obj( OGL_obj& ogl_obj ) {
	glDeleteBuffers( 1, &ogl_obj.ibo_id );
	glDeleteBuffers( 1, &ogl_obj.vbo_id );
	glDeleteBuffers( 1, &ogl_obj.vao_id );
	ogl_obj.ibo_id = 0;
	ogl_obj.vbo_id = 0;
	ogl_obj.vao_id = 0;
}

GLint uniform_location(const GLchar* uniform_name) {
	GLint prog;
	glGetIntegerv( GL_CURRENT_PROGRAM, &prog );
	if ( prog == 0 ) {
		glDebugMessageInsert( GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH, -1, "Trying to get uniform location, program was 0." );
		return -1;
	}
	return glGetUniformLocation( prog, uniform_name );
}

GLsizei mip_level_count( const Img& img ) {
	GLsizei target_lvl = 1;
	unsigned int idx = std::max( img.width, img.height );
	while ( idx >>= 1 ) {
		++target_lvl;
	}
	return target_lvl;
}

Img load_img_from_file( const std::filesystem::path& file_name ) {
	Img img;

	std::unique_ptr<SDL_Surface, decltype( &SDL_DestroySurface )> loaded_img( IMG_Load( file_name.string().c_str() ), SDL_DestroySurface );
	if ( !loaded_img ) {
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, "Error occured when loading file: %s", file_name.string().c_str() );
		return img;
	}

	SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA8888;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	format = SDL_PIXELFORMAT_ABGR8888;
#endif

	std::unique_ptr<SDL_Surface, decltype( &SDL_DestroySurface )> formatted_surface( SDL_ConvertSurface( loaded_img.get(), format ), SDL_DestroySurface );
	if ( !formatted_surface ) {
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR, "Error occured when formatting image" );
		return img;
	}

	img.assign( reinterpret_cast<glm::uint32*>( formatted_surface->pixels ), formatted_surface->w, formatted_surface->h );

	return img;
}

char orientation_char_from_camera_u( const float camera_u ) {
	int i = glm::round( glm::degrees( camera_u ) / 90.f ) + 2.f;

	switch ( i ) {
	case 0:
	case 4: return 'W';
	case 1: return 'N';
	case 2: return 'E';
	case 3: return 'S';
	}
}

bool ray_hit_plane( const Ray ray, const glm::vec3 plane_point, const glm::vec3 plane_u, const glm::vec3 plane_v, Ray_intersection& intersection ) {
	glm::mat3 a( -ray.direction, plane_u, plane_v );
	glm::vec3 b( ray.origin - plane_point );

	if ( fabsf( glm::determinant( a ) ) < 0.001f ) { return false; }
	glm::vec3 x = glm::inverse( a ) * b;

	intersection.t = x.x;
	intersection.uv.x = x.y;
	intersection.uv.y = x.z;

	return x.x >= 0;
}

bool ray_hit_ground_plane( const Ray ray, glm::vec2& position ) {
	glm::vec3 plane_point( 0.f, 0.f, 0.f );
	glm::vec3 plane_u( 1.f, 0.f, 0.f );
	glm::vec3 plane_v( 0.f, 0.f, 1.f );

	Ray_intersection intersection;

	bool result = ray_hit_plane( ray, plane_point, plane_u, plane_v, intersection );

	glm::vec3 intersect_pos = plane_point + intersection.uv.x * plane_u + intersection.uv.y * plane_v;
	position.x = intersect_pos.x;
	position.y = intersect_pos.z;

	if ( result ) {
		return true;
	} else {
		return false;
	}
}

#pragma region modifier functions
constexpr Modifier operator&( Modifier a, Modifier b ){
	return static_cast<Modifier>( static_cast<unsigned char>( a ) & static_cast<unsigned char>( b ) );
}

constexpr Modifier operator^( Modifier a, Modifier b ){
	return static_cast<Modifier>( static_cast<unsigned char>( a ) ^ static_cast<unsigned char>( b ) );
}

constexpr Modifier operator|( Modifier a, Modifier b ){
	return static_cast<Modifier>( static_cast<unsigned char>( a ) | static_cast<unsigned char>( b ) );
}

constexpr Modifier operator~( Modifier a ){
	return static_cast<Modifier>( ~static_cast<unsigned char>( a ) );
}

bool key_mod_used( Modifier current_mod, Modifier check ) {
	return static_cast<unsigned char>( current_mod & check ) != 0;
}

void key_mod_set( Modifier& current_mod, Modifier to_set ) {
	current_mod = current_mod | to_set;
}

void key_mod_remove( Modifier& current_mod, Modifier to_remove ) {
	current_mod = current_mod & ~ to_remove;
}

void key_mod_toggle( Modifier& current_mod, Modifier to_remove ) {
	current_mod = current_mod ^ to_remove;
}

void tmp_print_key_mod( Modifier current_mod ) {
	switch ( current_mod ) {
	case Modifier::none:
		std::cout << "[ MOD ] None" << std::endl;
		break;
	case Modifier::alt:
		std::cout << "[ MOD ] Alt" << std::endl;
		break;
	case Modifier::ctrl:
		std::cout << "[ MOD ] Ctrl" << std::endl;
		break;
	case Modifier::shift:
		std::cout << "[ MOD ] Shift" << std::endl;
		break;
	case Modifier::ctrl_alt:
		std::cout << "[ MOD ] Ctrl+Alt" << std::endl;
		break;
	case Modifier::ctrl_shift:
		std::cout << "[ MOD ] Ctrl+Shift" << std::endl;
		break;
	case Modifier::alt_shift:
		std::cout << "[ MOD ] Alt+Shift" << std::endl;
		break;
	case Modifier::ctrl_alt_shift:
		std::cout << "[ MOD ] Ctrl+Alt+Shift" << std::endl;
		break;
	default:
		std::cout << "b";
		for ( int i = 7; i >= 0; --i ) {
			if ( ( static_cast<unsigned char>( current_mod ) & ( 1 << i ) ) != 0 ) {
				std::cout << "1";
			} else {
				std::cout << "0";
			}
		}
		std::cout << std::endl;
	}
}
#pragma endregion