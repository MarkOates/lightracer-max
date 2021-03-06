#pragma once


#include "Lightracer/Racer.hpp"
#include "Lightracer/Track.hpp"
#include "Lightracer/PlayerStats.hpp"
using Lightracer::PlayerStats;

#include "allegro5/allegro_audio.h"


class RacerTrackCollisionResolver
{
private:
   Racer &player;
   PlayerStats &player_stats;
   Track *track;

   double OMG_DeltaTime;
   int &index_of_last_track_segment_that_collides;
   ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance;
   int &num_laps_to_win;
   float &lap_notification_counter;
   Motion &motion;
   float &delay_time_since_last_affect;
   float &foreground_white_opacity;
   ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance;

public:
   RacerTrackCollisionResolver(
        Racer *racer
      , PlayerStats &player_stats
      , Track *track
      , int &index_of_last_track_segment_that_collides
      , ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance
      , int &num_laps_to_win
      , float &lap_notification_counter
      , Motion &motion
      , float &delay_time_since_last_affect
      , float &foreground_white_opacity
      , ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance
   );

   void update_player_velocity_and_direction();
   void update_racer_along_track();
   void resolve();
};


