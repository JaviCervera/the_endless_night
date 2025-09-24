PROGRAM ejemplo_start_mode8;

GLOBAL
    fichero;

BEGIN
  set_mode(m640x480);
  set_fps(70,0);
  fichero = load_fpg("D:\WLD_VIEW.FPG");
  load_wld("D:\WLD_VIEW.WLD",fichero);

  start_mode8(id,0,0); // Inicializa una regi¢n de modo 8

  ctype=c_m8;
  height=64;
  radius=64;
  m8.height=32;
  go_to_flag(0);
  LOOP
      IF (key(_right)) angle-=5000; END
      IF (key(_left))  angle+=5000; END
      IF (key(_up))    advance(24); END
      IF (key(_down))  advance(-9); END
      IF (key(_q))     z+=16;       END
      IF (key(_a))     z-=16;       END
      IF (key(_w))     m8.angle+=8; END
      IF (key(_s))     m8.angle-=8; END
      FRAME;
  END
END