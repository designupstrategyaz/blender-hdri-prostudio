// test/harness.cpp
//
// Standalone test harness for the HDR-ProStudio C++ engine.
//
// The engine normally receives Blender data through five callbacks that
// return raw pointers into Blender's internal structures. This harness
// supplies synthetic stand-ins with the same memory layout, so the whole
// border / render / composite pipeline can be exercised and its output
// captured outside Blender.
//
// Purpose: a deterministic golden-output baseline, so render optimizations
// (Phase 2) can be proven not to change a single pixel.
//
// Build & run:  sh test/build_and_run.sh
// Manual build: g++ -O2 -std=c++11 -pthread -I cpp test/harness.cpp cpp/*.cpp -o test/harness
//
// Scope note: this first harness exercises FLAT-content lights only, which
// fully covers the border/render/composite pipeline that the bounding-box
// optimization touches. Gradient and image content lights are a planned
// extension.

#include "plugin.h"

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------
// Minimal mirrors of the Blender structures the engine's callbacks return.
// Layouts copied from cpp/blender.cpp. Used only by this harness; the engine
// receives them as void* and reinterprets them with its own definitions, so
// only the memory layout has to match.
// ---------------------------------------------------------------------------

struct H_ID {
    void *next, *prev;
    void *newid;
    void *lib;
    char name[66];
    short flag;
    int tag;
    int us;
    int icon_id;
    int recalc;
    int pad;
    void *properties;
    void *override_static;
    void *orig_id;
    void *py_instance;
};

struct H_ImBuf {
    void *next, *prev;
    int x, y;
    unsigned char planes;
    int channels;
    int flags;
    int mall;
    unsigned int *rect;
    float *rect_float;
    double ppm[2];
    int tilex, tiley;
    int xtiles, ytiles;
    unsigned int **tiles;
    int *zbuf;
    float *zbuf_float;
    float dither;
    void *mipmap[20];
    int miptot, miplevel;
    int index;
    int userflags;
};

struct H_MovieCacheItem {
    void *cache_owner;
    H_ImBuf *ibuf;
};

struct H_BLI_mempool {
    void *chunks;
    void *chunk_tail;
    unsigned int esize;
    unsigned int csize;
    unsigned int pchunk;
    unsigned int flag;
    void *free;
};

struct H_MovieCache {
    char name[64];
    void *hash, *hashfp, *cmpfp, *getdatafp;
    void *getprioritydatafp, *getitempriorityfp, *prioritydeleterfp;
    H_BLI_mempool *keys_pool;
    H_BLI_mempool *items_pool;
};

struct H_Image {
    H_ID id;
    char name[1024];
    H_MovieCache *cache;
};

struct H_CBData {
    float r, g, b, a, pos;
    int cur;
};

struct H_ColorBand {
    short tot, cur;
    char ipotype, ipotype_hue;
    char color_mode;
    char pad[1];
    H_CBData data[32];
};

struct H_CurveMapPoint {
    float x, y;
    short flag, shorty;
};

struct H_CurveMap {
    short totpoint, flag;
    float range;
    float mintable, maxtable;
    float ext_in[2], ext_out[2];
    H_CurveMapPoint *curve;
    H_CurveMapPoint *table;
    H_CurveMapPoint *premultable;
    float premul_ext_in[2];
    float premul_ext_out[2];
};

struct H_rctf { float xmin, xmax, ymin, ymax; };

struct H_CurveMapping {
    int flag, cur;
    int preset;
    int changed_timestamp;
    H_rctf curr, clipr;
    H_CurveMap cm[4];
    float black[3], white[3];
    float bwmul[3];
    float sample[3];
    short tone;
    short pad[3];
};

// ---------------------------------------------------------------------------
// Synthetic scene
// ---------------------------------------------------------------------------

static const int IMG_W = 256;
static const int IMG_H = 128;

static std::vector<float> g_pixels;          // image float buffer (RGBA)
static H_ImBuf            g_imbuf;
static H_MovieCacheItem   g_item;
static H_BLI_mempool      g_pool;
static H_MovieCache       g_cache;
static H_Image            g_imageData;

static H_CurveMapPoint    g_curveTable[257];  // CM_TABLE (256) + 1
static H_CurveMapping     g_curve;
static H_ColorBand        g_colorBand;

static void buildImage()
{
    g_pixels.assign((size_t)IMG_W * IMG_H * 4, 0.0f);
    for(int i = 0; i < IMG_W * IMG_H; i++) {
        g_pixels[i * 4 + 0] = 0.05f;
        g_pixels[i * 4 + 1] = 0.05f;
        g_pixels[i * 4 + 2] = 0.05f;
        g_pixels[i * 4 + 3] = 1.0f;
    }

    memset(&g_imbuf, 0, sizeof(g_imbuf));
    g_imbuf.x = IMG_W;
    g_imbuf.y = IMG_H;
    g_imbuf.channels = 4;
    g_imbuf.rect = 0;                       // force the float-buffer path
    g_imbuf.rect_float = g_pixels.data();

    g_item.cache_owner = 0;
    g_item.ibuf = &g_imbuf;

    memset(&g_pool, 0, sizeof(g_pool));
    g_pool.esize = sizeof(H_MovieCacheItem);
    // the engine computes the active item as (free - esize); make that &g_item
    g_pool.free = (void*)((size_t)&g_item + g_pool.esize);

    memset(&g_cache, 0, sizeof(g_cache));
    g_cache.items_pool = &g_pool;

    memset(&g_imageData, 0, sizeof(g_imageData));
    g_imageData.cache = &g_cache;
}

