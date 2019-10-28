#include "framework.h"

#define SCREEN_W 1920
#define SCREEN_H 1080
#define SCREEN_HW (SCREEN_W/2)
#define SCREEN_HH (SCREEN_H/2)

#include "profiling_timer.h"


#include <vector>


using std::vector;



#include <AllegroFlare/BitmapBin.hpp>
using AllegroFlare::BitmapBin;
BitmapBin bitmaps;


#include <AllegroFlare/Timer.hpp>
#include <AllegroFlare/TimerFormatter.hpp>
using AllegroFlare::TimerFormatter;
AllegroFlare::Timer stopwatch;



bool logo_showing = true;
float logo_scale = 0.0;


float delay_time_since_last_affect = 1.0;



// restart music at beginning of thing

// longer music

// remove extra time at end of music

// exit gate sound




ALLEGRO_SAMPLE_INSTANCE *instance_hit_bad = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_bounce = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_med = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_soft = nullptr;



void play_hit_bad()
{
    al_stop_sample_instance(instance_hit_bad);
    al_play_sample_instance(instance_hit_bad);
}

void play_hit_bounce()
{
    al_stop_sample_instance(instance_hit_bounce);
    al_play_sample_instance(instance_hit_bounce);
}

void play_hit_med()
{
    al_stop_sample_instance(instance_hit_med);
    al_play_sample_instance(instance_hit_med);
}

void play_hit_soft()
{
    al_stop_sample_instance(instance_hit_soft);
    al_play_sample_instance(instance_hit_soft);
}



ALLEGRO_SAMPLE_INSTANCE *engine_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *music_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance = nullptr;
ALLEGRO_SAMPLE *music_sample = nullptr;

ALLEGRO_VOICE *engine_voice = nullptr;
ALLEGRO_MIXER *engine_mixer = nullptr;
bool FLAG_draw_profile_graph = false;

ALLEGRO_COLOR start_text_color;


int max_num_lives = 6;
int num_lives = max_num_lives;


bool game_won = false;
bool game_over = false;

bool final_course = false;


float lap_notification_counter = 0.0;
float track_begin_notification_counter = 0.0;


void restart_music()
{
	if (!music_sample) return;
	al_stop_sample_instance(music_sample_instance);
	al_play_sample_instance(music_sample_instance);
}



//int segment_where_player_died = 0;

vector<int> segment_where_player_died;



void start_track_begin_text()
{
	animate(track_begin_notification_counter, 1.0, 0.0, 4.0, interpolator::trippleFastOut);
}


int num_of_segments_in_track = 4;



double OMG_DeltaTime = 1.0;

float finish_track_animation_counter = 1.0;
bool track_completed = false;

int num_laps_to_win = 3;

float foreground_black_opacity = 1.0;
float foreground_white_opacity = 0.0;


int index_of_last_track_segment_that_collides = 0;




void fade_to_black()
{
	animate(foreground_black_opacity, 0.0, 1.0, 3.0);
}

void fade_out_of_black()
{
	animate(foreground_black_opacity, 1.0, 0.0, 3.6);
}

void flash_white()
{
	animate(foreground_white_opacity, 1.0, 0.0, 2.0);
	al_stop_sample_instance(exit_sample_instance);
	al_play_sample_instance(exit_sample_instance);
}



class CheapCamera
{
public:
	float depth;
	float z;
	float x, y, rotation;
	float zoom;
	ALLEGRO_TRANSFORM transform;

	CheapCamera()
	{
		depth = 512;
		z = 256;
		rotation = 0.0;
		zoom = 1.0;
	}

	void transform_on()
	{
		al_identity_transform(&transform);
		al_translate_transform(&transform, -x, -y);
		al_scale_transform(&transform, zoom, zoom);
		//al_rotate_transform(&transform, rotation+FULL_ROTATION/2);
		al_translate_transform(&transform, 400, 300);
		//al_build_transform(&transform, x, y, 1.0, 1.0, rotation);
		al_use_transform(&transform);
	}
};

CheapCamera *camera = nullptr;


#include "good_camera.h"


camera_class *good_camera = nullptr;


void zoom_way_out()
{
	animate_to(camera->zoom, 0.05);
}

void zoom_normal()
{
	animate_to(camera->zoom, 1.0);
}

vec2d rotate_point(vec2d point, float angle)
{
	return vec2d(point.x*cos(angle) - point.y*sin(angle), point.x*sin(angle) + point.y*cos(angle));
}



#define LEFT_RAIL 1
#define RIGHT_RAIL 2

class SegmentInfo
{
public:
	vec2d start;
	vec2d end;
	vec2d from_start;
	vec2d perpendicular;
	vec2d normal;
	vec2d middle;
	float radius;
	float length;

	SegmentInfo() {}
	SegmentInfo(vec2d &start, vec2d &end)
	{
		this->start = start;
		this->end = end;
		from_start = end - start;
		middle = from_start/2 + start;
		length = distance(start, end);
		radius = length/2;
		perpendicular = vec2d(-from_start.y, from_start.x);
		normal = ~-perpendicular;
	}
};




class particle_effect;

void mark_particle_unused(particle_effect *p);

class particle_effect
{
public:
	static vector<particle_effect *> particle;
public:
	static int last_particle_index;
	particle_effect() : image(nullptr), in_use(false) {}
	static void reserve_particles(int num)
	{
		for (int i=0; i<(int)particle.size(); i++) delete particle[i];
		particle.clear();

		for (int i=0; i<num; i++)
			particle.push_back(new particle_effect());
	}
	static particle_effect *get_one()
	{
		return particle[++last_particle_index % particle.size()];
	}
	static void update_all()
	{
		for (int i=0; i<(int)particle.size(); i++) particle[i]->update();
	}

