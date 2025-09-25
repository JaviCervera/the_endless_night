PROGRAM the_endless_night;

PRIVATE
    fpg;

BEGIN
    fpg = load_fpg("d:\wld_view.fpg");

    start_mode7(0, fpg, 62, 62, 0, 120);

    m7.height = 64;
    m7.distance = 0;
    m7.color = 0;
    m7.camera = id;

    write(0, 160, 0, 1, "Use the arrow keys to move");
    LOOP
        IF (key(_right)) angle -= 8000; END
        IF (key(_left)) angle += 8000; END
        IF (key(_up)) advance(6); END
        IF (key(_down)) advance(-6); END
        FRAME;
    END
END