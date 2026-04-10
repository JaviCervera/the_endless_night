PROGRAM the_endless_night;

CONST
  _target_fps = 12;

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
  _crowbar_graph = 3;

  // Pickable objects
  _crowbar = 1;

  // Adventure states
  _open_barn = 1;
  _collect_parts = 2;
  _info_gathering = 3;
  _powering_up = 4;


  // Banner data
  _max_banner_texts = 10;
  _banner_ticks = 60; // Number of frames a text is visible for

  // Loop constants
  _loop_frames = 720; // Number of frames in a loop (90 seconds)

  // Sectors
  _shack_sector = 7;
  _barn_door_sector = 15;

GLOBAL
  adventure_state = _open_barn; // Progress made (every correct action increments this by 1)

  // State names
  _state_names[] =
    "",
    "The Awakening",
    "Collect Parts";

  // Action globals
  STRING action_text = ""; // The text to display at the bottom of the screen
  action_hit = 0; // Indicates if the action key was hit THE CURRENT FRAME

  // Banner related globals
  banner_texts[_max_banner_texts - 1]; // Allocate table for banner texts
  first_banner_text = 0; // The first banner text to draw
  num_banner_texts = 0; // The number of texts currently added
  banner_countdown = 0; // The frames remaining to show next banner texts

  // Graphics files
  STRUCT fpg_files
    textures;
    furniture;
    farmer;
  END

  // Sound files
  STRUCT pcm_files
    footsteps;
    humming;
  END

  // Font
  font;

  // Player global data
  player_id; // Player process id
  player_blocked = 0; // If blocked, cannot move or action. It unblocks after all banners are gone.
  player_sector; // The sector in which the player is located
  player_last_picked_object = 0; // Last object picked by player
  player_end_pos_x; // The player position when loop ended
  player_end_pos_y; // The player position when loop ended

  // Loop data
  STRUCT loop_data
    needs_restart = 0; // Whether the loop needs restart
    num_ticks = 0; // Number of frames passed in the current loop
    num_loop_in_state = 0; // The number of loops ran in the current state
    last_loop_state = 1; // Game state when the last loop occured
  END

  // General use variable that is printed at the bottom
  // of the debug data. Assign values here to trace their
  // value on the screen during development
  debug_data;
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
  set_fps(_target_fps, 4);

  // Load fpg
  fpg_files.textures = load_fpg("D:\TEXTURES.FPG");
  fpg_files.furniture = load_fpg("D:\FURNITUR.FPG");
  fpg_files.farmer = load_fpg("D:\FARMER.FPG");

  // Load pcm
  pcm_files.footsteps = load_pcm("D:\STEPS.PCM", 1);
  pcm_files.humming = load_pcm("D:\HUMMING.PCM", 1);

  //font = load_fnt("D:\FONT.FNT");

  define_region(_region_id, _region_x, _region_y, _region_width, _region_height);

  start_loop();

  LOOP
    IF (loop_data.needs_restart)
      let_me_alone();
      start_loop();
    END

    FRAME;
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
      text_id = write(font, 160, 190, 4, action_text);
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
  text_id_a = write(font, 160, 2, 1, "");
  text_id_b = write(font, 160, 10, 1, "");
  LOOP
    IF (num_banner_texts > 0 AND banner_countdown == 0)
      // If countdown reaches 0 and there are pending texts, draw next 2
      // and reset countdown
      delete_text(text_id_a);
      delete_text(text_id_b);
      text_id_a = write(font, 160, 2, 1, banner_texts[first_banner_text]);
      text_id_b = write(font, 160, 10, 1, banner_texts[(first_banner_text + 1) MOD _max_banner_texts]);
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
        text_id_a = write(font, 160, 2, 1, banner_texts[first_banner_text]);
        text_id_b = write(font, 160, 10, 1, banner_texts[(first_banner_text + 1) MOD _max_banner_texts]);
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
// LINEAR INTERPOLATION
// t should be in 0..max_t instead of 0..1
//-------------------------------------

FUNCTION lerp(a, b, t, max_t)
BEGIN
  RETURN(a + ((b - a ) * t) / max_t);
END

//-------------------------------------
// IN GAME PROCESSES
//-------------------------------------

PROCESS player()
PRIVATE
  footsteps_channel = -1;
  forward;
BEGIN
  ctype = c_m8;
  height = 110;
  radius = 32;
  go_to_flag(0);
  angle = 90000;
  IF (adventure_state == _open_barn AND loop_data.num_loop_in_state == 1)
    add_banner_text("");
    add_banner_text("What was that sound?");
    add_banner_text("");
    add_banner_text("It came from outside...");
  END
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
          footsteps_channel = sound(pcm_files.footsteps, 256, 350);
        END
      END
    END
    IF (forward == 0 AND footsteps_channel <> -1 AND is_playing_sound(footsteps_channel))
      stop_sound(footsteps_channel);
      footsteps_channel = -1;
    END
    z -= _gravity;
    player_sector = m8_sector;
    debug_data = player_sector;
    FRAME;
  END
