#pragma once

#include <glm/glm.hpp>
#include <stdexcept>

#include "util/U_ogl.h"

struct Bezier_segment {
	Bezier_segment( glm::vec2 start_handle, glm::vec2 end_handle, glm::vec2 end_point ) {
		this->start_handle = start_handle;
		this->end_handle = end_handle;
		this->end_point = end_point;
	}

	glm::vec2 start_handle; // start: previous segment end
	glm::vec2 end_handle;
	glm::vec2 end_point;
};

class Road
{
public:
	Road();
	~Road();

	glm::vec2 sample_bezier_segment( const int segment_id, const float t );
	glm::vec2 sample_bezier_auto_segment( const float t );
	std::vector<Marker> get_road_markers();

	void set_position( const int segment_idx, const int point_inside_segment, const glm::vec2 new_position );
	void add_segment( Bezier_segment new_segment );
	void remove_segment( const int& segment_idx );

	int segment_count() { return ( int )bezier_segments.size(); };
protected:

	// Point (P), Handle (H) structure:
	// (Start)->P ( H H P ) * ( n + 1 )
	glm::vec2 start_point = glm::vec2( 0.f, 0.f );
	std::vector<Bezier_segment> bezier_segments;
};