#include "Camera.h"

#include <iostream>

Camera::Camera() {
	set_view( glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec3( 0.f, 0.f, -1.f ) );
}

Camera::~Camera() {

}

void Camera::set_view( glm::vec3 pos, glm::vec3 world_up, glm::vec3 look_at ) {
	this->pos = pos;
	this->world_up = world_up;
	this->look_at = look_at;

	view_mtx = glm::lookAt( pos, look_at, world_up );


	glm::vec3 look_dir = look_at - pos;
	dist = glm::length( look_dir );

	u = atan2f( look_dir.z, look_dir.x );
	v = acosf( look_dir.y / dist );
}

void Camera::set_fov_y( float fov_y ) {
	set_proj( fov_y, this->aspect, this->z_near, this->z_far );
}

void Camera::set_aspect( float aspect ) {
	set_proj( this->fov_y, aspect, this->z_near, this->z_far );
}

void Camera::set_z_near( float z_near ) {
	set_proj( this->fov_y, this->aspect, z_near, this->z_far );
}

void Camera::set_z_far( float z_far ) {
	set_proj( this->fov_y, this->aspect, this->z_near, z_far );
}

void Camera::set_proj( float fov_y, float aspect, float z_near, float z_far ) {
	this->fov_y = fov_y;
	this->aspect = aspect;
	this->z_near = z_near;
	this->z_far = z_far;

	proj_mtx = glm::perspective( fov_y, aspect, z_near, z_far );
}

void Camera::set_sens( float sens ) {
	this->sens = sens;
}

void Camera::update( float dt ) {
	glm::vec3 new_look_dir( cosf( u ) * sinf( v ), cosf( v ), sinf( u ) * sinf( v ) );
	glm::vec3 new_pos = look_at - dist * new_look_dir;

	glm::vec3 right = glm::normalize( glm::cross( new_look_dir, world_up ) );
	glm::vec3 forward = glm::cross( world_up, right );

	glm::vec3 d_pos = ( forward * go_fwd + right * go_right + world_up * go_up ) * speed * dt;
	new_pos += d_pos;
	
	set_view( new_pos, world_up, look_at + d_pos );
}

void Camera::keyboard_down( const SDL_KeyboardEvent& event ) {
	switch ( event.key ) {
	case SDLK_W:
		go_fwd = 1.f;
		break;
	case SDLK_S:
		go_fwd = -1.f;
		break;

	case SDLK_D:
		go_right = 1.f;
		break;
	case SDLK_A:
		go_right = -1.f;
		break;

	case SDLK_E:
		go_up = 1.f;
		break;
	case SDLK_Q:
		go_up = -1.f;
		break;
	}
}

void Camera::keyboard_up( const SDL_KeyboardEvent& event ) {
	switch ( event.key ) {
	case SDLK_W:
	case SDLK_S:
		go_fwd = 0.f;
		break;

	case SDLK_D:
	case SDLK_A:
		go_right = 0.f;
		break;

	case SDLK_E:
	case SDLK_Q:
		go_up = 0.f;
		break;
	}
}

void Camera::mouse_move( const SDL_MouseMotionEvent& event ) {
	if ( event.state & SDL_BUTTON_LMASK ) {
		u += event.xrel / sens;
		v += event.yrel / sens;
	} else if ( event.state & SDL_BUTTON_RMASK ) {
		dist *= pow( .9f, event.yrel / sens );
	}
}

void Camera::mouse_scroll( const SDL_MouseWheelEvent& event ) {
	dist *= pow( .9f, event.y / sens );
}
