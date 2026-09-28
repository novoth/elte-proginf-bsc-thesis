#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <SDL3/SDL_events.h>

class Camera
{
public:
	Camera();
	~Camera();

	inline glm::vec3 get_pos() const { return pos; }
	inline glm::vec3 get_world_up() const { return world_up; }
	inline glm::vec3 get_look_at() const { return look_at; }
	inline glm::mat4 get_view_mtx() const { return view_mtx; }
	inline glm::mat4 get_proj_mtx() const { return proj_mtx; }
	inline glm::mat4 get_view_proj_mtx() const { return proj_mtx * view_mtx; }
	inline float get_z_near() const { return z_near; }
	inline float get_z_far() const { return z_far; }
	inline float get_fov_y() const { return fov_y; }
	inline float get_aspect() const { return aspect; }
	inline float get_sens() const { return sens; }

	void set_view( glm::vec3 pos, glm::vec3 world_up, glm::vec3 look_at );
	void set_fov_y( float fov_y );
	void set_aspect( float aspect );
	void set_z_near( float z_near );
	void set_z_far( float z_far );
	void set_proj( float fov_y, float aspect, float z_near, float z_far );
	void set_sens( float sens );
	

	void update( float dt );

	inline float get_speed() const { return speed; }

	inline void set_speed( float speed ) { this->speed = speed; }

	void keyboard_down( const SDL_KeyboardEvent& event );
	void keyboard_up( const SDL_KeyboardEvent& event );
	void mouse_move( const SDL_MouseMotionEvent& event );
	void mouse_scroll( const SDL_MouseWheelEvent& event );
private:
	glm::vec3 pos;
	glm::vec3 world_up;
	glm::vec3 look_at;

	float z_near = 0.01f;
	float z_far = 1000.0f;

	float fov_y = glm::radians( 27.0f );
	float aspect = 1.0f;

	glm::mat4 view_mtx;
	glm::mat4 proj_mtx;


	float u = 0.f;
	float v = 0.f;
	float dist = 0.f;
	float speed = 10.f;

	float go_fwd = 0.f;
	float go_up = 0.f;
	float go_right = 0.f;


	float sens = 100.f;
};