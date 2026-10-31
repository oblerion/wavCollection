#include "AssetCollection.h"
#include <stdio.h>
struct WaveCollection* init_WaveCollection()
{
    struct WaveCollection* wc = NULL;
    wc = (struct WaveCollection*)RL_MALLOC(sizeof(struct WaveCollection));
                       /* allocation failure */
    if(wc!=NULL)
    {
        wc->waves = NULL;
        wc->count = 0;
    }
    return wc;
}
struct WaveCollection* add_WaveCollection(struct WaveCollection* pwc, const char* file)
{
    if (pwc == NULL)
    {
        pwc = init_WaveCollection();
        if (pwc == NULL)
            return NULL;
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

struct header_wave
{
    unsigned int frameCount;
    unsigned int sampleRate;
    unsigned int sampleSize;
    unsigned int channels;
    unsigned int dataSize;
};
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

        struct header_wave hw = {
            (unsigned int)w->frameCount,
            (unsigned int)w->sampleRate,
            (unsigned int)w->sampleSize,
            (unsigned int)w->channels,
            w->frameCount * w->channels * (w->sampleSize / 8)
        };


    /*     unsigned int frameCount  = (unsigned int)w->frameCount;
        unsigned int sampleRate  = (unsigned int)w->sampleRate;
        unsigned int sampleSize  = (unsigned int)w->sampleSize;
        unsigned int channels    = (unsigned int)w->channels;
        unsigned int dataSize    = frameCount * channels * (sampleSize / 8); */

        // Métadonnées
/*         if (fwrite(&frameCount, sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&sampleRate, sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&sampleSize, sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&channels,   sizeof(unsigned int), 1, file) != 1 ||
            fwrite(&dataSize,   sizeof(unsigned int), 1, file) != 1) {
            fclose(file);
            return false;
        } */

        if(fwrite(&hw, sizeof(struct header_wave),1,file) !=1)
        {
            fclose(file);
            return false;
        }

        // Données brutes
        if (hw.dataSize > 0 && w->data != NULL) {
            if (fwrite(w->data, 1, hw.dataSize, file) != hw.dataSize) {
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
struct WaveCollection* load_WavesFromBinary(const char *fileName)
{
    if (fileName == NULL)
        return NULL;

    FILE *file = fopen(fileName, "rb");
    if (file == NULL)
        return NULL;

    // 1. Lire le nombre de Wave
    unsigned int count = 0;
    if (fread(&count, sizeof(unsigned int), 1, file) != 1 || count == 0) {
        fclose(file);
        return NULL;
    }

    // Allouer le tableau de Wave
    Wave *waves = (Wave *)RL_CALLOC(count, sizeof(Wave));
    if (waves == NULL) {
        fclose(file);
        return NULL;
    }

    // 2. Lire chaque Wave
    for (unsigned int i = 0; i < count; i++) {
        struct header_wave hw = {0};
        //unsigned int frameCount = 0, sampleRate = 0, sampleSize = 0, channels = 0, dataSize = 0;

 /*        if (fread(&frameCount, sizeof(unsigned int), 1, file) != 1 ||
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
            return NULL;
        } */

        if (fread(&hw, sizeof(struct header_wave), 1, file))
        {
            // Libérer ce qui a déjà été alloué
            for (unsigned int j = 0; j < i; j++) {
                if (waves[j].data) RL_FREE(waves[j].data);
            }
            RL_FREE(waves);
            fclose(file);
            return NULL;
        }

        waves[i].frameCount = hw.frameCount;
        waves[i].sampleRate = hw.sampleRate;
        waves[i].sampleSize = hw.sampleSize;
        waves[i].channels   = hw.channels;
        waves[i].data       = NULL;

        if (hw.dataSize > 0) {
            waves[i].data = RL_MALLOC(hw.dataSize);
            if (waves[i].data == NULL ||
                fread(waves[i].data, 1, hw.dataSize, file) != hw.dataSize) {
                // Nettoyage en cas d’échec
                for (unsigned int j = 0; j <= i; j++) {
                    if (waves[j].data) RL_FREE(waves[j].data);
                }
                RL_FREE(waves);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    struct WaveCollection *collection = init_WaveCollection();
    collection->waves = waves;
    collection->count = count;
    return collection;
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