	bool update()
	{
		//if ((start_time + duration) > al_get_time()) return (in_use = false);
		position += velocity;
		return true;
	}
	float start_time;
	float duration;
	vec2d position;
	vec2d velocity;

	float position_z;
	float opacity;
	ALLEGRO_BITMAP *image;
	bool in_use;

	vec2d projected_position; // < ignore... it's for use the outside world
};
vector<particle_effect *> particle_effect::particle;
int particle_effect::last_particle_index = 0;

void mark_particle_unused(particle_effect *p)
{
	p->in_use = false;
}





void create_particle_spread(float x, float y)
{
	return;

	particle_effect *particle = nullptr;
	int num_balls_in_spread = 20;
	for (int i=0; i<num_balls_in_spread; i++)
	{
		particle = particle_effect::get_one();
		if (!particle) return;
		particle->position = vec2d(x, y);
		particle->position_z = 0;
		particle->opacity = 1.0;
		particle->start_time = al_get_time();
		particle->duration = 10.0;
		particle->in_use = true;
	// potential crash right here
		//if () particle->position
		particle->image = bitmaps.auto_get("yellow_ball.png");

		float rotation = FULL_ROTATION/num_balls_in_spread * i;

		particle->velocity = vec2d(sin(rotation), cos(rotation)) * 10;
		particle->opacity = 0.5;

	}

		//std::cout << "PARTICLE!" << std::endl;
}






enum { COLOR_TYPE_WHITE = 0,
	COLOR_TYPE_OFF,
	COLOR_TYPE_YELLOW,
	COLOR_TYPE_GREEN,
	COLOR_TYPE_BLUE,
	COLOR_TYPE_RED,
	COLOR_TYPE_RED_DEATH,
	COLOR_TYPE_MAX };


class TrackSegment
{
public:
	float w, h;

	float rotation;
	//vec2d displacement;

	std::vector<vec2d *> left_rail;
	std::vector<SegmentInfo *> left_rail_segment;
	float left_rail_length;

	std::vector<vec2d *> right_rail;
	std::vector<SegmentInfo *> right_rail_segment;
	float right_rail_length;


	vec2d *entrance_p1;
	vec2d *entrance_p2;
	SegmentInfo *entrance_segment_info;


	int color_type;

	int num_points()
	{
		return right_rail.size() + left_rail.size();
	}

	TrackSegment() : color_type(COLOR_TYPE_YELLOW), entrance_p1(nullptr), entrance_p2(nullptr), entrance_segment_info(nullptr)
    {}

	~TrackSegment()
	{
		for (int i=0; i<(int)left_rail.size(); i++) delete left_rail[i];
		for (int i=0; i<(int)right_rail.size(); i++) delete right_rail[i];
		left_rail.clear();
		right_rail.clear();
		update_slope_info();
	}

	void add_point(int wall_num, float x, float y)
	{
		if (wall_num==LEFT_RAIL) left_rail.push_back(new vec2d(x, y));
		if (wall_num==RIGHT_RAIL) right_rail.push_back(new vec2d(x, y));
		//update_slope_info();
	}

	void move(float x, float y)
	{
		for (int i=0; i<(int)left_rail.size(); i++)
		{
			left_rail[i]->x += x;
			left_rail[i]->y += y;
		}
		for (int i=0; i<(int)right_rail.size(); i++)
		{
			right_rail[i]->x += x;
			right_rail[i]->y += y;
		}
		update_slope_info();
	}

	void rotate(float angle)
	{
		for (int i=0; i<(int)left_rail.size(); i++)
		{
			*left_rail[i] = rotate_point(*left_rail[i], angle);
		}
		for (int i=0; i<(int)right_rail.size(); i++)
		{
			*right_rail[i] = rotate_point(*right_rail[i], angle);
		}
		update_slope_info();
	}

	void draw_projected();

	void reverse()
	{
		// doesn't work (yet)
		update_slope_info();
	}

	void draw()
	{
		int line_thickness = 3;

		for (int i=1; i<(int)left_rail.size(); i++)
			al_draw_line(left_rail[i-1]->x, left_rail[i-1]->y, left_rail[i]->x, left_rail[i]->y, al_color_name("orange"), 2.0*line_thickness);

		for (int i=1; i<(int)left_rail.size(); i++)
			al_draw_filled_circle(left_rail[i-1]->x, left_rail[i-1]->y, 2.0*line_thickness/2, al_color_name("orange"));

		for (int i=1; i<(int)right_rail.size(); i++)
			al_draw_line(right_rail[i-1]->x, right_rail[i-1]->y, right_rail[i]->x, right_rail[i]->y, al_color_name("orange"), 2.0*line_thickness);

		for (int i=1; i<(int)right_rail.size(); i++)
			al_draw_filled_circle(right_rail[i-1]->x, right_rail[i-1]->y, 2.0*line_thickness/2, al_color_name("orange"));

		return;

		al_draw_circle(left_rail.front()->x, left_rail.front()->y, 12, al_color_name("green"), 3.0);
		al_draw_circle(left_rail.back()->x, left_rail.back()->y, 6, al_color_name("red"), 3.0);
	}

	void __HACK_reverse_order_of_points()
	{
		std::reverse(left_rail.begin(), left_rail.end());
		std::reverse(right_rail.begin(), right_rail.end());
		std::reverse(left_rail_segment.begin(), left_rail_segment.end());
		std::reverse(right_rail_segment.begin(), right_rail_segment.end());
		update_slope_info();
	}

