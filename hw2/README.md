# HW2

Yibin Ye 

115638220

yibin.ye@stonybrook.edu

## Overview

- Implemented a sample modern OpenGL program with GLFW as the windowing toolkit. 
- Implemented some basic geometries, including lines, triangles, and circles. Circles are implemented with tessellation shaders. 

## Dependencies

- OpenGL:
```bash
sudo add-apt-repository ppa:kisak/kisak-mesa
sudo apt update
sudo apt-get dist-upgrade
sudo reboot
```
- [GLAD](https://glad.dav1d.de/)
  - Configuration w.r.t. results of `sudo glxinfo | grep "OpenGL`
  - Command `glxinfo` needs `mesa-utils`
- Remaining dependencies could be installed via apt:
```bash
apt install libopencv-dev libglm-dev libglew-dev libglfw3-dev mesa-utils libx11-dev libxi-dev libxrandr-dev
```

## Compile & Run

- Run inside this directory, i.e., `hw2/`: 
```bash
mkdir build
cd build
cmake -DMAKE_BUILD_TYPE=Release ..
make 
cd ..
./build/hw2
```

## Features Implemented

- [x] 1. Bouncing Ball
  - [x] Creation
  - [x] Dynamically reading config file
  - [x] Movement
  - [x] Collison detection
- [x] 2. 4+ Bouncing Balls
- [x] 3. Bouncing Face
  - [x] Creation
  - [x] Dynamically reading config file
  - [x] Movement
  - [x] Collison detection
  - [x] Generation Evolution
- [x] 4. More Bouncing Faces
  - [x] 8+ bouncing faces
  - [x] 16+ bonucing faces
- [ ] 5. BONUS
  - [ ] Please specify

## Usage

- The user must enter either mode 1 or mode 3 to start a generation. 
- To adjust window size, modify `kWindowWidth` and/or `kWindowHeight` in app.h.

## Appendix
- The `config.txt` file in `etc/` should contain exactly one line, which contains
three space-separated floating point numbers, separately denoting $r$, $v_{x}$, and $v_{y}$. A positive $v_{x}$
means to move toward the right, and a positive $v_{y}$ means to move upwards. A negative value
means to move in the opposite direction.
- Motion may appear too fast or too slow on screen, so $\leq\langle.001,.001\rangle \geq |v|\leq\langle.009,.009\rangle$ is suggested.