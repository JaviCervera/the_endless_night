PROGRAM the_endless_night;

CONST
  // Region data
  _region_id = 1;
  _region_x = 0;
  _region_y = 20;
  _region_width = 320;
  _region_height = 160;

  // Movement data
  _gravity = 50;
  _forward_speed = 50;
  _backwards_speed = 20;
  _turn_speed = 15000;
  _walk_frames = 4;

  // Furniture graphs
  _tree_graph = 1;
  _bush_graph = 2;
  _standpipe_graph = 3;
  _garbage_graph = 4;

  // Adventure states
  _find_coin = 1;
  _buy_marbles = 2;
  _play_marbles = 3;
  _smash_car = 4;
  _get_keys = 5;
  _enter_house = 6;
  _game_over = 7;

  // Banner data
  _max_banner_texts = 10;
  _banner_ticks = 60; // Number of frames a text is visible for

GLOBAL
  adventure_state = _find_coin; // Progress made (every correct action increments this by 1)

  // Action globals
  STRING action_text = ""; // The text to display at the bottom of the screen
  action_hit = 0; // Indicates if the action key was hit THE CURRENT FRAME

  // Banner related globals
  banner_texts[_max_banner_texts - 1]; // Allocate table for banner texts
  first_banner_text = 0; // The first banner text to draw
  num_banner_texts = 0; // The number of texts currently added
  banner_countdown = 0; // The frames remaining to show next banner texts

  // Graphics files
  textures_fpg;
  furniture_fpg;

  // Player global data
  player_id; // Player process id
  player_blocked = 0; // If blocked, cannot move or action. It unblocks after all banners are gone.

  // Sounds used by several processes
  door_sound;
  money_sound;

PRIVATE
  f;
  STRING title_str;
  STRING credits_str;
  STRING contest_str;
  STRING thanks_str;
  title_text;
  counter;

BEGIN
  /*
  f = fopen("D:\GLOBAL.ES", "r");
  title_str = read_string(f);
  credits_str = read_string(f);
  contest_str = read_string(f);
  thanks_str = read_string(f);
  fclose(f);
  */

  set_mode(m320x200);
  set_fps(12, 4);

  /*
  door_sound = load_pcm("D:\DOOR.PCM", 0);
  money_sound = load_pcm("D:\COIN.PCM", 0);
  */
  textures_fpg = load_fpg("D:\WLD_VIEW.FPG");
  //furniture_fpg = load_fpg("D:\FURNITUR.FPG");

  load_wld("D:\WLD_VIEW.WLD", textures_fpg);

  write_int(0, 4, 24, 0, offset fps);
  write_int(0, 4, 34, 0, offset adventure_state);

  define_region(_region_id, _region_x, _region_y, _region_width, _region_height);
  action_hit_controller();
  action_text_viewer();
  banner_text_viewer();
  reset_banner();

  player_id = player();
  /*
  benji();
  seller();
  coin();
  car();
  keys();
  house();
  furniture(_tree_graph, 1);
  furniture(_tree_graph, 2);
  furniture(_bush_graph, 3);
  furniture(_bush_graph, 4);
  furniture(_bush_graph, 5);
  furniture(_standpipe_graph, 6);
  furniture(_garbage_graph, 7);
  furniture(_garbage_graph, 8);
  furniture(_tree_graph, 13);
  furniture(_tree_graph, 14);
  furniture(_tree_graph, 15);
  */

  start_mode8(player_id, 0, _region_id);
  m8.height=128;

  LOOP
    IF (adventure_state <> _game_over)
      FRAME;
    ELSE
      /*
      stop_song();
      stop_mode8(0);
      let_me_alone();
      delete_text(all_text);
      title_text = write(0, 160, 100, 4, title_str);
      counter = 0;
      fade_on();
      WHILE (counter < _banner_ticks)
        counter++;
        FRAME;
      END
      delete_text(title_text);
      title_text = write(0, 160, 100, 4, credits_str);
      counter = 0;
      WHILE (counter < _banner_ticks)
        counter++;
        FRAME;
      END
      delete_text(title_text);
      title_text = write(0, 160, 100, 4, contest_str);
      counter = 0;
      WHILE (counter < _banner_ticks)
        counter++;
        FRAME;
      END
      fade_off();
      WHILE (fading) FRAME; END
      exit(thanks_str, 0);
      */
    END
  END
END

//-------------------------------------
// ACTION TEXT
// Displayed at the bottom frame, indicates which action can be performed
// by the player, or "..." if the player is blocked.
//-------------------------------------

PROCESS action_text_viewer()
PRIVATE
  INT text_id = 0;
  STRING current_text = ""; // Tracks the string currently printed
BEGIN
  text_id = write(0, 160, 190, 4, "");
  LOOP
    IF (player_blocked)
      // Display "..." if player is blocked
      action_text = "...";
    ELSE
      IF (strcmp(action_text, "...") == 0)
        // Remove "..." when the player gets unblocked
        action_text = "";
      END
    END
    IF (strcmp(current_text, action_text) <> 0)
      // If "action_text" changes, replace the text on screen
      delete_text(text_id);
      text_id = write(0, 160, 190, 4, action_text);
      current_text = action_text;
    END
    FRAME;
  END
