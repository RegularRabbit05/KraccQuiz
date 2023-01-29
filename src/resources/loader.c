#include <malloc.h>
#include <stdint.h>

struct vFile {
    void *ptr;
    int size;
};

#include "mrbeast.h"
#include "therock.h"
#include "dead.h"
#include "dream.h"
#include "song.h"
#include "cursor.h"
#include "prompt.h"
#include "wrong.h"
#include "effectA.h"
#include "icon.h"
#include "cross.h"
#include "laugh.h"
#include "laughS.h"
#include "rockS.h"
#include "fail.h"
#include "deadS.h"
#include "point.h"

struct ImageLoader {
    Image *buffer;
    void *fData;
    uint32_t fps;
    uint32_t oSize;
};

void unloadAllFrames(struct ImageLoader loader) {
    for (int i = 0; i < loader.oSize; ++i) {
        UnloadImage(loader.buffer[i]);
    }
}

struct ImageLoader loadAllFrames(unsigned char *f) {
    struct ImageLoader loader;
    loader.fData = f;
    uint32_t oSize = 0;
    uint32_t fps = 0;

    //The first 4 bytes contain the FPS value of the video
    ((unsigned char*) &fps)[0] = f[0];
    ((unsigned char*) &fps)[1] = f[1];
    ((unsigned char*) &fps)[2] = f[2];
    ((unsigned char*) &fps)[3] = f[3];
    f+=(char)4;
    //After the FPS the other 4 bytes contain the amount of frames
    ((unsigned char*) &oSize)[0] = f[0];
    ((unsigned char*) &oSize)[1] = f[1];
    ((unsigned char*) &oSize)[2] = f[2];
    ((unsigned char*) &oSize)[3] = f[3];
    f+=(char)4;

    Image *buffer = malloc(sizeof (Image) * oSize);

    for (unsigned int i = 0; i < oSize; i++) {
        uint16_t frameNum = 0;
        //4 bytes contain the N of this frame (from 0 to oSize) so you can check whether everything is working fine
        ((unsigned char*) &frameNum)[0] = f[0];
        ((unsigned char*) &frameNum)[1] = f[1];
        f+=2;
        uint32_t frameSize = 0;
        //The next 4 bytes contain the size of the "encoded" frame
        ((unsigned char*) &frameSize)[0] = f[0];
        ((unsigned char*) &frameSize)[1] = f[1];
        ((unsigned char*) &frameSize)[2] = f[2];
        ((unsigned char*) &frameSize)[3] = f[3];
        f+=4;
        //Load the frame from the data
        buffer[i] = LoadImageFromMemory(".png", f, (int) frameSize);
        //Skip to the next frame
        f+=frameSize;
    }

    loader.buffer = buffer;
    loader.oSize = oSize;
    loader.fps = fps;
    return loader;
}

Sound loadSound(struct vFile f) {
    Wave data = LoadWaveFromMemory(".mp3", f.ptr, f.size);
    Sound track = LoadSoundFromWave(data);
    UnloadWave(data);
    return track;
}

Image loadImage(struct vFile f) {
    return LoadImageFromMemory(".png", f.ptr, f.size);
}

Texture loadTexture(struct vFile f) {
    Image tmp = LoadImageFromMemory(".png", f.ptr, f.size);
    Texture tex = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    return tex;
}
