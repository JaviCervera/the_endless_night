# The Endless Night

![Cover](stuff/cover.png)

An alien horror adventure in glorious 3D - Every night is the night of reckoning! Only for MS-DOS

## Instructions (Windows)

* Extract "DIV2_DosBox.7z" into the project root, so that the DIV2 executable is "<root>/C_DRIVE/DIV2/D.EXE".
* DIV2 can be launched by executing "<root>/DIV2/DOSBox.exe".

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
  5. Bring the crowbar halfway to the barn before the reset.
* Takeaway: Crowbar remains where you dropped it → persistence revealed.

### 2. First Unlock

* Objective: Use persistence to open a new area.
* Player Actions:
  1. Rush to the crowbar and reach the old barn.
  2. Break open the door; discover generator parts and alien residue.
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
  2. Timer slightly shortens (e.g., 5 → 4.5 minutes) to increase tension.
* Player Actions:
  3. Finish wiring EMP device.
* Retrieve final detonator component from village blacksmith.

### 7. The Final Setup

* Objective: Assemble & transport the weapon to the field.
* Player Actions:
  1. Carefully sneak past patrols with the assembled device.
  2. Place it under the mothership.
* Ending Choice:
  1. Hit remote trigger and escape before reset, or
  2. Stay and watch as time collapses with the ship.

## Map

```
            Radio Tower
               ▲
               │
 Barn ◄── Village ──► Mechanic’s Workshop
               │
           Mothership Field

```

## TODO

- [x] Load resources on main program, not processes, to ensure they are not reloaded after loop reset.
- [x] Shimmering light should be shown based on distance to mothership, not on viewing angle.
- [ ] Reset game after countdown.
- [ ] At the beginning of each loop, it should indicate chapter name + loop number.
- [ ] Sound when showing the white light after countdown.
- [ ] Sound when interacting.
- [ ] Ticking sound on the last seconds of countdown?
- [ ] Fireflies, birds, and other simple processed to improve immersion.
- [ ] Event-driven music.