#pragma once
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// try to send multiple wav into cartbridge
struct WaveCollection 
{
    Wave *waves;
    unsigned int count;
};
struct WaveCollection* add_WaveCollection(struct WaveCollection* pwc, const char* file);
bool save_WavesToBinary(const char *fileName, const struct WaveCollection *collection);
bool load_WavesFromBinary(const char *fileName, struct WaveCollection *collection);
void unload_WaveCollection(struct WaveCollection *collection);


struct AssetCollection 
{
    struct WaveCollection swaves;
};

// Structure pour regrouper plusieurs Wave


