#include "Road.h"

Road::Road() {

}

Road::~Road() {

}

glm::vec2 Road::sample_bezier_auto_segment( const float t ) {
	if ( t < 0 ) {
		throw std::exception( "Negative t value: %d", t );
	}

	int segment_id = ( int )floor( t );
	float remainder = t - segment_id;

	if ( segment_id >= segment_count() ) {
		segment_id = segment_count();
		remainder = 0.f;
	}

	return sample_bezier_segment( segment_id, remainder );
}

glm::vec2 Road::sample_bezier_segment( const int segment_id, const float t ) {
	if ( segment_id == segment_count() && t < 0.001f ) { return bezier_segments[ segment_id - 1 ].end_point; }
	if ( segment_id > segment_count() || segment_id < 0 ) { throw std::invalid_argument( "segment_id out of range" ); }

	glm::vec2 segment_start_point( 0.f, 0.f );
	if ( segment_id > 0 ) {
		segment_start_point = bezier_segments[ segment_id - 1 ].end_point;
	} else {
		segment_start_point = start_point;
	}

	Bezier_segment current_segment = bezier_segments[ segment_id ];
	return bezier( segment_start_point, current_segment.start_handle, current_segment.end_handle, current_segment.end_point, t );
}

glm::vec2 Road::sample_bezier_derivative_auto( const int& derivative, const float& t ) {
	if ( derivative < 0 || derivative > 2 ) {
		throw new std::exception( "Unexpected derivative id: %d", derivative );
	}
	else if ( t < 0 ) {
		throw std::exception( "Negative t value: %d", t );
	} else if ( derivative == 0 ) {
		return sample_bezier_auto_segment( t );
	}

	int segment_id = ( int )floor( t );
	float remainder = t - segment_id;

	if ( segment_id >= segment_count() ) {
		segment_id = segment_count() - 1;
		remainder = 1.f;
	}

	Bezier_segment segment = bezier_segments[ segment_id ];
	glm::vec2 a = start_point;
	if ( segment_id != 0 ) {
		a = bezier_segments[ segment_id - 1 ].end_point;
	}

	if ( derivative == 1 ) {
		return bezier_1der( a, segment.start_handle, segment.end_handle, segment.end_point, remainder );
	} else {
		return bezier_2der( a, segment.start_handle, segment.end_handle, segment.end_point, remainder );
	}
}

std::vector<Marker> Road::get_road_markers() {
	std::vector<Marker> return_markers = {};

	Marker start_m( &start_point );
	start_m.set_id( fill_marker_id( 0, 0 ) );
	return_markers.push_back( start_m );

	for ( int i = 0; i < bezier_segments.size(); ++i ) {
		Marker start_handle_m( &bezier_segments[ i ].start_handle );
		start_handle_m.set_id( fill_marker_id( i, 1 ) );
		return_markers.push_back(start_handle_m);

		Marker end_handle_m( &bezier_segments[ i ].end_handle );
		end_handle_m.set_id( fill_marker_id( i, 2 ) );
		return_markers.push_back(end_handle_m);

		Marker end_point_m( &bezier_segments[ i ].end_point );
		end_point_m.set_id( fill_marker_id( i, 3 ) );
		return_markers.push_back(end_point_m);
	}

	return return_markers;
}

void Road::add_segment( Bezier_segment new_segment ) {
	if ( bezier_segments.size() < 64 ) {
		bezier_segments.push_back( new_segment );
	} else {
		throw std::exception( "Road has reached maximum segment count." );
	}
}

void Road::set_position( const int segment_idx, const int point_inside_segment, const glm::vec2 new_pos ) {
	if ( segment_idx == -1 ) {
		start_point = new_pos;
	}
}

void Road::remove_segment( const int& segment_idx ) {
	if ( segment_idx < 1 || segment_idx >= segment_count() ) {
		return;
	}

	bezier_segments.erase( bezier_segments.begin() + segment_idx ); 
}

Mesh Road::generate_road_mesh( const float& d_t ) {
	Mesh ret_mesh;

	if ( tmp_left + tmp_right < 0.01f ) {
		return ret_mesh;
	}

	std::vector< Vertex > vertices = {};
	std::vector< GLuint > indices = {};

	std::vector< glm::vec2 > left_points = {};
	std::vector< glm::vec2 > counter_clockwise_points = {};

	const float t_max = ( float )segment_count();
	int debug_i = 0;

	Vertex left_out;

	for ( float t = 0.f; t < t_max; t += d_t ) {
		glm::vec2 p = sample_bezier_auto_segment( t );
		glm::vec2 der1 = sample_bezier_derivative_auto( 1, t );
		glm::vec2 right = rot_right( glm::normalize( der1 ) );

		glm::vec2 left_pos = p - s_mult( tmp_left, right );
		glm::vec2 right_pos = p + s_mult( tmp_right, right );

		left_points.push_back( left_pos );
		counter_clockwise_points.push_back( right_pos );
	}

	{
		glm::vec2 p = sample_bezier_auto_segment( t_max );
		glm::vec2 der1 = sample_bezier_derivative_auto( 1, t_max );
		glm::vec2 right = rot_right( glm::normalize( der1 ) );

		glm::vec2 left_pos = p - s_mult( tmp_left, right );
		glm::vec2 right_pos = p + s_mult( tmp_right, right );

		left_points.push_back( left_pos );
		counter_clockwise_points.push_back( right_pos );
	}

	for ( int i = left_points.size() - 1; i >= 0; --i ) {
		counter_clockwise_points.push_back( left_points[ i ] );
	}

	for ( glm::vec2 point : counter_clockwise_points ) {
		Vertex v;
		v.pos = glm::vec3( point.x, 0.f, point.y );
		v.normal = glm::vec3( 0.f, 1.f, 0.f );
		v.uv = glm::vec2( 1.f, 0.f );

		vertices.push_back( v );
	}

	try {
		indices = triangulate_counterclockwise( counter_clockwise_points );
	} catch ( std::exception ) {
		// ignored
	}

	ret_mesh.vertices = vertices;
	ret_mesh.indices = indices;

	return ret_mesh;
}