#pragma once

// Grid variables
#define GRID_SIZE 100
#define CELL_SIZE 2
#define GRID_MIN_X 0
#define GRID_MIN_Z 0
#define TERRAIN_HEIGHT 15

// Texture variables
#define WATER_FRACTION 1.0 / 6.0
#define GRASS_FRACTION 1.0 / 4.0
#define GRASS_ROCK_BLEND_FRACTION 5.0 / 16.0
#define ROCK_FRACTION 3.0 / 4.0
#define ROCK_SNOW_BLEND_FRACTION 13.0 / 16.0

// Tessellation variables
#define HIGH_TESS_LEVEL 3
#define LOW_TESS_LEVEL 1
#define TESS_DISTANCE_MIN 0
#define TESS_DISTANCE_MAX GRID_SIZE * CELL_SIZE
#define TESS_MUL_LEVEL_1 1.5
#define TESS_MUL_LEVEL_2 2.0
#define TESS_MUL_LEVEL_3 2.5
#define TESS_MUL_LEVEL_4 3.0
#define TESS_GRAD_LEVEL_1 1
#define TESS_GRAD_LEVEL_2 2
#define TESS_GRAD_LEVEL_3 5
#define TESS_GRAD_LEVEL_4 8
#define ENABLE_GRADIENT_TESSELATION_MULTIPLIER 1 // 1 for true 0 for false

float verts[GRID_SIZE * GRID_SIZE * 3];
GLushort elems[GRID_SIZE * GRID_SIZE * 4];