#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

#include "AllegroFlare/FontBin.hpp"
#include "AllegroFlare/SampleBin.hpp"
#include "AllegroFlare/Useful.hpp"
#include "AllegroFlare/Vec2D.hpp"
using AllegroFlare::vec2d;
//#include "position2d.h"
//#include "object2d.h"
#include "AllegroFlare/Interpolators.hpp"
#include "AllegroFlare/Motion.hpp"
#include <AllegroFlare/FrameworkScreenRegistrar.hpp>



#define SCREEN_W 1920
#define SCREEN_H 1080
#define SCREEN_HW (SCREEN_W/2)
#define SCREEN_HH (SCREEN_H/2)

#include "allegro_flare/profile_timer.h"
using namespace allegro_flare;


using namespace AllegroFlare;
//Motion motion;


#include <vector>
#include <cmath>


#include "allegro_flare/camera3d.h"


using std::vector;



#include <AllegroFlare/BitmapBin.hpp>
using AllegroFlare::BitmapBin;
BitmapBin bitmaps;


#include <AllegroFlare/FontBin.hpp>
using AllegroFlare::FontBin;
FontBin fonts;


#include <AllegroFlare/SampleBin.hpp>
using AllegroFlare::SampleBin;
SampleBin samples;


#include <AllegroFlare/Timer.hpp>
AllegroFlare::Timer stopwatch;



bool logo_showing = true;
float logo_scale = 0.0;


float delay_time_since_last_affect = 1.0;



// restart music at beginning of thing

// longer music

// remove extra time at end of music

// exit gate sound




ALLEGRO_BITMAP *black_screen_overlay = nullptr;




//ALLEGRO_SAMPLE_INSTANCE *engine_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *music_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance = nullptr;
ALLEGRO_SAMPLE *music_sample = nullptr;

ALLEGRO_VOICE *engine_voice = nullptr;
ALLEGRO_MIXER *engine_mixer = nullptr;
bool FLAG_draw_profile_graph = false;

ALLEGRO_COLOR start_text_color;




#include "Lightracer/PlayerStats.hpp"




//bool game_won = false;
//bool game_over = false;

bool final_course = false;


float lap_notification_counter = 0.0;
float track_begin_notification_counter = 0.0;


void init_black_screen_overlay()
{
   ALLEGRO_STATE previous_bitmap_state;
   al_store_state(&previous_bitmap_state, ALLEGRO_STATE_TARGET_BITMAP);

   black_screen_overlay = al_create_bitmap(SCREEN_W/3, SCREEN_H/3);
   al_set_target_bitmap(black_screen_overlay);
   al_clear_to_color(al_color_name("white"));

   al_restore_state(&previous_bitmap_state);
}


void draw_black_screen_overlay(ALLEGRO_COLOR color)
{
   static int NO_FLAGS = 0;
   al_draw_tinted_scaled_bitmap(black_screen_overlay, color,
      0, 0, al_get_bitmap_width(black_screen_overlay), al_get_bitmap_height(black_screen_overlay),
      0, 0, SCREEN_W, SCREEN_H, NO_FLAGS);
}



void restart_music()
{
   if (!music_sample) return;
   al_stop_sample_instance(music_sample_instance);
   al_play_sample_instance(music_sample_instance);
}


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





//int segment_where_player_died = 0;


void start_track_begin_text()
{
   float start_time = al_get_time();
   float end_time = start_time + 4.0;
   motion.animate(&track_begin_notification_counter, 1.0, 0.0, start_time, end_time, interpolator::tripple_fast_out);
}


//int num_of_segments_in_track = 4;



float finish_track_animation_counter = 1.0;
//bool track_completed = false;

int num_laps_to_win = 4;

float foreground_black_opacity = 1.0;
float foreground_white_opacity = 0.0;


int index_of_last_track_segment_that_collides = 0;



#include "Lightracer/shared.hpp"



#include "Lightracer/CheapCamera.hpp"



#include "Lightracer/Camera.hpp"



Camera *good_camera = nullptr;


void zoom_way_out()
{
   motion.move_to(&camera->zoom, 0.05, 0.4);
}

void zoom_normal()
{
   motion.move_to(&camera->zoom, 1.0, 0.4);
}



#include "Lightracer/LineSegmentInfo.hpp"





#include "Lightracer/TrackSegment.hpp"


// track construction functions //


#include "Lightracer/TrackFactory.hpp"


#include "Lightracer/Track.hpp"


void complete_track();
void mark_player_not_using_boost(void *);