static void buildCurve()
{
    // constant lookup table: luminosity 1.0 across the whole range
    for(int i = 0; i <= 256; i++) {
        g_curveTable[i].x = (float)i / 256.0f;
        g_curveTable[i].y = 1.0f;
        g_curveTable[i].flag = 0;
        g_curveTable[i].shorty = 0;
    }

    memset(&g_curve, 0, sizeof(g_curve));
    for(int c = 0; c < 4; c++) {
        g_curve.cm[c].totpoint = 257;
        g_curve.cm[c].flag = 0;
        g_curve.cm[c].mintable = 0.0f;
        g_curve.cm[c].maxtable = 1.0f;
        g_curve.cm[c].range = 256.0f;       // CM_TABLE / (maxtable - mintable)
        g_curve.cm[c].table = g_curveTable;
        g_curve.cm[c].curve = g_curveTable;
    }
}

static void buildColorBand()
{
    memset(&g_colorBand, 0, sizeof(g_colorBand));
    g_colorBand.tot = 2;
    g_colorBand.data[0].r = 0; g_colorBand.data[0].g = 0; g_colorBand.data[0].b = 0;
    g_colorBand.data[0].a = 1; g_colorBand.data[0].pos = 0;
    g_colorBand.data[1].r = 1; g_colorBand.data[1].g = 1; g_colorBand.data[1].b = 1;
    g_colorBand.data[1].a = 1; g_colorBand.data[1].pos = 1;
}

// ---------------------------------------------------------------------------
// Engine callbacks
// ---------------------------------------------------------------------------

static void *cb_image()               { return &g_imageData; }
static void *cb_curveMappingFlat(int)  { return &g_curve; }
static void *cb_curveMappingImage(int) { return &g_curve; }
static void *cb_colorRamp(int)         { return &g_colorBand; }
static void *cb_lightImage(int)        { return 0; }   // no image-content lights

// ---------------------------------------------------------------------------
// Scene helper
// ---------------------------------------------------------------------------

static void addFlatLight(int x, int y, int w, int h,
                         float r, float g, float b, BlendMode blend)
{
    float color[3] = { r, g, b };
    addLight(
        true,        // enabled
        false,       // solo
        blend,       // blendMode
        1.0f,        // brightnessColor
        1.0f,        // brightnessOpacity
        0.0f, 0.0f,  // curveX, curveY
        x, y,        // position
        w, h,        // width, height
        0,           // angle
        true,        // isEllipse
        3,           // sides
        0.15f,       // cornerRadius
        Flat,        // contentType
        color,
        Linear,      // falloffType
        0,           // gradientAngle
        0.0f, 0.0f,  // radialX, radialY
        1.0f, 1.0f   // radialWidth, radialHeight
    );
}

// ---------------------------------------------------------------------------

static void sample(const char *label, int x, int y)
{
    const size_t i = ((size_t)y * IMG_W + x) * 4;
    printf("  %-22s (%3d,%3d) = %.5f %.5f %.5f %.5f\n",
           label, x, y,
           g_pixels[i + 0], g_pixels[i + 1], g_pixels[i + 2], g_pixels[i + 3]);
}

int main()
{
    buildImage();
    buildCurve();
    buildColorBand();

    init(cb_image, cb_curveMappingFlat, cb_curveMappingImage,
         cb_colorRamp, cb_lightImage);
    initImage();

    // three flat lights, moderate positions
    addFlatLight(128, 64, 50, 50, 1.0f, 0.3f, 0.2f, Mix);
    addFlatLight( 80, 50, 40, 40, 0.2f, 1.0f, 0.3f, Add);
    addFlatLight(180, 80, 60, 45, 0.3f, 0.4f, 1.0f, Mix);

    createFinalImage();

    // deterministic checksum of the composited buffer
    double sum = 0.0;
    uint64_t hash = 1469598103934665603ULL;            // FNV-1a offset basis
    const unsigned char *bytes = (const unsigned char*)g_pixels.data();
    const size_t nbytes = g_pixels.size() * sizeof(float);
    for(size_t i = 0; i < nbytes; i++) {
        hash ^= bytes[i];
        hash *= 1099511628211ULL;                      // FNV-1a prime
    }
    for(size_t i = 0; i < g_pixels.size(); i++)
        sum += g_pixels[i];

    printf("=== HDR-ProStudio engine test harness ===\n");
    printf("  image      : %dx%d RGBA float\n", IMG_W, IMG_H);
    printf("  lights     : 3 (flat content)\n");
    printf("  threads    : engine-decided\n");
    printf("--- GOLDEN OUTPUT ---\n");
    printf("  pixel sum  : %.6f\n", sum);
    printf("  byte hash  : %016llx\n", (unsigned long long)hash);
    sample("light 1 centre",  128, 64);
    sample("light 2 centre",   80, 50);
    sample("light 3 centre",  180, 80);
    sample("background",      230, 20);
    sample("between lights",  104, 57);

    clear();
    deinit();

    printf("--- harness finished cleanly ---\n");
    return 0;
}
