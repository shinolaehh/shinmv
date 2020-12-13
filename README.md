shinola's model viewer
======================
a modern goldsource modelviewer; with glfw and imgui as its backends
please go over to the releases tab to get a new version!

**note: this program is still very in-development. many things are placeholder, and it requires you to drag a model on top of the exe for it to run. please expect bugs and report them + give feedback in the 'issues' section!**

controls
--------
mouse:
left mouse button - rotate
right mouse button - zoom
middle mouse button - pan

keyboard:
page up - next sequence
page down - previous sequence
spacebar - pause / play sequence

home - return to default view
f - toggle texture filtering
o - view relative to the origin

features
--------
- the ability to now view sequence events, activities, and flags for bone controllers, sequences and blends
- toggle texture filtering for the models (at last!)
- uses glfw rather than the disgusting monstrosity known as glut

license
-------
this program is released under **the half-life 1 sdk license.** please check `LICENSE.md` for specific details

compiling
---------
currently, this program is *windows only and requires visual studio 2019.* however, linux support is planned and it's pretty portable besides a handful of windows functions
this program also requires glfw and imgui. the build instructions are:
**glfw:** put the `include/glfw/` folder from your win32 zip (ie `glfw-3.3.2.bin.WIN32.zip`) in this repo's `./src/include/`, and the libraries from the `lib-vc2019` folder in the glfw zip in `./src/lib/` of this repo
**imgui:** put the imgui files from `imgui-master.zip` into the `./src/include/imgui/` directory in your local copy of this repo. it also requires `imgui_impl_opengl2.xxx` and `imgui_impl_glfw.xxx` from the `examples/example_glfw_opengl2` folder **into the same folder as the other imgui files in the model viewer repo.**