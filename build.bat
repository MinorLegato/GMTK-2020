@echo off
gcc main.c -O3 -s -std=c11 -march=native -lmingw32 -lglfw3 -lopengl32 -lglu32 -lgdi32 -ldsound