	void soften_left_rail()
	{
		if (left_rail.size() <= 2) return;

		std::vector<vec2d *> *newpoints = new std::vector<vec2d *>;
		
		// break each segment into 4 seperate segments
		for (int i=1; i<(int)left_rail.size(); i++)
		{
			newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.0 + *left_rail[i-1]));
			newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.25 + *left_rail[i-1]));
			newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.5 + *left_rail[i-1]));
			newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.75 + *left_rail[i-1]));
		}
		newpoints->push_back(new vec2d(*left_rail[left_rail.size()-1]));

		
		// skip the first 2
		// erase every other point
		bool keep = true;
		for (int i=1; i<(int)newpoints->size(); i++)
		{
			if (!keep)
			{
				delete newpoints->at(i);
				newpoints->erase(newpoints->begin()+i);
				i--;
			}
			keep = !keep;
		}
		newpoints->push_back(new vec2d(*left_rail[left_rail.size()-1]));

		delete newpoints->at(1);
		newpoints->erase(newpoints->begin()+1);

		delete newpoints->at((int)newpoints->size()-2);
		newpoints->erase(newpoints->end()-2);


		for (int i=0; i<(int)left_rail.size(); i++) delete left_rail[i];
		left_rail.clear();
		left_rail = *newpoints;

		update_slope_info();
	}

	void soften_right_rail()
	{
		if (right_rail.size() <= 2) return;

		std::vector<vec2d *> *newpoints = new std::vector<vec2d *>;
		
		// break each segment into 4 seperate segments
		for (int i=1; i<(int)right_rail.size(); i++)
		{
			newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.0 + *right_rail[i-1]));
			newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.25 + *right_rail[i-1]));
			newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.5 + *right_rail[i-1]));
			newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.75 + *right_rail[i-1]));
		}
		newpoints->push_back(new vec2d(*right_rail[right_rail.size()-1]));

		
		// skip the first 2
		// erase every other point
		bool keep = true;
		for (int i=1; i<(int)newpoints->size(); i++)
		{
			if (!keep)
			{
				delete newpoints->at(i);
				newpoints->erase(newpoints->begin()+i);
				i--;
			}
			keep = !keep;
		}
		newpoints->push_back(new vec2d(*right_rail[right_rail.size()-1]));

		delete newpoints->at(1);
		newpoints->erase(newpoints->begin()+1);

		delete newpoints->at((int)newpoints->size()-2);
		newpoints->erase(newpoints->end()-2);


		for (int i=0; i<(int)right_rail.size(); i++) delete right_rail[i];
		right_rail.clear();
		right_rail = *newpoints;

		update_slope_info();
	}

	void update_slope_info()
	{
		// for the left //

		if (left_rail.size() <= 1) return;
		for (int i=0; i<(int)left_rail_segment.size(); i++) delete left_rail_segment[i];

		left_rail_segment.clear();
		left_rail_segment.push_back(new SegmentInfo());
		left_rail_length = 0;

		for (int i=1; i<(int)left_rail.size(); i++)
		{
			left_rail_segment.push_back(new SegmentInfo(*left_rail[i-1], *left_rail[i]));
			left_rail_length += left_rail_segment.back()->length;
		}

		// for the right //

		if (right_rail.size() <= 1) return;
		for (int i=0; i<(int)right_rail_segment.size(); i++) delete right_rail_segment[i];

		right_rail_segment.clear();
		right_rail_segment.push_back(new SegmentInfo());
		right_rail_length = 0;

		for (int i=1; i<(int)right_rail.size(); i++)
		{
			right_rail_segment.push_back(new SegmentInfo(*right_rail[i-1], *right_rail[i]));
			right_rail_length += right_rail_segment.back()->length;
		}

		// entrance

		//if (entrance_p1) delete entrance_p1;
		//if (entrance_p2) delete entrance_p2;

		//entrance_p1 = new vec2d(0, 1);
		//entrance_p2 = new vec2d(1, 0);
		//entrance_segment_info = new SegmentInfo(*entrance_p1, *entrance_p2);

		entrance_p1 = left_rail.back();
		entrance_p2 = right_rail.front();
		if (entrance_segment_info) delete entrance_segment_info;
		entrance_segment_info = new SegmentInfo(*entrance_p1, *entrance_p2);
	}
};



// track construction functions //


void fill_track_rail_points();
#include "track_segments.h"

class Racer;

class Track
{
public:

	static vector<int> __this_points_color__left_rail;
	static vector<int> __this_points_color__right_rail;
	static vector<int> __this_point_belongs_to_segment;

	static vector<vec2d> track_rail_light; // < for caching
	static vector<std::pair<bool, int> > rail_light_belongs_to; // < for caching
	static vector<vec2d> track_rail_light_projected; // < for caching

	static vector<std::pair<bool, int> > __left_color_light_belongs_to; // < for caching
	static vector<std::pair<bool, int> > __right_color_light_belongs_to; // < for caching

	static vector<float> car_distance_cache;

public:
	vec2d exit_p1;
	vec2d exit_p2;
	vec2d enter_p1;
	vec2d enter_p2;
	SegmentInfo *exit_segment_info;
	double start_time;

	std::vector<TrackSegment *> segment;
	ALLEGRO_VERTEX left_point[512];
	ALLEGRO_VERTEX right_point[512];


	Track() : exit_segment_info(nullptr) {}

