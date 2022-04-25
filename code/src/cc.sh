#!/bin/bash
g++ \
-O3 -Wall -std=c++11 \
main.cpp Image.cpp SuperPixels.cpp Image_ppm.cpp \
-o Superpixels