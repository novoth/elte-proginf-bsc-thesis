#include "Road.h"

Road::Road() {

}

Road::~Road() {

}

glm::vec2 Road::sample_bezier_auto_segment( const float t ) {
	int segment_id = ( int )floor( t );
	float remainder = t - segment_id;
	if ( segment_id < 0 ) { 
		segment_id = 0;
		remainder = 0.f;
	}
	else if ( segment_id >= segment_count() ) {
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
	return sample_bezier( segment_start_point, current_segment.start_handle, current_segment.end_handle, current_segment.end_point, t );
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