	~Track()
	{
		for (int i=0; i<(int)segment.size(); i++) delete segment[i];
		segment.clear();
		segment_where_player_died.clear();
	}
	void clear()
	{
		for (int i=0; i<(int)segment.size(); i++)
			delete segment[i];
		segment.clear();
		segment_where_player_died.clear();
	}
	void draw()
	{
		for (int i=0; i<(int)segment.size(); i++)
			segment[i]->draw();

		al_draw_line(exit_p1.x, exit_p1.y, exit_p2.x, exit_p2.y, al_color_name("dodgerblue"), 20.0);

		al_draw_line(enter_p1.x, enter_p1.y, enter_p2.x, enter_p2.y, al_color_name("lightgreen"), 20.0);
	}

	void draw_projected(CheapCamera *cam, float racer_facing_angle, float racer_x, float racer_y, float racer_speed);

	void update_exit_slope_info()
	{
		if (exit_segment_info) delete exit_segment_info;
		exit_segment_info = new SegmentInfo(exit_p1, exit_p2);
	}

	void __HACK_finalize_track()
	{
		for (int i=0; i<(int)segment.size(); i++)
			segment[i]->__HACK_reverse_order_of_points();

		update_exit_slope_info();
		fill_track_rail_points();
	}

	void make_segments_easy_where_player_died()
	{
		for (int i=0; i<(int)segment_where_player_died.size(); i++)
		{
			segment[segment_where_player_died[i]]->color_type = COLOR_TYPE_OFF;

			//std::cout << "EASY: " << segment_where_player_died[i] << std::endl;
		}
		fill_track_rail_points();
	}

	void create_gate()
	{
		TrackSegment *gate = opening_gate();
		segment.push_back(gate);
	}

	void append_segment(TrackSegment *ts)
	{
		if (segment.empty()) { segment.push_back(ts); return; }

		vec2d pipe_output_vector = *segment.back()->right_rail.front()- *segment.back()->left_rail.back();
		float distance = pipe_output_vector.GetMagnitude();

		ts->rotate(pipe_output_vector.GetAngle());

		/*
		std::cout << "x: " << pipe_output_vector.x << std::endl;
		std::cout << "y: " << pipe_output_vector.y << std::endl;
		std::cout << "angle: " << pipe_output_vector.GetAngle() << std::endl;
		std::cout << "angle(rad): " << degrees_to_radians(pipe_output_vector.GetAngle()) << std::endl;
		std::cout << "mag: " << distance << std::endl;
		std::cout << std::endl;
		*/
		//std::cout << "angle(deg): " << radians_to_degrees(pipe_output_vector.GetAngle()) << std::endl;
		ts->move(segment.back()->left_rail.back()->x, segment.back()->left_rail.back()->y);

		segment.push_back(ts);

		//ts->__HACK_reverse_order_of_points();

		// update the entrance / exit
		exit_p1 = *segment.back()->left_rail.back();
		exit_p2 = *segment.back()->right_rail.front();
		enter_p1 = *segment.at(1)->left_rail.front(); // the edge of the gate
		enter_p2 = *segment.at(1)->right_rail.back(); // the edge of the gate
		//fill_track_rail_points();
	}
};
vector<int> Track::__this_points_color__left_rail;
vector<int> Track::__this_points_color__right_rail;
vector<int> Track::__this_point_belongs_to_segment;
vector<vec2d> Track::track_rail_light; // < for caching
vector<vec2d> Track::track_rail_light_projected; // < for caching
vector<float> Track::car_distance_cache;
vector<std::pair<bool, int> > Track::rail_light_belongs_to;
vector<std::pair<bool, int> > Track::__left_color_light_belongs_to;
vector<std::pair<bool, int> > Track::__right_color_light_belongs_to;



void complete_track();
void mark_player_not_using_boost();

class Racer : public uses_keyboard
{
public:
	vec2d position, direction, velocity;
	float velocity_magnitude;
	float direction_angle;

	std::vector<double> lap_time;

	float max_health, health;
	int max_boosts;
	int boosts;
	bool throttle_on;
	bool break_on;
	bool turning_right, turning_left;
	bool dead;

	bool using_boost;
	float using_boost_counter;

	Racer()
	{
		//num_laps = 0;
		position = vec2d(100, 100);
		direction = vec2d(1, 0);
		velocity = vec2d(0, 0);
		direction_angle = direction.GetAngle();
		direction_angle += FULL_ROTATION/2;
		velocity_magnitude = 0;
		throttle_on = false;
		turning_right = false;
		turning_left = false;
		break_on = false;
		health = 100;
		max_health = 100;
		max_boosts = 3;
		boosts = max_boosts;
		dead = false;
		using_boost = false;
		using_boost_counter = 0.0;
	}

	void complete_lap()
	{
		//num_laps++;
		//lap_time = al_get_time();
		lap_time.push_back(al_get_time());

		animate(lap_notification_counter, 1.0, 0.0, 3.0, interpolator::quadrupleFastOut);

		if ((int)lap_time.size() > num_laps_to_win) complete_track();
	}

	void draw()
	{
		//vec2d direction_vector;
		al_draw_circle(position.x, position.y, 10, al_color_name("pink"), 2.0);
		al_draw_line(position.x, position.y, position.x+velocity.x*20, position.y+velocity.y*20, al_color_name("pink"), 2.0);
		al_draw_line(position.x, position.y, position.x+direction.x*20, position.y+direction.y*20, al_color_name("yellow"), 2.0);
	}

