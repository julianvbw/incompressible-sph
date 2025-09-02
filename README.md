# Bachelor - Simulating Incompressible Fluids

## About
This program was produced in scope of my Bachelor's Thesis on Simulating Incompressible Fluids.

## GUI and Controls
Press the button on the top left named "Simulate" to start rendering some frames of the selected scene. With the next 
three buttons a preset scene can be selected. The next area on the top bar provides a button labeled "Play", a slider 
the current frame / the total number of frames. The slider can be used to change the current frames (provided the frame 
number is greater 0) and the play button automatically moves the slider for playback. The number of total frames should 
automatically increase when the "Simulate" button is toggled.

To navigate the scene mouse and keyboard is used. Holding any mouse button allows turning the camera, while the keys 
[W], [A], [S], [D] are used to move forward, left, right and backwards respectively. Holding [Space] or [Shift] allows 
upwards or downwards movement. To toggle boundary particle rendering, press [O].

## Compiling
The code can be compiled using CMake. Note that this project requires Qt5 and OpenMP to compile.