#include "Lightracer/Racer.hpp"



#include "Lightracer/Hud.hpp"




// globuals //



void mark_player_not_using_boost(void *)
{
   racer->using_boost = false;
}






//#include "image_bin.h"

float get_zoom_scale(float const &world_z, CheapCamera *cam)
{
    float distance = cam->z - world_z;
   if (distance <= 0) return 0;
   if (cam->depth <= 0) return 0;

    return cam->depth / distance; 
}

inline float scale(float &val)
{
   return val;
}



inline void rotate_point(vec2d *point, const vec2d &axis, float angle)
{
   const float c = cosf(-angle * ALLEGRO_PI / 180);
   const float s = sinf(-angle * ALLEGRO_PI / 180);

   point->x -= axis.x;
   point->y -= axis.y;
   
   const float tx = point->x;
   
   point->x = c * tx - s * point->y;
   point->y = s * tx + c * point->y;
   
   point->x += axis.x;
   point->y += axis.y;
}


inline void project_point(ALLEGRO_VERTEX &v, CheapCamera *cam, float multiplier=0.1)
{
   //if (v.y < 0) v.y = 0;
   //v.x -= cam->x;
   v.x -= cam->x;
   v.y -= cam->y;
   vec2d p(v.x, v.y);
   p = rotate_point(p, cam->rotation + FULL_ROTATION/2);
   v.x = p.x;
   v.y = p.y;
   //v.x *= get_zoom_scale(v.y, cam);
   v.x *= get_zoom_scale(v.y*multiplier, cam);
}





// hud //