	void use_boost()
	{
		if (boosts == 0) { /* do nothing */ }
		if (using_boost) return;

		animate(using_boost_counter, 1.0, 0.0, 2.0, interpolator::doubleFastOut).callback(mark_player_not_using_boost);
		velocity_magnitude = 4.5;

		using_boost = true;
	}

	void on_key_up()
	{
		if (dead) return;

		switch(current_event->keyboard.keycode)
		{
		case ALLEGRO_KEY_UP:
			throttle_on = false;
			break;
		case ALLEGRO_KEY_LEFT:
			turning_left = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			turning_right = false;
			break;
		case ALLEGRO_KEY_DOWN:
			break_on = false;
			break;
		}
	}
	void on_key_down()
	{
		if (dead) return;

		switch(current_event->keyboard.keycode)
		{
		case ALLEGRO_KEY_UP:
			throttle_on = true;
			break;
		case ALLEGRO_KEY_LEFT:
			turning_left = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			turning_right = true;
			break;
		case ALLEGRO_KEY_DOWN:
			break_on = true;
			break;
		case ALLEGRO_KEY_SPACE:
			//use_boost();
			break;
		}
	}
};








// globuals //

Track *track = nullptr;
TrackSegment *track_segment = nullptr;
Racer *racer = nullptr;



void mark_player_not_using_boost()
{
	racer->using_boost = false;
}


#include "projection_render.h"



std::string format_lap_time(double time)
{
	int seconds = (int)(fmod(time, 60));
	int minutes = (int)(time / 60);
	int mseconds = (int)(time * 1000) - seconds*1000 - minutes*60*1000;

	std::string output;
	
	if (minutes < 10) output += "0";
	output += tostring(minutes) + "'";

	if (seconds < 10) output += "0";
	output += tostring(seconds) + "\"";

	if (mseconds < 10) output += "0";
	if (mseconds < 100) output += "0";
	output += tostring(mseconds);

	return output;
}

// hud //

