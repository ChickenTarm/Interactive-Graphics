TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    rasterize.cpp \
    tiny_obj_loader.cpp \
    triangle.cpp \
    vec4.cpp \
    mat4.cpp \
    camera.cpp

HEADERS += \
    tiny_obj_loader.h \
    triangle.h \
    vec4.h \
    mat4.h \
    camera.h
