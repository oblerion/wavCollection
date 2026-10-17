#include "AssetCollection.h"
#include <stdio.h>
struct WaveCollection* add_WaveCollection(struct WaveCollection* pwc, const char* file)
{
    if (pwc == NULL)
    {
        pwc = (struct WaveCollection*)RL_MALLOC(sizeof(struct WaveCollection));
        if (pwc == NULL)
            return NULL;                     /* allocation failure */
        pwc->waves = NULL;
        pwc->count = 0;
    }

    Wave* new_waves = (Wave*)RL_REALLOC(pwc->waves,
                                        (pwc->count + 1) * sizeof(Wave));
    if (new_waves == NULL)
        return pwc;                          /* or handle failure as preferred */
    
    pwc->waves = new_waves;
    pwc->waves[pwc->count] = LoadWave(file);
    pwc->count++;

    return pwc;
}

// ------------------------------------------------------------------
// Écriture d’une collection de Wave dans un fichier binaire
// ------------------------------------------------------------------
bool save_WavesToBinary(const char *fileName, const struct WaveCollection *collection)
{
    if (fileName == NULL || collection == NULL || collection->count == 0)
        return false;
    FILE *file = fopen(fileName, "wb");
    if (file == NULL)
    {
        
    }

    // 1. Écrire le nombre de Wave
    unsigned int count = collection->count;
    if (fwrite(&count, sizeof(count), 1, file) != 1) {
        fclose(file);
        return false;
    }

    // 2. Écrire chaque Wave
    for (unsigned int i = 0; i < collection->count; i++) {
        const Wave *w = &collection->waves[i];

        unsigned int frameCount  = (unsigned int)w->frameCount;
        unsigned int sampleRate  = (unsigned int)w->sampleRate;
        unsigned int sampleSize  = (unsigned int)w->sampleSize;
        unsigned int channels    = (unsigned int)w->channels;
        unsigned int dataSize    = frameCount * channels * (sampleSize / 8);

        // Métadonnées
        if (fwrite(&frameCount, sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&sampleRate, sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&sampleSize, sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&channels,   sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&dataSize,   sizeof(unsigned int), 1, file) != 1) {
            fclose(file);
            return false;
        }

        // Données brutes
        if (dataSize > 0 && w->data != NULL) {
            if (fwrite(w->data, 1, dataSize, file) != dataSize) {
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    return true;
}

// ------------------------------------------------------------------
// Lecture d’une collection de Wave depuis un fichier binaire
// ------------------------------------------------------------------
bool load_WavesFromBinary(const char *fileName, struct WaveCollection *collection)
{
    if (fileName == NULL || collection == NULL)
        return false;

    FILE *file = fopen(fileName, "rb");
    if (file == NULL)
        return false;

    // 1. Lire le nombre de Wave
    unsigned int count = 0;
    if (fread(&count, sizeof(unsigned int), 1, file) != 1 || count == 0) {
        fclose(file);
        return false;
    }

    // Allouer le tableau de Wave
    Wave *waves = (Wave *)RL_CALLOC(count, sizeof(Wave));
    if (waves == NULL) {
        fclose(file);
        return false;
    }

    // 2. Lire chaque Wave
    for (unsigned int i = 0; i < count; i++) {
        unsigned int frameCount = 0, sampleRate = 0, sampleSize = 0, channels = 0, dataSize = 0;

        if (fread(&frameCount, sizeof(unsigned int), 1, file) != 1 ||
            fread(&sampleRate, sizeof(unsigned int), 1, file) != 1 ||
            fread(&sampleSize, sizeof(unsigned int), 1, file) != 1 ||
            fread(&channels,   sizeof(unsigned int), 1, file) != 1 ||
            fread(&dataSize,   sizeof(unsigned int), 1, file) != 1) {
            // Libérer ce qui a déjà été alloué
            for (unsigned int j = 0; j < i; j++) {
                if (waves[j].data) RL_FREE(waves[j].data);
            }
            RL_FREE(waves);
            fclose(file);
            return false;
        }

        waves[i].frameCount = frameCount;
        waves[i].sampleRate = sampleRate;
        waves[i].sampleSize = sampleSize;
        waves[i].channels   = channels;
        waves[i].data       = NULL;

        if (dataSize > 0) {
            waves[i].data = RL_MALLOC(dataSize);
            if (waves[i].data == NULL ||
                fread(waves[i].data, 1, dataSize, file) != dataSize) {
                // Nettoyage en cas d’échec
                for (unsigned int j = 0; j <= i; j++) {
                    if (waves[j].data) RL_FREE(waves[j].data);
                }
                RL_FREE(waves);
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);

    collection->waves = waves;
    collection->count = count;
    return true;
}

// ------------------------------------------------------------------
// Libération de la mémoire d’une collection
// ------------------------------------------------------------------
void unload_WaveCollection(struct WaveCollection *collection)
{
    if (collection == NULL) return;

    if (collection->waves != NULL) {
        for (unsigned int i = 0; i < collection->count; i++) {
            if (collection->waves[i].data != NULL) {
                RL_FREE(collection->waves[i].data);
            }
        }
        RL_FREE(collection->waves);
    }

    collection->waves = NULL;
    collection->count = 0;
}