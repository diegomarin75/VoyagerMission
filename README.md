# Voyager Mission

**Spaceflight simulator on the solar system**

![marsandphobos](./screenshots/mars_and_phobos.jpg)

This is a spaceflight simulator on the solar system.
It is created entirely using C languaje and works on linux.

The application uses old style computer graphics, al rendering and 3D graphics are made with CPU computing all pixels one by one
(I mean, it does not use OpenGL or other 3D graphic libraries). 
It only uses svgalib to access screen surface.

The simulation includes 71 celestial bodies counting planets and satellites. 20 of them are rendered with rectangular surface images available on internet, which are rendered on spheres.
The background is also filled with stars comming from a 1600 star catalog.

The simulation includes a spacecraft that is modelled as a a set of points acting as a rigid solid. Some of the points of the spacecraft have engines attached on them that produce propulsion or rotation of the spacecraft. This is controlled with the keyboard.

On the screen the application displays the following things:

- Outside window: 
  
  The visible objects on the rendered universe through the spacefraft cockpit window are displated here, this includes, planets, satellites and stars.

- Spacecraft cockpit panels:
  - Spacecraft orientation plus velocity and acceleration vectors
  - Solar system 2D proyection map, showing nearby celestial bodies 
  - Spacecraft state information (orientation, velocity, accereration, torque, angular monentum and velocity)
  - Power of all the engines
  - Navigation computer console

The application is not fully finished, as there are planned features not implemented, however the simulation is playable as you can see on screenshots below.
They show the main menu of the application plus some scenes of the spacecraft orbiting Earth and Mars.

## Screenshots

Main menu:

![mainmenu](./screenshots/main_menu.jpg)

Orbiting Earth:

![orbitingearth](./screenshots/orbiting_earth.jpg)

Earth and Moon:

![earthandmoon](./screenshots/earth_and_moon.jpg)

Mars and Phobos:

![marsandphobos](./screenshots/mars_and_phobos.jpg)