void draw_text_with_letter_spacing(int font_size, ALLEGRO_COLOR color, int x, int y, float letter_spacing, ALLEGRO_FONT *font, std::string string_to_write)
{
   //ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf", font_size);
   //ALLEGRO_COLOR color = al_color_name("white");

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


// game calculation functions //





#include <algorithm>



#include "Lightracer/RacerTrackCollisionResolver.hpp"





void start_track()
{
   float start_time = al_get_time();
   float end_time = start_time + 3.0;
   motion.animate(&camera->zoom, 0.2, 1.0, start_time, end_time, interpolator::double_slow_in_out);
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

   fade_out_of_black(motion, foreground_black_opacity);

}




// core and orginazation functions //

void init_game()
{
   init_black_screen_overlay();

   std::cout << "start of bitmap bin path setting" << std::endl;
   bitmaps.set_path("data/images");
   samples.set_path("data/sounds");
   fonts.set_path("data/fonts");
   std::cout << "end of bitmap bin path setting" << std::endl;
   float start_time = al_get_time();
   float end_time = start_time + 7.0;
   motion.animate(&logo_scale, 0.7, 1.0, start_time, end_time, interpolator::tripple_fast_in);

   start_text_color = al_color_name("white");

   engine_voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
   engine_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
   al_attach_mixer_to_voice(engine_mixer, engine_voice);

   engine_sample_instance = al_create_sample_instance(samples["engine.ogg"]);
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

   passthough_sample_instance = al_create_sample_instance(samples["click.ogg"]);
   al_attach_sample_instance_to_mixer(passthough_sample_instance, engine_mixer);
   al_set_sample_instance_playmode(passthough_sample_instance, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(passthough_sample_instance, 1.0);
   al_set_sample_instance_speed(passthough_sample_instance, 1.0);

   // exit pass-through sound

   exit_sample_instance = al_create_sample_instance(samples["through_gate.ogg"]);
   al_attach_sample_instance_to_mixer(exit_sample_instance, engine_mixer);
   al_set_sample_instance_playmode(exit_sample_instance, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(exit_sample_instance, 0.7);
   al_set_sample_instance_speed(exit_sample_instance, 1.0);
   //al_play_sample_instance(exit_sample_instance);




   instance_hit_bad = al_create_sample_instance(samples["hit_bad.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_bad, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_bad, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_bad, 0.7);
   al_set_sample_instance_speed(instance_hit_bad, 1.0);
   //al_play_sample_instance(exit_sample_instance);

   instance_hit_bounce = al_create_sample_instance(samples["hit_bounce.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_bounce, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_bounce, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_bounce, 0.7);
   al_set_sample_instance_speed(instance_hit_bounce, 1.0);
   //al_play_sample_instance(exit_sample_instance);


   instance_hit_med = al_create_sample_instance(samples["hit_med2.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_med, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_med, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_med, 0.7);
   al_set_sample_instance_speed(instance_hit_med, 1.0);
   //al_play_sample_instance(exit_sample_instance);

   instance_hit_soft = al_create_sample_instance(samples["hit_soft.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_soft, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_soft, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_soft, 0.7);
   al_set_sample_instance_speed(instance_hit_soft, 1.0);
   //al_play_sample_instance(exit_sample_instance);

   

   //al_play_sample(samples["engine"), 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_LOOP, nullptr);

   track = new Track();
   TrackFactory::create_random_track(segment_where_player_died, track, num_of_segments_in_track);
   start_track_begin_text();

   restart_music();

   racer = new Racer();

   camera = new CheapCamera();
   good_camera = new Camera(SCREEN_W, SCREEN_H);

   fade_out_of_black(motion, foreground_black_opacity);

   //start_track();
}



void destroy_game()
{
   std::cout << CONSOLE_COLOR_YELLOW << "Shutting down game..." << CONSOLE_COLOR_DEFAULT << std::endl;
   samples.clear();
   fonts.clear();
   bitmaps.clear();
   std::cout << CONSOLE_COLOR_YELLOW << "...done." << CONSOLE_COLOR_DEFAULT << std::endl;
}



void key_char_func(ALLEGRO_EVENT *current_event)
{
   //z.update();

   //if (z.get_num_active_animations() != 0) debug_message("camera on dolly");

   //debug_message("Camera (" + tostring(x) + ", " + tostring(y) + ", " + tostring(z) + ")");
   //debug_message("Camera Depth (" + tostring(depth) + ")");

   switch(current_event->keyboard.keycode)
   {
      case ALLEGRO_KEY_PAD_6: good_camera->x += 2.5f; break;
      case ALLEGRO_KEY_PAD_4: good_camera->x -= 2.5f; break;
      case ALLEGRO_KEY_PAD_8: good_camera->y -= 2.5f; break;
      case ALLEGRO_KEY_PAD_2: good_camera->y += 2.5f; break;

      case ALLEGRO_KEY_PAD_PLUS: good_camera->z += 0.5f; break;
      case ALLEGRO_KEY_PAD_MINUS: good_camera->z -= 0.5f; break;
      case ALLEGRO_KEY_PAD_ASTERISK: good_camera->depth -= 1.0f; break;
      case ALLEGRO_KEY_PAD_SLASH: good_camera->depth += 1.0f; break;

      //if (just_pressed(ALLEGRO_KEY_F2: draw_objects_in_3D_space = !draw_objects_in_3D_space;
   }
}


void racer__on_key_up(PlayerStats &player_stats, ALLEGRO_EVENT *current_event)
{
   if (racer->dead) return;

   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_UP:
      if (racer->throttle_on) player_stats.increment_throttle_release_count();
      racer->throttle_on = false;
      break;
   case ALLEGRO_KEY_LEFT:
      racer->turning_left = false;
      break;
   case ALLEGRO_KEY_RIGHT:
      racer->turning_right = false;
      break;
   case ALLEGRO_KEY_DOWN:
      racer->break_on = false;
      break;
   }
}


void racer__on_key_down(ALLEGRO_EVENT *current_event)
{
   if (racer->dead) return;

   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_UP:
      if (!stopwatch.is_running()) stopwatch.start();
      racer->throttle_on = true;
      break;
   case ALLEGRO_KEY_LEFT:
      racer->turning_left = true;
      break;
   case ALLEGRO_KEY_RIGHT:
      racer->turning_right = true;
      break;
   case ALLEGRO_KEY_DOWN:
      racer->break_on = true;
      break;
   case ALLEGRO_KEY_SPACE:
      //use_boost(motion, mark_player_not_using_boost);
      break;
   }
}



void key_up_func(PlayerStats &player_stats, ALLEGRO_EVENT *current_event)
{
   racer__on_key_up(player_stats, current_event);
}


#include "AllegroFlare/Framework.hpp"



void key_down_func(Framework &framework, ALLEGRO_EVENT *current_event)
{
   if (!track_completed) racer__on_key_down(current_event);

   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_ESCAPE:
      framework.shutdown_program = true;
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
         TrackFactory::create_random_track(segment_where_player_died, track, num_of_segments_in_track);
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
         TrackFactory::create_random_track(segment_where_player_died, track, num_of_segments_in_track);
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


void main_menu_timer_func(ALLEGRO_EVENT *event)
{
   float screen_center_x = SCREEN_HW;
   float screen_center_y = SCREEN_HH;

   ALLEGRO_BITMAP *logo_img = bitmaps.auto_get("lightracer-max-logo-02.png");
   ALLEGRO_COLOR logo_fade_opacity = al_map_rgba_f(1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity);
   al_draw_tinted_scaled_rotated_bitmap(logo_img, logo_fade_opacity, al_get_bitmap_width(logo_img)/2, al_get_bitmap_height(logo_img)/2,
      screen_center_x, (250 - 300) + screen_center_y, logo_scale, logo_scale, 0, 0);

   int font_size = -25;
   draw_text_with_letter_spacing(font_size, al_color_name("white"), screen_center_x, (375 - 300) + screen_center_y + 20, 12*2, fonts["venus_rising_rg.ttf 25"], "press ENTER to BEGIN");
   //al_draw_text(fonts["venus_rising_rg.ttf", -25), al_color_name("white"),
      //screen_center_x, (375 - 300) + screen_center_y + 20, ALLEGRO_ALIGN_CENTRE, "press ANY KEY to BEGIN");

   //draw_black_screen_overlay(al_map_rgba_f(0, 0, 0, foreground_black_opacity));
}



using Lightracer::PlayerStats;



#include "Lightracer/Camera3.hpp"
#include "Lightracer/GLRenderer.hpp"
#include "Lightracer/Banner/TrackCompleted.hpp"
#include "Lightracer/Banner/Lap.hpp"
#include "Lightracer/Banner/YouFailed.hpp"


void game_timer_func(Lightracer::PlayerStats &player_stats, Camera3 &camera3, Display *display, ModelBin &models, ALLEGRO_EVENT *current_event, ALLEGRO_BITMAP *sub_bitmap_backbuffer_of_display_for_gl_projection)
{
   float screen_center_x = SCREEN_HW;
   float screen_center_y = SCREEN_HH;

   static ALLEGRO_FONT *font_regular = fonts["venus_rising_rg.ttf 28"];
   static ALLEGRO_FONT *font_large = fonts["venus_rising_rg.ttf 50"];



   delay_time_since_last_affect -= 0.2;
   if (delay_time_since_last_affect < 0) delay_time_since_last_affect = -0.1f;



   if (track_completed)
   {
      stopwatch.pause();
      racer->velocity_magnitude *= 0.98;
      racer->throttle_on = false;
   }



   start_profile_timer("racer/track");
   RacerTrackCollisionResolver collision_resolver(
        racer
      , player_stats
      , track
      , index_of_last_track_segment_that_collides
      , passthough_sample_instance
      , num_laps_to_win
      , lap_notification_counter
      , motion
      , delay_time_since_last_affect
      , foreground_white_opacity
      , exit_sample_instance
      );
   collision_resolver.resolve();
   stop_profile_timer("racer/track");


   camera->x = racer->position.x;
   camera->y = racer->position.y;
   camera->rotation = racer->direction_angle;
   camera->transform_on();


   good_camera->x = racer->position.x;
   good_camera->z = racer->position.y;

   al_set_sample_instance_speed(engine_sample_instance, 0.8+(racer->velocity_magnitude*racer->velocity_magnitude)/7);
   al_set_sample_instance_gain(engine_sample_instance, 0.6);


   // draw_gl_projection
         float racer_speed = racer->velocity_magnitude;
         good_camera->z += 100 + 30*(4.0-racer_speed);
         float camera_y = 550 - 65*racer_speed;  // higher numbers (400) mean flatter, more birds-eye perspective
         //float track_y_value = 50 + 50*(4.0-racer_speed) + (1.0-camera->zoom)*1000;

         camera3.spin = -camera->rotation + FULL_ROTATION/2;
         camera3.stepout = vec3d(0, 0, 10 + camera_y * 0.05);

         ALLEGRO_STATE previous_bitmap_state;
         al_store_state(&previous_bitmap_state, ALLEGRO_STATE_TARGET_BITMAP);
         al_set_target_bitmap(sub_bitmap_backbuffer_of_display_for_gl_projection);
         ALLEGRO_DISPLAY *al_display = display->al_display;
         GLRenderer().draw_gl_projection(al_display, camera3, racer, sub_bitmap_backbuffer_of_display_for_gl_projection, bitmaps, models, track, index_of_last_track_segment_that_collides);
         al_restore_state(&previous_bitmap_state);



   int current_lap_num = (int)racer->lap_time.size()+1;
   int current_racer_health = racer->health;
   int current_racer_max_health = racer->max_health;
   int count_of_removed_track_segments = segment_where_player_died.size();
   Hud(racer->velocity_magnitude, num_of_segments_in_track, current_racer_health, current_racer_max_health, current_lap_num, num_laps_to_win, fonts, SCREEN_W, SCREEN_H, stopwatch, num_lives, player_stats.get_wall_hit_count(), player_stats.get_throttle_release_count(), count_of_removed_track_segments).draw();


   ALLEGRO_TRANSFORM ident;
   al_identity_transform(&ident);
   al_use_transform(&ident);

   //if (foreground_black_opacity >= 0.01) draw_black_screen_overlay(al_map_rgba_f(0, 0, 0, foreground_black_opacity));

   //ALLEGRO_COLOR __foreground_color = al_map_rgba_f(foreground_white_opacity, foreground_white_opacity, foreground_white_opacity, foreground_white_opacity);
   //draw_black_screen_overlay(__foreground_color);


   if (game_won)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "YOU WIN");
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "congratulations and");
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 300-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "thanks for playing!");
   }
   else if (game_over)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("red"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
      al_draw_text(font_regular, al_color_name("red"), screen_center_x, 350-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to START A NEW GAME");
   }
   else if (racer->dead)
   {
      stopwatch.pause();

      Banner::YouFailed you_failed_banner(font_large, font_regular, screen_center_x, screen_center_y);
      you_failed_banner.draw();
   }
   else
   {
      bool show_lap_banner = lap_notification_counter != 0 && ((racer->lap_time.size()) != num_laps_to_win);

      if (show_lap_banner)
      {
         int lap_number = racer->lap_time.size()+1;
         Banner::Lap lap(font_large, font_regular, screen_center_x, screen_center_y, lap_number);
         lap.draw();
      }

      if (track_completed)
      {
         Banner::TrackCompleted track_completed_banner(font_large, font_regular, screen_center_x, screen_center_y);
         track_completed_banner.draw();
      }

   }


}



#include "Lightracer/Camera3.hpp"



#include "AllegroFlare/Framework.hpp"
#include "AllegroFlare/Screen.hpp"


using Lightracer::PlayerStats;


class LightracerMax : public Screen
{
public:
   ALLEGRO_BITMAP *sub_bitmap_backbuffer_of_display_for_gl_projection;
   Camera3 camera3;
   Camera3D camera3d;
   ModelBin models;
   PlayerStats player_stats;
   Framework &framework;

   LightracerMax(Framework &framework, Screens &screens, Display *display)
      : Screen(display)
      , framework(framework)
      , sub_bitmap_backbuffer_of_display_for_gl_projection(nullptr)
      , camera3()
      , camera3d(vec3d(0, 0, 0))
      , models()
      , player_stats()
   {}

   void initialize()
   {
      init_game();
      initialize_gl_render();
   }

   virtual void primary_timer_func() override
   {
      motion.update(al_get_time());

      start_profile_timer("WHOLE UPDATE");
      al_clear_to_color(al_color_name("black"));

      if (logo_showing)
      {
         main_menu_timer_func(framework.current_event);
      }
      else
      {
         game_timer_func(player_stats, camera3, display, models, framework.current_event, sub_bitmap_backbuffer_of_display_for_gl_projection);
      }

      stop_profile_timer("WHOLE UPDATE");
      if (FLAG_draw_profile_graph) draw_profile_timer_graph(20, 20);
   }

   void initialize_gl_render()
   {
      camera3.stepout = vec3d(0, 0, 10);
		camera3.tilt = 0.6;

      models.set_path("data/models");
      ALLEGRO_BITMAP *main_target = al_get_backbuffer(display->al_display);
      sub_bitmap_backbuffer_of_display_for_gl_projection = al_create_sub_bitmap(main_target,
         0, 0, al_get_bitmap_width(main_target), al_get_bitmap_height(main_target));
   }

   virtual void key_up_func(ALLEGRO_EVENT *ev) override
   {
      ::key_up_func(player_stats, framework.current_event);
   }

   virtual void key_down_func(ALLEGRO_EVENT *ev) override
   {
      ::key_down_func(framework, framework.current_event);
   }

   virtual void key_char_func(ALLEGRO_EVENT *ev) override
   {
      ::key_char_func(framework.current_event);
   }
};


int main(int argc, char **argv)
{
   Screens screens;
   Framework framework(&screens);
   framework.initialize();
   Display *display = framework.create_display(SCREEN_W, SCREEN_H);

   LightracerMax lightracer_max(framework, screens, display);
   lightracer_max.initialize();
   AllegroFlare::FrameworkScreenRegistrar(&screens, &lightracer_max).append();

   framework.run_loop();

   destroy_game();

   return 0;
}