void draw_text_with_letter_spacing(int font_size, int x, int y, float letter_spacing, ALLEGRO_FONT *font, std::string string_to_write)
{
   //ALLEGRO_FONT *font = get_font("venus_rising_rg.ttf", font_size);
   ALLEGRO_COLOR color = al_color_name("white");

   std::string the_char = " ";
   int num_characters = string_to_write.length();
   int letters_traversed = 0;
   int leftmost_character_x = SCREEN_HW - (num_characters * letter_spacing) * 0.5;
   for (auto &c : string_to_write)
   {
      the_char[0] = c;
      al_draw_text(font, color, leftmost_character_x + (letters_traversed * letter_spacing), y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
}


void draw_health_bar_OLD()
{
   int health_bar_height = 10;

	std::string lives_string = "Lives: " + tostring(num_lives);
	al_draw_text(get_font("venus_rising_rg.ttf", -26), al_color_name("white"), 20, SCREEN_H-60, 0, lives_string.c_str());

	float health_percentage = racer->health/racer->max_health;
	ALLEGRO_COLOR health_bar_color;
	if (health_percentage > 0.9) health_bar_color = al_color_name("yellow");
	else if (health_percentage > 0.6) health_bar_color = al_color_name("yellow");
	else if (health_percentage > 0.3) health_bar_color = al_color_name("orange");
	else health_bar_color = al_color_name("red");


	al_draw_filled_rectangle(20, SCREEN_H-20, 220, SCREEN_H-(20 + health_bar_height), al_color_name("black"));
	al_draw_filled_rectangle(20, SCREEN_H-20, 20+200*(health_percentage), SCREEN_H-(20 + health_bar_height), health_bar_color);
}


void draw_health_bar(int text_y, int horizontal_screen_padding)
{
   int health_bar_height = 10;
   ALLEGRO_FONT *font = get_font("venus_rising_rg.ttf", -26);

	std::string lives_string = "Lives: " + tostring(num_lives);
	al_draw_text(font, al_color_name("white"), horizontal_screen_padding, text_y, 0, lives_string.c_str());

	float health_percentage = racer->health/racer->max_health;

	ALLEGRO_COLOR health_bar_color;
	if (health_percentage > 0.9) health_bar_color = al_color_name("dodgerblue");
	else if (health_percentage > 0.6) health_bar_color = al_color_name("violet");
	else if (health_percentage > 0.3) health_bar_color = al_color_name("orange");
	else health_bar_color = al_color_name("red");

   int lives_text_length = 140;
   int bar_length = 220;
   int bar_y = text_y + al_get_font_ascent(font) / 2 + 2;
   int bar_x = horizontal_screen_padding + lives_text_length + 4;

	al_draw_filled_rectangle(
         bar_x,
         bar_y - health_bar_height * 0.5,
         bar_x + bar_length,
         bar_y + health_bar_height * 0.5,
         al_color_name("darkblue"));

	al_draw_filled_rectangle(
         bar_x,
         bar_y - health_bar_height * 0.5,
         bar_x + bar_length * health_percentage,
         bar_y + health_bar_height * 0.5,
         al_color_name("lightblue"));
}


void draw_stopwatch(int text_y, int horizontal_screen_padding)
{
   std::string ellapsed_time_str = TimerFormatter(stopwatch.get_elappsed_time_msec()).format();
   ALLEGRO_FONT *font = get_font("venus_rising_rg.ttf", -26);
   ALLEGRO_COLOR color = al_color_name("white");

   //float text_width = al_get_text_width(font, ellapsed_time_str.c_str());
   std::string the_char = " ";
   int letter_spacing = 12 * 2;
   int letters_traversed = 0;
   for (auto &c : ellapsed_time_str)
   {
      the_char[0] = c;
      al_draw_text(font, color, SCREEN_W - 300 + (letters_traversed * letter_spacing), text_y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
}   


void draw_num_segments(int text_y, int num_segments)
{
   std::stringstream ss;
   ss << num_segments << " SEG";

   std::string string_to_write = ss.str();
   ALLEGRO_FONT *font = get_font("venus_rising_rg.ttf", -26);
   ALLEGRO_COLOR color = al_color_name("white");

   std::string the_char = " ";
   int letter_spacing = 12 * 2;
   int letters_traversed = 0;
   for (auto &c : string_to_write)
   {
      the_char[0] = c;
      al_draw_text(font, color, SCREEN_W - 300 - 300 + (letters_traversed * letter_spacing), text_y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
}


void draw_num_laps(int text_y, int num_laps_passed, int total_num_laps)
{
   std::stringstream ss;
   ss << num_laps_passed << " / " << total_num_laps;

   std::string string_to_write = ss.str();
   ALLEGRO_FONT *font = get_font("venus_rising_rg.ttf", -26);
   ALLEGRO_COLOR color = al_color_name("white");

   std::string the_char = " ";
   int letter_spacing = 12 * 2;
   int letters_traversed = 0;
   for (auto &c : string_to_write)
   {
      the_char[0] = c;
      al_draw_text(font, color, SCREEN_W - 300 - 300 - 200 + (letters_traversed * letter_spacing), text_y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
}


void draw_hud()
{
	ALLEGRO_TRANSFORM transform;
	al_identity_transform(&transform);
	al_use_transform(&transform);

	//int velocity = (int)(racer->velocity_magnitude*100);
	//al_draw_text(get_font("HelveticaNeue.ttf", -20), al_color_name("white"), 40, 600-70, NULL, tostring(velocity).c_str());

	//al_draw_text(get_font("HelveticaNeue.ttf", -20), al_color_name("white"), 20, 20, NULL,
	//	tostring(format_lap_time(al_get_time() - track->start_time)).c_str());

	//al_draw_text(get_font("lacuna.ttf", -19), al_color_name("white"), 20, 300, NULL, tostring(OMG_DeltaTime).c_str());

	
	double lap_so_far = 0;
	for (int i=0; i<(int)racer->lap_time.size(); i++)
	{
		//if (i!=0) lap_so_far = racer->lap_time[i-1];
		//std::string laps_string = "Lap: " + format_lap_time(racer->lap_time[i] - track->start_time - lap_so_far);
		//al_draw_text(get_font("lacuna.ttf", -20), al_color_name("white"), 40, 600-70-20*(i+1), NULL, laps_string.c_str());
	}
	
	/*

	ALLEGRO_TRANSFORM arc_transform;
	al_identity_transform(&arc_transform);
	al_scale_transform(&transform, 2.0, 0.5);
	al_use_transform(&transform);
	float arc_radius = 80;
	float EST_MAX_VELOCITY_MAGNITUDE = 6.0;
	float speed_percentage = racer->velocity_magnitude / EST_MAX_VELOCITY_MAGNITUDE;
	al_draw_arc(arc_radius+50, SCREEN_H-100+300, arc_radius, FULL_ROTATION/4*2, FULL_ROTATION/8*2, al_color_name("darkgray"), 30);
	al_draw_arc(arc_radius+50, SCREEN_H-100+300, arc_radius, FULL_ROTATION/4*2, (FULL_ROTATION/8*2)*speed_percentage, al_color_name("dodgerblue"), 26);
	*/

  int text_y = 40;
  int horizontal_screen_padding = 100;

   draw_num_segments(text_y, num_of_segments_in_track);

   draw_num_laps(text_y, (int)racer->lap_time.size(), num_laps_to_win);
   draw_stopwatch(text_y, horizontal_screen_padding);

   draw_health_bar(text_y, horizontal_screen_padding);
}


void kill_player(int _segment_where_player_died)
{
	racer->dead = true;
	animate_to(camera->zoom, 0.8, 2, interpolator::slowInOut);
	//animate_delta(racer->direction_angle, FULL_ROTATION*2, 5.0, interpolator::trippleFastIn);
	al_stop_sample_instance(engine_sample_instance);

	racer->throttle_on = false;
	racer->turning_left = false;
	racer->turning_right = false;
	racer->break_on = false;

	num_lives--;

	if (num_lives <= 0) game_over = true;

	if (_segment_where_player_died != 0) segment_where_player_died.push_back(_segment_where_player_died);

	racer->velocity_magnitude = 0;
	racer->velocity = 0;
}

// game calculation functions //

#include "update_racer_and_track.h"

/*
void update_racer_and_track(Racer *r, Track *t)
{
	if (r->throttle_on) r->velocity_magnitude += 0.1;
	if (r->turning_right) r->direction_angle -= 0.02;
	if (r->turning_left) r->direction_angle += 0.02;
	
	r->direction = vec2d(sin(r->direction_angle), cos(r->direction_angle));

	// friction
	r->velocity_magnitude *= 0.98;
	
	vec2d position_before = r->position;
	vec2d position_after = r->position + (r->direction.Normalized() * r->velocity_magnitude);

	//if (

	r->position = position_after;
}
*/



void start_track()
{
	animate(camera->zoom, 0.2, 1.0, 3.0, interpolator::doubleSlowInOut);
	racer->position = vec2d(50, -50);
	racer->direction_angle = FULL_ROTATION/2;
	racer->velocity_magnitude = 0;
	racer->lap_time.clear();
	track->start_time = al_get_time();
	al_play_sample_instance(engine_sample_instance);

	racer->health = racer->max_health;
	racer->dead = false;
	track_completed = false;

	logo_showing = false;

	index_of_last_track_segment_that_collides = 0;

	track->make_segments_easy_where_player_died();

	start_track_begin_text();
   stopwatch = AllegroFlare::Timer();
   stopwatch.start();

	fade_out_of_black();

}



void complete_track()
{
	//racer->dead = true;
	if (num_of_segments_in_track == 30) game_won = true;
	track_completed = true;
}



bool create_random_track(Track *t, int num_segments)
{
	t->clear();
	//int num_segments = 20;

	if (num_segments > 30) num_segments = 30;

	TrackSegment *track_segment = nullptr;

	track->create_gate(); // gate is added automatically

	segment_where_player_died.clear();

	track_segment = create_track_segmentE();
	track->append_segment(track_segment);
	//							track_segment = create_track_segmentE();
								//track->append_segment(track_segment);
								//track->__HACK_finalize_track();
	//return;

	track_segment = create_track_segmentE();
	track->append_segment(track_segment);

	int num_consecutive_color_segments = 2;
	int consecutive_color_segment_ct = 0;
	int last_color_segment = 0;

	for (int i=0; i<num_segments; i++)
	{
		// pick the segment
		switch(random_int(0, 4))
		{
		case 0:
			track_segment = create_track_segmentA();
			break;
		case 1:
			track_segment = create_track_segmentB();
			break;
		case 2:
			track_segment = create_track_segmentC();
			break;
		case 3:
			track_segment = create_track_segmentD();
			break;
		case 4:
			track_segment = create_track_segmentE();
			break;
		}

		// pick a reverse flip of the segment or not
		if (random_bool()) track_segment->reverse();

		if ((consecutive_color_segment_ct%num_consecutive_color_segments) == 0)
		{
			int new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_MAX-1);

			if (num_segments == 4)
			{
				new_color_segment = COLOR_TYPE_YELLOW;
			}
			else if (num_segments == 10)
			{
				new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_GREEN);
			}
			else if (num_segments == 16)
			{
				// introduce blue, and unlikely red
				new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
				//if (new_color_segment == COLOR_TYPE_RED)
				//	new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
			}
			else if (num_segments == 22)
			{
				// no yellow and unlikely green
				new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED_DEATH);
				while (new_color_segment == COLOR_TYPE_GREEN)
					new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
			}
			else if (num_segments == 28)
			{
				// no yellow or green, and more likely red
				new_color_segment = random_int(COLOR_TYPE_BLUE, COLOR_TYPE_RED_DEATH);
				if (new_color_segment != COLOR_TYPE_RED)
					new_color_segment = random_int(COLOR_TYPE_BLUE, COLOR_TYPE_RED_DEATH);
			}
			else if (num_segments == 30) new_color_segment = COLOR_TYPE_RED_DEATH;

			last_color_segment = new_color_segment;
		}

		consecutive_color_segment_ct++;

		track_segment->color_type = last_color_segment;

		//track_segment->soften_left_rail();
		//track_segment->soften_left_rail();
		//track_segment->soften_left_rail();
		//track_segment->soften_left_rail();
		//track_segment->soften_left_rail();

		//track_segment->soften_right_rail();
		//track_segment->soften_right_rail();
		//track_segment->soften_right_rail();
		//track_segment->soften_right_rail();
		//track_segment->soften_right_rail();

		// append the segment
		track->append_segment(track_segment);
	}

	track_segment = create_track_segmentE();
	track->append_segment(track_segment);

	track_segment = create_track_segmentE();
	track_segment->color_type = COLOR_TYPE_WHITE;
	track->append_segment(track_segment);

	track->__HACK_finalize_track();

	return true;
}




// core and orginazation functions //

void init_game()
{
   std::cout << "start of bitmap bin path setting" << std::endl;
   bitmaps.set_path("data/images");
   std::cout << "end of bitmap bin path setting" << std::endl;
	animate(logo_scale, 0.7, 1.0, 7.0, interpolator::trippleFastIn);

	OMG_DeltaTime = 0.6;

	particle_effect::reserve_particles(100);

	start_text_color = al_color_name("white");

	engine_voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
	engine_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
	al_attach_mixer_to_voice(engine_mixer, engine_voice);

	engine_sample_instance = al_create_sample_instance(get_sample("engine"));
	al_attach_sample_instance_to_mixer(engine_sample_instance, engine_mixer);
	al_set_sample_instance_playmode(engine_sample_instance, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(engine_sample_instance, 0.0);
	al_set_sample_instance_speed(engine_sample_instance, 0.8);

	music_sample = al_load_sample("data/sounds/LightRacer music - 01.ogg");
	if (music_sample)
	{
		start_text_color = al_color_name("dodgerblue");
		music_sample_instance = al_create_sample_instance(music_sample);
		al_attach_sample_instance_to_mixer(music_sample_instance, engine_mixer);
		al_set_sample_instance_playmode(music_sample_instance, ALLEGRO_PLAYMODE_LOOP);
		al_set_sample_instance_gain(music_sample_instance, 1.0);
		al_set_sample_instance_speed(music_sample_instance, 1.0);
	}

	//al_play_sample_instance(engine_sample_instance);

	// gate pass-through sound

	passthough_sample_instance = al_create_sample_instance(get_sample("click"));
	al_attach_sample_instance_to_mixer(passthough_sample_instance, engine_mixer);
	al_set_sample_instance_playmode(passthough_sample_instance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(passthough_sample_instance, 1.0);
	al_set_sample_instance_speed(passthough_sample_instance, 1.0);

	// exit pass-through sound

	exit_sample_instance = al_create_sample_instance(get_sample("through_gate"));
	al_attach_sample_instance_to_mixer(exit_sample_instance, engine_mixer);
	al_set_sample_instance_playmode(exit_sample_instance, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(exit_sample_instance, 0.7);
	al_set_sample_instance_speed(exit_sample_instance, 1.0);
	//al_play_sample_instance(exit_sample_instance);




	instance_hit_bad = al_create_sample_instance(get_sample("hit_bad"));
	al_attach_sample_instance_to_mixer(instance_hit_bad, engine_mixer);
	al_set_sample_instance_playmode(instance_hit_bad, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instance_hit_bad, 0.7);
	al_set_sample_instance_speed(instance_hit_bad, 1.0);
	//al_play_sample_instance(exit_sample_instance);

	instance_hit_bounce = al_create_sample_instance(get_sample("hit_bounce"));
	al_attach_sample_instance_to_mixer(instance_hit_bounce, engine_mixer);
	al_set_sample_instance_playmode(instance_hit_bounce, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instance_hit_bounce, 0.7);
	al_set_sample_instance_speed(instance_hit_bounce, 1.0);
	//al_play_sample_instance(exit_sample_instance);


	instance_hit_med = al_create_sample_instance(get_sample("hit_med2"));
	al_attach_sample_instance_to_mixer(instance_hit_med, engine_mixer);
	al_set_sample_instance_playmode(instance_hit_med, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instance_hit_med, 0.7);
	al_set_sample_instance_speed(instance_hit_med, 1.0);
	//al_play_sample_instance(exit_sample_instance);

	instance_hit_soft = al_create_sample_instance(get_sample("hit_soft"));
	al_attach_sample_instance_to_mixer(instance_hit_soft, engine_mixer);
	al_set_sample_instance_playmode(instance_hit_soft, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instance_hit_soft, 0.7);
	al_set_sample_instance_speed(instance_hit_soft, 1.0);
	//al_play_sample_instance(exit_sample_instance);

	

	//al_play_sample(get_sample("engine"), 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_LOOP, nullptr);

	track = new Track();
	create_random_track(track, num_of_segments_in_track);
	start_track_begin_text();

	restart_music();

	racer = new Racer();

	camera = new CheapCamera();
	good_camera = new camera_class();

	fade_out_of_black();

	//start_track();
}


void key_down_func()
{
	switch(current_event->keyboard.keycode)
	{
	case ALLEGRO_KEY_ESCAPE:
		current_framework->abort_program = true;
		break;
	case ALLEGRO_KEY_ENTER:
		if (game_over || game_won)
		{
			delete track;
			track = new Track();
			game_won = false;
			final_course = false;
			game_over = false;
			num_lives = max_num_lives;
			num_of_segments_in_track = 4;
			create_random_track(track, num_of_segments_in_track);
		}
		if (track_completed)
		{
			delete track;
			track = new Track();
			num_of_segments_in_track += 6;
			if (num_of_segments_in_track > 30)
			{
				num_of_segments_in_track = 30;
				final_course = true;
			}
			create_random_track(track, num_of_segments_in_track);
		}
		start_track();
		break;
	}
}


/*
void DEVELOPER_key_down_func()
{
	switch(current_event->keyboard.keycode)
	{
	case ALLEGRO_KEY_ENTER:
		if (track_completed)
		{
			delete track;
			track = new Track();
			num_of_segments_in_track += 6;
			create_random_track(track, num_of_segments_in_track);
		}
		start_track();
		break;
		//zoom_way_out();
		break;
	case ALLEGRO_KEY_PGDN:
		animate_to(camera->zoom, 0.5); 
		break;
	case ALLEGRO_KEY_HOME:
		zoom_normal();
		break;
	case ALLEGRO_KEY_E:
		delete track;
		track = new Track();
		create_random_track(track, 0);
		start_track();
		break;
	case ALLEGRO_KEY_N:
		delete track;
		track = new Track();
		create_random_track(track, num_of_segments_in_track);
		start_track();
		break;
	case ALLEGRO_KEY_K:
		kill_player(0);
		break;
	case ALLEGRO_KEY_F1:
		FLAG_draw_profile_graph = !FLAG_draw_profile_graph;
		break;
	case ALLEGRO_KEY_S:
		start_track();
		break;
	case ALLEGRO_KEY_OPENBRACE:
		OMG_DeltaTime -= 0.1;
		break;
	case ALLEGRO_KEY_CLOSEBRACE:
		OMG_DeltaTime += 0.1;
		break;
	}
}
*/


std::string get_number_string(int num)
{
	switch(num)
	{
	case 1:
		return "one";
		break;
	case 2:
		return "two";
		break;
	case 3:
		return "three";
		break;
	case 4:
		return "four";
		break;
	case 5:
		return "five";
		break;
	case 6:
		return "six";
		break;
	case 7:
		return "seven";
		break;
	case 8:
		return "eight";
		break;
	case 9:
		return "nine";
		break;
	case 10:
		return "ten";
		break;
	case 11:
		return "eleven";
		break;
	default: return tostring(num);
		break;
	}
}



#include "game_timer_func.h"



int main(int argc, char **argv)
{
	Framework f(SCREEN_W, SCREEN_H);
	init_profiling();
	init_game();
	f.timer_func = game_timer_func;
	f.key_down_func = key_down_func;
	return f.run_loop();

   bitmaps.clear();
}