END

PROCESS farmer()
PRIVATE
  STRING talk_str;
  STRING loop1_1_str, loop1_2_str, loop1_3_str;
  STRING loop2_1_str;
BEGIN
  talk_str = "Talk to farmer";
  loop1_1_str = "Something strange has happened in the fields";
  loop1_2_str = "to the south, but the path is closed.";
  loop1_3_str = "A shimmering light comes from there.";
  loop2_1_str = "I have the feeling that I have done this before...";
  /*
  f = fopen("D:\DAD.ES", "r");
  talk_str = read_string(f);
  woah_text = read_string(f);
  whathappened1_str = read_string(f);
  whathappened2_str = read_string(f);
  drunk1_str = read_string(f);
  drunk2_str = read_string(f);
  fclose(f);
  */

  ctype = c_m8;
  file = fpg_files.farmer;
  graph = 1;
  go_to_flag(21);
  LOOP
    IF (update_action(id, talk_str))
      IF (adventure_state == _open_barn)
        IF (loop_data.num_loop_in_state > 1)
          add_banner_text(loop2_1_str);
        END
        add_banner_text(loop1_1_str);
        add_banner_text(loop1_2_str);
        add_banner_text(loop1_3_str);
      END
    END
    FRAME;
  END
END

PROCESS barn_worker()
PRIVATE
  STRING talk_str;
  STRING loop1_1_str, loop1_2_str, loop1_3_str, loop1_4_str;
  STRING loop2_1_str;
BEGIN
  talk_str = "Talk to barn worker";
  loop1_1_str = "A strange machine has landed down there.";
  loop1_2_str = "I work at the barn to the west, but I can't";
  loop1_3_str = "open the door. I have some parts there that";
  loop1_4_str = "might be useful to analyse that humming sound.";
  loop2_1_str = "I think there is something odd...";

  ctype = c_m8;
  file = fpg_files.farmer;
  graph = 1;
  go_to_flag(22);
  LOOP
    IF (update_action(id, talk_str))
      IF (adventure_state == _open_barn)
        IF (loop_data.num_loop_in_state > 1)
          add_banner_text(loop2_1_str);
        END
        add_banner_text(loop1_1_str);
        add_banner_text(loop1_2_str);
        add_banner_text(loop1_3_str);
        add_banner_text(loop1_4_str);
      END
    END
    FRAME;
  END
END


PROCESS crowbar()
PRIVATE
  tick;
BEGIN
  IF (adventure_state <> _open_barn) RETURN; END
  ctype = c_m8;
  file = fpg_files.furniture;
  graph = _crowbar_graph;
  go_to_flag(30);
  IF (player_last_picked_object == _crowbar)
    x = player_end_pos_x;
    y = player_end_pos_y;
  END
  LOOP
    // Arrow animation
    tick++;
    IF (tick == 3)
      tick = 0;
      IF (graph == _crowbar_graph) graph++; ELSE graph--; END
    END

    // Action
    IF (player_sector == _shack_sector OR player_last_picked_object == _crowbar)
      IF (update_action(id, "Take crowbar"))
        action_text = "";
        signal(id, s_kill);
        IF (player_last_picked_object <> _crowbar)
          add_banner_text("With this I should be able to open the barn.");
        ELSE
          add_banner_text("The crowbar is where I left it!");
        END
        player_last_picked_object = _crowbar;
      END
    ELSE
      remove_action_text("Take crowbar");
    END

    FRAME;
  END
END

PROCESS barn_door()
PRIVATE
  seconds_passed;
BEGIN
  IF (adventure_state <> _open_barn)
    open_barn_door();
    RETURN;
  END
  ctype = c_m8;
  go_to_flag(23);
  LOOP
    IF (update_action(id, "Open barn door"))
      seconds_passed = loop_data.num_ticks / _target_fps;
      IF (seconds_passed > 10)
        add_banner_text("There is something blocking the door.");
      ELSE
        IF (get_id(TYPE crowbar) == 0)
          adventure_state++;
          open_barn_door();
          signal(id, s_kill);
          remove_action_text("Open barn door");
        ELSE
          add_banner_text("The door is locked.");
        END
      END
    END
    FRAME;
  END
END

PROCESS open_barn_door()
BEGIN
  set_sector_height(_barn_door_sector, 992, 1184);
END

