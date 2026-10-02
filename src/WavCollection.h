#pragma once
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// try to send multiple wav into cartbridge

// Structure pour regrouper plusieurs Wave
typedef struct {
    Wave *waves;
    unsigned int count;
} WaveCollection;

bool SaveWavesToBinary(const char *fileName, const WaveCollection *collection);
bool LoadWavesFromBinary(const char *fileName, WaveCollection *collection);