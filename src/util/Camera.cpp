#include "Camera.h"

#include <iostream>
#include "../App.h"

Camera::Camera( App *app ) {
	this->app = app;

	set_view( glm::vec3( 0.f, 0.f, 0.f ), glm::vec3( 0.f, 1.f, 0.f ), glm::vec3( 0.f, 0.f, -1.f ) );
	
	fov_y = glm::radians( 90.f );
	aspect = 1.0f;
	z_near = 0.01f;
	z_far = 1000.0f;

	set_proj( fov_y, aspect, z_near, z_far );
}

Camera::~Camera() {
	set_proj( this->fov_y, this->aspect, this->z_near, this->z_far );
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

void Camera::update( float dt ) {
	glm::vec3 new_look_dir( cosf( u ) * sinf( v ), cosf( v ), sinf( u ) * sinf( v ) );
	glm::vec3 new_pos = look_at - dist * new_look_dir;

	glm::vec3 right = glm::normalize( glm::cross( new_look_dir, world_up ) );
	glm::vec3 forward = glm::cross( world_up, right );

	glm::vec3 d_pos = ( forward * go_fwd + right * go_right + world_up * go_up ) * speed * dt * dist;
	new_pos += d_pos;
	new_pos.y = glm::max( 0.2f, new_pos.y );

	glm::vec3 new_look_at = look_at + d_pos;
	new_look_at.y = glm::max( 0.2f, new_look_at.y );
	
	set_view( new_pos, world_up, new_look_at );

	topdown = v >= PI - 0.1f;
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

	case SDLK_TAB:
		if ( event.repeat ) { break; }

		if ( topdown ) {
			v = 2.f;
		} else {
			v = topdown_treshold;
		}
		break;

	case SDLK_LSHIFT:
		if ( event.repeat ) { break; }
		u = glm::radians( -90.f );
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
	if ( event.state & SDL_BUTTON_MMASK || event.state & SDL_BUTTON_RMASK ) {
		u += event.xrel / sens;
		v += event.yrel / sens;

		if ( v < padding ) {
			v = padding;
		} else if ( v > PI - padding ) {
			v = PI - padding;
		}
	}
}

void Camera::mouse_scroll( const SDL_MouseWheelEvent& event ) {
	if ( dist > 0.6f && dist < 219.9f ) {
		glm::vec3 before_zoom_position;
		glm::vec3 after_zoom_position;

		glm::vec2 mouse_pos = app->get_mouse_pos();
		glm::vec2 window_size = app->get_window_size();

		ray_hit_ground_plane( get_ray_through_pixel( mouse_pos, window_size ), before_zoom_position );

		dist *= pow( .9f, event.y );
		glm::vec3 new_look_dir( cosf( u ) * sinf( v ), cosf( v ), sinf( u ) * sinf( v ) );
		glm::vec3 new_pos = look_at - dist * new_look_dir;
		new_pos.y = glm::max( 0.2f, new_pos.y );
		set_view( new_pos, world_up, look_at );

		ray_hit_ground_plane( get_ray_through_pixel( mouse_pos, window_size ), after_zoom_position );

		glm::vec3 diff = ( before_zoom_position - after_zoom_position );
		set_view( new_pos + diff, world_up, look_at + diff );
	} else {
		dist *= pow( .9f, event.y );
	}
	
	if ( dist > 220 ) {
		dist = 220.f;
	} else if ( dist < 0.5f ) {
		dist = 0.5f;
	}
}

Ray Camera::get_ray_through_pixel( const glm::vec2 mouse_pos, const glm::vec2 window_size ) {
	glm::vec3 mouse_ndc( 
		2.f * ( mouse_pos.x + .5f ) / window_size.x - 1.f,
		1.f - 2.f * ( mouse_pos.y + .5f ) / window_size.y,
		0.f
	);

	glm::vec4 world_pick = glm::inverse( get_view_proj_mtx() ) * glm::vec4( mouse_ndc, 1.f );
	world_pick /= world_pick.w;
	Ray ray;

	ray.origin = pos;
	ray.direction = glm::normalize( glm::vec3( world_pick ) - ray.origin );
	return ray;
}