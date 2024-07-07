#ifndef MAIN_H
#define MAIN_H

#include "clJobFileInterpreter.h"
#include "clJobSliceFile.h"

struct parameters {
    float power;    // laser power
    float velocity; // velocity of the laser
    float time;     // time 
    float recoatingtime; // time for the recoating the layer
    float layerPos;      // position layer 
    string slicer_input_filename; // input filename
    string abaqus_output_filename; // output filename 
};

#endif
