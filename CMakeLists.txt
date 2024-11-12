cmake_minimum_required(VERSION 3.6)

project(mjaron-fakeuname
    VERSION 0.0.1
    DESCRIPTION "An Linux uname stub library."
    HOMEPAGE_URL "https://github.com/mjfryc/"
)

add_library(mjaron-fakeuname SHARED
    mjaron-fakeuname.h
    mjaron-fakeuname.c
)


add_executable(mjaron-fakeuname-demo
    mjaron-fakeuname-demo.c
)
