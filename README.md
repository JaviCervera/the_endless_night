# The Endless Night

![Cover](stuff/cover.png)

An alien horror adventure in glorious 3D - Every night is the night of reckoning! Only for MS-DOS

## Installing DJGPP on Windows 11 and compiling

Download: https://github.com/andrewwutw/build-djgpp/releases/download/v3.4/djgpp-mingw-gcc1220.zip

1. Extract the package
  * Expand-Archive djgpp-mingw-gcc1220.zip -DestinationPath C:\djgpp
  * Or right-click the ZIP → "Extract All" → type `C:\djgpp` as the destination.
  * You should end up with `C:\djgpp\bin\i586-pc-msdosdjgpp-g++.exe`.
2. Add DJGPP to your PATH (optional but convenient)
  * Open Settings → System → About → Advanced system settings → Environment Variables.
  * Under System variables, edit `Path` and add: `C:\djgpp\bin`
3. Set `DJDIR` (required by the linker)
  * In Environment Variables create a new System variable:
    - Variable name: `DJDIR`
    - Variable value: `C:\djgpp`
  * `dosbuild.bat` sets `DJDIR` automatically for its own session, so this is only necessary if you want to call the DJGPP tools directly from a terminal.
4. Build for DOS
  * Run `dosbuild.bat`
   This produces `endless.exe` and runs it in DOSBox (which must have been previously installed).

![Poster](stuff/poster.png)

## Premise

A shimmering alien mothership lands in a field near a quiet village just after sunset. Every few minutes, an eerie pulse of light resets the world to the same moment of arrival. Objects you move, damage, or collect persist between loops. Your mission: Find the pieces requires to build an EMP device, so that the ship is disabled and the time loop is broken. For this, you need to collect a generator, a capacitor, and an alien crystal.

As you make progress, future loops will see more aliens patrol the areas.

## World Structure

* Village Square – NPCs reset every loop; their dialogue hints that they are noticing something strange. The will give you clues on how to progress.
* Mechanic’s Workshop – Source of tools; locked behind a code.
* Old Barn – Houses explosives; requires crowbar found elsewhere.
* Radio Tower – Lets you intercept alien signals to learn weaknesses.
* Fields – Mothership landing site; guarded by alien drones.

I'd like a feeling of unease with a distant hum of the mothership. When the reset time draws closer, the humming should become stronger and a you hear ticking clocks.

## Progression

If we were doing a straight progression from start to end, the loops would be divided as follows (but in reality the player
might require several iterations of a loop to complete the objectives).

### 1. The Awakening

* Objective: Introduce the loop & persistence mechanic.
* Player Actions:
  1. Player wakes in their cabin to a thunderous hum.
  2. Outside, villagers are panicked; a green light illuminates the sky.
  3. Talk to NPCs to learn of the “strange machine” in the fields.
  4. Discover a locked barn door and a hidden crowbar in a shed.
  5. Bring the crowbar to the barn before the reset.
  6. If you try to open, there's something blocking the door.
* Takeaway: Crowbar remains where you dropped it → persistence revealed.

### 2. Collect Parts

* Objective: Use persistence to open a new area.
* Player Actions:
  1. Rush to the crowbar and reach the old barn.
  2. If you reach the barn in the first 10 seconds, you can open the door (after that, a sandbag is dropped by a machine inside the barn, blocking the door).
  3. Discover generator parts and alien residue.
* Tease: A faint alien signal is audible near the radio tower.

### 3. Information Gathering

* Objective: Learn the alien weakness.
* Player Actions:
  1. Explore radio tower, find locked terminal (needs code).
  2. Discover scrap of paper with partial code inside the barn or mechanic’s workshop.
  3. Collect fuel canisters near the tractor.
* Twist: Aliens begin light patrols near the field.

### 4. Powering Up

* Objective: Activate the radio tower.
* Player Actions:
  1. Bring generator parts to the tower (may take more than one run).
  2. Power up the terminal → intercept alien transmissions.
  3. Learn mothership shield weak to EMP pulse.
* Optional Path: Discover notes hinting at alternative bomb solution.

### 5. Ingredients & Assembly

* Objective: Begin constructing the weapon.
* Player Actions:
  1. Gather capacitors from the mechanic’s workshop.
  2. Drag alien crystal from a downed drone (field patrol).
  3. Hide parts in the barn to keep them “safe” for next loop.

### 6. Alien Countermeasures

* Objective: Raise stakes.
* Events:
  1. Aliens deploy stronger patrols and motion-activated lights.
  2. Timer slightly shortens (e.g., 1 → 0.5 minutes) to increase tension.
* Player Actions:
  3. Finish wiring EMP device.
* Retrieve final detonator component from village blacksmith.

### 8. The Final Setup

* Objective: Assemble & transport the weapon to the field.
* Player Actions:
  1. Carefully sneak past patrols with the assembled device.
  2. Place it under the mothership.
* Ending Choice:
  1. Hit remote trigger and escape before reset, or
  2. Stay and watch as time collapses with the ship.

## Map

```
   Radio Tower  Church (REJECTED)
      ▲           ▲
      │           │
 Barn ◄── Village ──► Mechanic’s Workshop ──► Harvest
               │
           Mothership Field

```

## IDEAS

+ The path to the mothership is blocked by vines. You have to find herbicide (on the harvest) and put them on the vines, which will cause them to not appear on the next loops. ACCEPTED.
+ The time distortion could be stronger close to the mothership. Your actions when you are near it could be recorded, and on each loop, you could see your ghost with the actions you did on the last few loops (for example, last four loops). You could achieve victory by by performing a certain action in sync with your past selves. If all four do the action within a certain time window, the EMP pulse is activated (enable electricity on a pylon?). ACCEPTED.
- It would be nice if there was one game mechanic that requires you to leave an object at a certain place, so that when the loop restarts, the element being there will cause another object to behave differently. REJECTED.
- Persistence could only apply to the surroundings of the barn. If loop resets when you are near it, whatever you are carrying persists. REJECTED.
- New puzzle: There's a church in front of your cabin. A few seconds after loop starts, a rock slides from the roof and falls on the side. There is a rope connected to the bell. Next to the rock, one of the beams has a hook. If you attach the rope to the hook, on the next loop the rock will hit the rope and the bell will sound. We can use that for example to reduce the number of patrols nearby (maybe the path to the radio tower is much harder otherwise?). REJECTED.