END

// If the action panel is displaying the text passed as argument, clears it
FUNCTION remove_action_text(STRING text)
BEGIN
  IF (strcmp(action_text, text) == 0)
    action_text = "";
  END
END

FUNCTION update_action(caller_id, STRING text)
BEGIN
  IF (player_blocked) RETURN(0); END
  IF (fget_dist(caller_id.x, caller_id.y, player_id.x, player_id.y) < 200)
    action_text = text;
    IF (action_hit)
      reset_banner();
      RETURN(1);
    END
  ELSE
    remove_action_text(text);
  END
  RETURN(0);
END

//-------------------------------------
// BANNER TEXT
// Displayed at the top, prints the character's dialogs and action results.
// A maximum of two lines can be displayed.
// The message is shown for _banner_ticks frames.
//-------------------------------------

PROCESS banner_text_viewer()
PRIVATE
  text_id_a;
  text_id_b;
BEGIN
  text_id_a = write(0, 160, 2, 1, "");
  text_id_b = write(0, 160, 10, 1, "");
  LOOP
    IF (num_banner_texts > 0 AND banner_countdown == 0)
      // If countdown reaches 0 and there are pending texts, draw next 2
      // and reset countdown
      delete_text(text_id_a);
      delete_text(text_id_b);
      text_id_a = write(0, 160, 2, 1, banner_texts[first_banner_text]);
      text_id_b = write(0, 160, 10, 1, banner_texts[(first_banner_text + 1) MOD _max_banner_texts]);
      banner_countdown = _banner_ticks;
    END
    IF (banner_countdown > 0)
      banner_countdown--;
      IF (banner_countdown == 0)
        banner_texts[first_banner_text] = "";
        banner_texts[(first_banner_text + 1) MOD _max_banner_texts] = "";
        first_banner_text = (first_banner_text + 2) MOD _max_banner_texts;
        num_banner_texts -= 2;
        delete_text(text_id_a);
        delete_text(text_id_b);
        text_id_a = write(0, 160, 2, 1, banner_texts[first_banner_text]);
        text_id_b = write(0, 160, 10, 1, banner_texts[(first_banner_text + 1) MOD _max_banner_texts]);
        IF (num_banner_texts > 0)
          banner_countdown = _banner_ticks;
        ELSE
          player_blocked = 0;
        END
      END
    END
    FRAME;
  END
END

FUNCTION add_banner_text(text)
PRIVATE
  last_banner_index = 0;
BEGIN
  last_banner_index = (first_banner_text + num_banner_texts - 1) MOD _max_banner_texts;
  IF (last_banner_index == -1) last_banner_index = 0; END
  IF (num_banner_texts < _max_banner_texts AND strcmp(banner_texts[last_banner_index], text) <> 0)
    banner_texts[(first_banner_text + num_banner_texts) MOD _max_banner_texts] = text;
    num_banner_texts++;
  END
END

FUNCTION reset_banner()
PRIVATE
  i;
BEGIN
  first_banner_text = 0;
  num_banner_texts = 0;
  banner_countdown = 0;
  FROM i = 0 TO _max_banner_texts -1;
    banner_texts[i] = "";
  END
END

//-------------------------------------
// ACTION CONTROLLER
// Updates action_hit global is action is hit the current frame
//-------------------------------------

PROCESS action_hit_controller()
PRIVATE
  space_was_hit = 0;
BEGIN
  LOOP
    IF (key(_space))
      IF (NOT space_was_hit)
        action_hit = 1;
        space_was_hit = 1;
      ELSE
        action_hit = 0;
      END
    ELSE
      action_hit = 0;
      space_was_hit = 0;
    END
    FRAME;
  END
END

//-------------------------------------
// STRING READING
//-------------------------------------

FUNCTION read_string(f)
PRIVATE
  i;
  chars[256];
  STRING text;
BEGIN
  fread(OFFSET chars[0], 1, f);
  fread(OFFSET chars[1], chars[0], f);
  FOR (i = 1; i <= chars[0]; i++)
    text += chars[i];
  END
  RETURN(text);
END

//-------------------------------------
// IN GAME PROCESSES
//-------------------------------------

PROCESS player()
PRIVATE
  footsteps;
  footsteps_channel = -1;
  forward;
BEGIN
  footsteps = load_pcm("D:\STEPS.PCM", 1);
  ctype = c_m8;
  height = 96;
  radius = 32;
  go_to_flag(0);
  LOOP
    IF (NOT player_blocked)
      forward = 0;
      IF (key(_right) OR key(_d)) angle -= _turn_speed; END
      IF (key(_left) OR key(_a)) angle += _turn_speed; END
      IF (key(_up) OR key(_w)) forward += _forward_speed; END
      IF (key(_down) OR key(_s)) forward -= _backwards_speed; END
      IF (forward <> 0)
        advance(forward);
        IF (NOT is_playing_sound(footsteps_channel))
          footsteps_channel = sound(footsteps, 256, 350);
        END
      END
    END
    IF (forward == 0 AND footsteps_channel <> -1 AND is_playing_sound(footsteps_channel))
      stop_sound(footsteps_channel);
      footsteps_channel = -1;
    END
    z -= _gravity;
    FRAME;
  END
END
