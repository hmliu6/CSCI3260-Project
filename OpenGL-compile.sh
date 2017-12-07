# /bin/bash

g++ \
  -framework Cocoa \
  -framework OpenGL \
  -framework IOKit \
  -framework GLUT \
  -framework GLUI \
  -o demo main.cpp Library/*.cpp \
  -I include \
  -I /sw/include \
  -I /usr/local/include \
  /usr/local/Cellar/glfw/3.2.1/lib/libglfw.3.dylib \
  /usr/local/Cellar/glew/2.1.0/lib/libGLEW.2.1.0.dylib \
  -Wno-deprecated && ./demo
