#ifndef DEFINITIONS_H
#define DEFINITIONS_H

const int CELL_W = 16;     // width of graphic cell
const int CELL_H = CELL_W; // height of graphic cell

const int Z_GRID    = 999;    // Z-level of the grid
const int Z_SELEC   = 99'999; // Z-level of the selection
const int Z_PREVIEW = 88'888; // Z-level of preview tiles

enum class eLayerType
{
    None = 0,
    Graphic,
    Blocking
};

#endif // DEFINITIONS_H