PROCESS furniture(graph, flag)
BEGIN
  file = fpg_files.furniture;
  ctype = c_m8;
  radius = 8;
  go_to_flag(flag);
  LOOP
    FRAME;
  END
END

PROCESS humming()
PRIVATE
  chn;
  vol = 128;
BEGIN
  chn = sound(pcm_files.humming, vol, vol);
  LOOP
    IF (_loop_frames - loop_data.num_ticks < 120)
      vol++;
      change_sound(chn, vol, vol);
    END
    FRAME;
  END
END

PROCESS fog_updater()
PRIVATE
  val;
  s;
BEGIN
  ctype = c_m8;
  go_to_flag(31);
  LOOP
    IF (player_id <> 0)
      s = sin(angle);
      IF (s <> 0)
        val = lerp(100, 0, get_dist(player_id), abs(s));
        IF (val < 0) val = 0; END
        IF (val > 100) val = 100; END
      ELSE
        val = 0;
      END
      angle += 5000;

      // Update env color
      set_env_color(0, val, 0);
    END
    FRAME;
  END
END

//-------------------------------------
// LOOP CONTROL
//-------------------------------------

PROCESS start_loop()
PRIVATE
  i;
  chapter_text;
  loop_text;
  STRING chapter;
  STRING loop_;
BEGIN
  loop_data.needs_restart = 0;
  load_wld("D:\TOWN.WLD", fpg_files.textures);
  IF (adventure_state <> loop_data.last_loop_state)
    loop_data.last_loop_state = adventure_state;
    loop_data.num_loop_in_state = 1;
  ELSE
    loop_data.num_loop_in_state++;
  END

  stop_sounds();
  delete_text(all_text);
  fade(0, 0, 0, 64);
  fade_on();
  chapter = "CHAPTER " + itoa(adventure_state) + ": " + _state_names[adventure_state];
  loop_ = "Loop " + itoa(loop_data.num_loop_in_state);
  chapter_text = write(font, 160, 100, 4, chapter);
  FROM i = 1 TO 36;
    IF (i == 12 AND loop_data.num_loop_in_state > 1)
      loop_text = write(font, 160, 115, 4, loop_);
    END
    FRAME;
  END
  delete_text(chapter_text);
  delete_text(loop_text);
  fade_off();
  WHILE (fading) FRAME; END

  action_hit_controller();
  action_text_viewer();
  banner_text_viewer();

  reset_banner();

  player_id = player();

  start_mode8(player_id, 0, _region_id);
  set_fog(0, 100);
  m8.height=128;

  farmer();
  barn_worker();
  crowbar();
  barn_door();
  FROM i = 1 TO 20;
    furniture(_tree_graph, i);
  END
  humming();
  fog_updater();
  loop_controller();
  debug_data_viewer();

  fade_on();
END

PROCESS loop_controller()
BEGIN
  LOOP
    loop_data.num_ticks++;
    IF (loop_data.num_ticks == _loop_frames)
      loop_finisher();
      signal(id, s_kill);
    END
    FRAME;
  END
END

PROCESS loop_finisher()
BEGIN
  fade(200, 200, 200, 5);
  WHILE (fading) FRAME; END
  fade(100, 100, 100, 64);
  loop_data.num_ticks = 0;
  delete_text(all_text);
  stop_sounds();
  stop_mode8(0);
  loop_data.needs_restart = 1;
  player_end_pos_x = player_id.x;
  player_end_pos_y = player_id.y;
END

PROCESS stop_sounds()
BEGIN
  FROM x = 0 TO 32;
    stop_sound(x);
  END
END

//-------------------------------------
// DEBUG VIEWER
// Draws debug information on screen
//-------------------------------------

PROCESS debug_data_viewer()
PRIVATE
  STRING fps_str;
  fps_id;
  time_id;
  debug_data_id;
  secs_left;
BEGIN
  debug_data_id = write_int(font, 4, 44, 0, OFFSET debug_data);
  LOOP
    secs_left = (_loop_frames - loop_data.num_ticks) / _target_fps;
    fps_str = itoa(fps) + " FPS";
    fps_id = write(font, 4, 24, 0, fps_str);
    time_id = write(font, 4, 34, 0, time_fmt(secs_left));
    FRAME;
    delete_text(fps_id);
    delete_text(time_id);
  END
END

FUNCTION time_fmt(secs)
PRIVATE
  STRING str;
BEGIN
  str = pad2(secs / 60) + ":" + pad2(secs mod 60);
  RETURN(str);
END

FUNCTION pad2(val)
BEGIN
  IF (val < 10)
    RETURN("0" + itoa(val));
  ELSE
    RETURN(itoa(val));
  END
END