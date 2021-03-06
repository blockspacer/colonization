# Colonization [Work In Progress]
Multiplayer RTS game in colonization setting.

**Engine:** Urho3D.

**Programming languages:** C++ (main library, tests, launchers) and AngelScript (client, custom victory types, *in future: AIs*), AngelScript **!=** ActionScript.

**Travis CI:** [![Build Status](https://travis-ci.org/KonstantinTomashevich/colonization.svg?branch=master)](https://travis-ci.org/KonstantinTomashevich/colonization)

**Automated code reviews:**[![Codacy Badge](https://api.codacy.com/project/badge/Grade/e8b7bf9ec5824c858619541d72830d99)](https://www.codacy.com/app/KonstantinTomashevich/colonization?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KonstantinTomashevich/colonization&amp;utm_campaign=Badge_Grade)[![Code Climate](https://codeclimate.com/github/KonstantinTomashevich/colonization/badges/gpa.svg)](https://codeclimate.com/github/KonstantinTomashevich/colonization)[![Issue Count](https://codeclimate.com/github/KonstantinTomashevich/colonization/badges/issue_count.svg)](https://codeclimate.com/github/KonstantinTomashevich/colonization)

## Features:
- [x] Districts system like Europa Universalis.
- [x] Various victory types like Civilization (victory types can be added by separate script, specified in map info xml).
- [x] Colonies evolution system (for now, evolution of farms, mines, industry, logistics and defense).
- [x] Trade areas system (district with good logistics unite to trade area and it will give you additional profit).
- [x] Diplomacy system (basic, you can declare war and send peace treaty).
- [ ] Colony level of life system.
- [ ] Natives and relations with them.
- [x] Multiplayer support (scene replication is one-directional, the server always has authority, the client responds with network messages).

## How to build
WARNING! `Lua` required. Build scripts use `lua` command.

Step 1. Build [Urho3D](https://urho3d.github.io) SDK: clone sources from [git repository](https://github.com/Urho3D/Urho3D), [build it](https://urho3d.github.io/documentation/HEAD/_building.html) and generate SDK via `make install`. Recomended Urho3D version: [this commit](https://github.com/urho3d/Urho3D/commit/8f8f0e658de42935cbc1f6928c4097eaa0cd170c), newer version are not supported.

Step 2. Set `URHO3D_HOME` environment var equal to the installed sdk dir.

Step 3. Clone repository:
```bash
git clone https://github.com/KonstantinTomashevich/colonization.git
```
Step 4. Generate build directory via CMake:
```bash
cd colonization && mkdir build && cd build && cmake ..
```
Step 5. Build and run tests.
```bash
make && make test
```
Step 6. Executables will be in `${CMAKE_BINARY_DIR}/bin` in `Tools`, `Tests` and `GameLauncher` folders.

## Screenshots
![0](https://pp.userapi.com/c847021/v847021302/7bfba/l1TjwSyYaJU.jpg)
![1](https://pp.userapi.com/c847021/v847021302/7bfc4/ZWNbo9vavus.jpg)
![2](https://pp.userapi.com/c847021/v847021302/7bfce/S-W0ZzdZeJs.jpg)
![3](https://pp.userapi.com/c847021/v847021302/7bfd8/O4WJ-ULSRww.jpg)
![4](https://pp.userapi.com/c847021/v847021302/7bfe2/VbX3OzYnIvo.jpg)
![5](https://pp.userapi.com/c847021/v847021302/7bfec/fr7lF2Y4_Rg.jpg)
![6](https://pp.userapi.com/c847021/v847021302/7bff6/btE_kUGMbBo.jpg)
