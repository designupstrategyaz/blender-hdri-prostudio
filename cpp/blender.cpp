//replaced pointers to undefined structs with "void*"
#include "blender.h"
#include "image.h"

#include <stddef.h>


extern "C"
{

#include <math.h>

#define typeof(x) __typeof__(x)

#define CLAMP(a, b, c)  {           \
    if      ((a) < (b)) (a) = (b);  \
    else if ((a) > (c)) (a) = (c);  \
} (void)0

#ifdef __GNUC__
#  define UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#  define UNLIKELY(x)     (x)
#endif

#ifdef __GNUC__
#define CHECK_TYPE(var, type)  {  \
    typeof(var) *__tmp;           \
    __tmp = (type *)NULL;         \
    (void)__tmp;                  \
} (void)0
#else
#define EXPR_NOP(expr) (void)(0 ? ((void)(expr), 1) : 0)
#  define CHECK_TYPE(var, type)  { EXPR_NOP(var); }(void)0
#endif

#define SWAP(type, a, b)  {    \
    type sw_ap;                \
    CHECK_TYPE(a, type);       \
    CHECK_TYPE(b, type);       \
    sw_ap = (a);               \
    (a) = (b);                 \
    (b) = sw_ap;               \
} (void)0

typedef struct CBData {
    float r, g, b, a, pos;
    int cur;
} CBData;

typedef struct Blender::ColorBand {
    short tot, cur;
    char ipotype, ipotype_hue;
    char color_mode;
    char pad[1];

    CBData data[32];
} ColorBand;

/* **************** ColorBand ********************* */

/* colormode */
enum {
    COLBAND_BLEND_RGB   = 0,
    COLBAND_BLEND_HSV   = 1,
    COLBAND_BLEND_HSL   = 2,
};

/* interpolation */
enum {
    COLBAND_INTERP_LINEAR       = 0,
    COLBAND_INTERP_EASE         = 1,
    COLBAND_INTERP_B_SPLINE     = 2,
    COLBAND_INTERP_CARDINAL     = 3,
    COLBAND_INTERP_CONSTANT     = 4,
};

/* color interpolation */
enum {
    COLBAND_HUE_NEAR    = 0,
    COLBAND_HUE_FAR     = 1,
    COLBAND_HUE_CW      = 2,
    COLBAND_HUE_CCW     = 3,
};

#define _VA_NARGS_GLUE(x, y) x y
#define _VA_NARGS_RETURN_COUNT(\
        _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, \
        _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, \
        _33_, _34_, _35_, _36_, _37_, _38_, _39_, _40_, _41_, _42_, _43_, _44_, _45_, _46_, _47_, _48_, \
        _49_, _50_, _51_, _52_, _53_, _54_, _55_, _56_, _57_, _58_, _59_, _60_, _61_, _62_, _63_, _64_, \
        count, ...) count
#define _VA_NARGS_EXPAND(args) _VA_NARGS_RETURN_COUNT args
#define _VA_NARGS_OVERLOAD_MACRO2(name, count) name##count
#define _VA_NARGS_OVERLOAD_MACRO1(name, count) _VA_NARGS_OVERLOAD_MACRO2(name, count)
#define _VA_NARGS_OVERLOAD_MACRO(name,  count) _VA_NARGS_OVERLOAD_MACRO1(name, count)
/* --- expose for re-use --- */
/* 64 args max */
#define VA_NARGS_COUNT(...) _VA_NARGS_EXPAND((__VA_ARGS__, \
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
        48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
        16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2, 1, 0))
#define VA_NARGS_CALL_OVERLOAD(name, ...) \
        _VA_NARGS_GLUE(_VA_NARGS_OVERLOAD_MACRO(name, VA_NARGS_COUNT(__VA_ARGS__)), (__VA_ARGS__))

/* ELEM#(v, ...): is the first arg equal any others? */
/* internal helpers*/
#define _VA_ELEM2(v, a) \
       ((v) == (a))
#define _VA_ELEM3(v, a, b) \
       (_VA_ELEM2(v, a) || ((v) == (b)))
#define _VA_ELEM4(v, a, b, c) \
       (_VA_ELEM3(v, a, b) || ((v) == (c)))
#define _VA_ELEM5(v, a, b, c, d) \
       (_VA_ELEM4(v, a, b, c) || ((v) == (d)))
#define _VA_ELEM6(v, a, b, c, d, e) \
       (_VA_ELEM5(v, a, b, c, d) || ((v) == (e)))
#define _VA_ELEM7(v, a, b, c, d, e, f) \
       (_VA_ELEM6(v, a, b, c, d, e) || ((v) == (f)))
#define _VA_ELEM8(v, a, b, c, d, e, f, g) \
       (_VA_ELEM7(v, a, b, c, d, e, f) || ((v) == (g)))
#define _VA_ELEM9(v, a, b, c, d, e, f, g, h) \
       (_VA_ELEM8(v, a, b, c, d, e, f, g) || ((v) == (h)))
#define _VA_ELEM10(v, a, b, c, d, e, f, g, h, i) \
       (_VA_ELEM9(v, a, b, c, d, e, f, g, h) || ((v) == (i)))
#define _VA_ELEM11(v, a, b, c, d, e, f, g, h, i, j) \
       (_VA_ELEM10(v, a, b, c, d, e, f, g, h, i) || ((v) == (j)))
#define _VA_ELEM12(v, a, b, c, d, e, f, g, h, i, j, k) \
       (_VA_ELEM11(v, a, b, c, d, e, f, g, h, i, j) || ((v) == (k)))
#define _VA_ELEM13(v, a, b, c, d, e, f, g, h, i, j, k, l) \
       (_VA_ELEM12(v, a, b, c, d, e, f, g, h, i, j, k) || ((v) == (l)))
#define _VA_ELEM14(v, a, b, c, d, e, f, g, h, i, j, k, l, m) \
       (_VA_ELEM13(v, a, b, c, d, e, f, g, h, i, j, k, l) || ((v) == (m)))
#define _VA_ELEM15(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
       (_VA_ELEM14(v, a, b, c, d, e, f, g, h, i, j, k, l, m) || ((v) == (n)))
#define _VA_ELEM16(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) \
       (_VA_ELEM15(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n) || ((v) == (o)))
#define _VA_ELEM17(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
       (_VA_ELEM16(v, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) || ((v) == (p)))

/* reusable ELEM macro */
#define ELEM(...) VA_NARGS_CALL_OVERLOAD(_VA_ELEM, __VA_ARGS__)

enum {
    KEY_LINEAR      = 0,
    KEY_CARDINAL    = 1,
    KEY_BSPLINE     = 2,
    KEY_CATMULL_ROM = 3,
};

void key_curve_position_weights(float t, float data[4], int type)
{
    float t2, t3, fc;

    if (type == KEY_LINEAR) {
        data[0] =          0.0f;
        data[1] = -t     + 1.0f;
        data[2] =  t;
        data[3] =          0.0f;
    }
    else if (type == KEY_CARDINAL) {
        t2 = t * t;
        t3 = t2 * t;
        fc = 0.71f;

        data[0] = -fc          * t3  + 2.0f * fc          * t2 - fc * t;
        data[1] =  (2.0f - fc) * t3  + (fc - 3.0f)        * t2 + 1.0f;
        data[2] =  (fc - 2.0f) * t3  + (3.0f - 2.0f * fc) * t2 + fc * t;
        data[3] =  fc          * t3  - fc * t2;
    }
    else if (type == KEY_BSPLINE) {
        t2 = t * t;
        t3 = t2 * t;

        data[0] = -0.16666666f * t3  + 0.5f * t2   - 0.5f * t    + 0.16666666f;
        data[1] =  0.5f        * t3  - t2                        + 0.66666666f;
        data[2] = -0.5f        * t3  + 0.5f * t2   + 0.5f * t    + 0.16666666f;
        data[3] =  0.16666666f * t3;
    }
    else if (type == KEY_CATMULL_ROM) {
        t2 = t * t;
        t3 = t2 * t;
        fc = 0.5f;

        data[0] = -fc          * t3  + 2.0f * fc          * t2 - fc * t;
        data[1] =  (2.0f - fc) * t3  + (fc - 3.0f)        * t2 + 1.0f;
        data[2] =  (fc - 2.0f) * t3  + (3.0f - 2.0f * fc) * t2 + fc * t;
        data[3] =  fc          * t3  - fc * t2;
    }
}

#define BLI_MATH_DO_INLINE 1

#if BLI_MATH_DO_INLINE
#  ifdef _MSC_VER
#    define MINLINE static __forceinline
#    define MALWAYS_INLINE MINLINE
#  else
#    define MINLINE static inline
#    define MALWAYS_INLINE static inline __attribute__((always_inline)) __attribute__((unused))
#  endif
#else
#  define MINLINE
#  define MALWAYS_INLINE
#endif

MINLINE float min_ff(float a, float b)
{
    return (a < b) ? a : b;
}

MINLINE float max_ff(float a, float b)
{
    return (a > b) ? a : b;
}

MINLINE float min_fff(float a, float b, float c)
{
    return min_ff(min_ff(a, b), c);
}

MINLINE float max_fff(float a, float b, float c)
{
    return max_ff(max_ff(a, b), c);
}

#ifndef fabsf
#define fabsf(a) ((float)fabs(a))
#endif

#  define BLI_assert(a) ((void)0)

void hsv_to_rgb(float h, float s, float v, float *r, float *g, float *b)
{
    float nr, ng, nb;

    nr =        fabsf(h * 6.0f - 3.0f) - 1.0f;
    ng = 2.0f - fabsf(h * 6.0f - 2.0f);
    nb = 2.0f - fabsf(h * 6.0f - 4.0f);

    CLAMP(nr, 0.0f, 1.0f);
    CLAMP(nb, 0.0f, 1.0f);
    CLAMP(ng, 0.0f, 1.0f);

    *r = ((nr - 1.0f) * s + 1.0f) * v;
    *g = ((ng - 1.0f) * s + 1.0f) * v;
    *b = ((nb - 1.0f) * s + 1.0f) * v;
}

void hsl_to_rgb(float h, float s, float l, float *r, float *g, float *b)
{
    float nr, ng, nb, chroma;

    nr =        fabsf(h * 6.0f - 3.0f) - 1.0f;
    ng = 2.0f - fabsf(h * 6.0f - 2.0f);
    nb = 2.0f - fabsf(h * 6.0f - 4.0f);

    CLAMP(nr, 0.0f, 1.0f);
    CLAMP(nb, 0.0f, 1.0f);
    CLAMP(ng, 0.0f, 1.0f);

    chroma = (1.0f - fabsf(2.0f * l - 1.0f)) * s;

    *r = (nr - 0.5f) * chroma + l;
    *g = (ng - 0.5f) * chroma + l;
    *b = (nb - 0.5f) * chroma + l;
}

void hsv_to_rgb_v(const float hsv[3], float r_rgb[3])
{
    hsv_to_rgb(hsv[0], hsv[1], hsv[2], &r_rgb[0], &r_rgb[1], &r_rgb[2]);
}

void hsl_to_rgb_v(const float hsl[3], float r_rgb[3])
{
    hsl_to_rgb(hsl[0], hsl[1], hsl[2], &r_rgb[0], &r_rgb[1], &r_rgb[2]);
}

void rgb_to_hsv(float r, float g, float b, float *lh, float *ls, float *lv)
{
    float k = 0.0f;
    float chroma;
    float min_gb;

    if (g < b) {
        SWAP(float, g, b);
        k = -1.0f;
    }
    min_gb = b;
    if (r < g) {
        SWAP(float, r, g);
        k = -2.0f / 6.0f - k;
        min_gb = min_ff(g, b);
    }

    chroma = r - min_gb;

    *lh = fabsf(k + (g - b) / (6.0f * chroma + 1e-20f));
    *ls = chroma / (r + 1e-20f);
    *lv = r;
}

void rgb_to_hsv_v(const float rgb[3], float r_hsv[3])
{
    rgb_to_hsv(rgb[0], rgb[1], rgb[2], &r_hsv[0], &r_hsv[1], &r_hsv[2]);
}

void rgb_to_hsl(float r, float g, float b, float *lh, float *ls, float *ll)
{
    const float cmax = max_fff(r, g, b);
    const float cmin = min_fff(r, g, b);
    float h, s, l = min_ff(1.0, (cmax + cmin) / 2.0f);

    if (cmax == cmin) {
        h = s = 0.0f; // achromatic
    }
    else {
        float d = cmax - cmin;
        s = l > 0.5f ? d / (2.0f - cmax - cmin) : d / (cmax + cmin);
        if (cmax == r) {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        }
        else if (cmax == g) {
            h = (b - r) / d + 2.0f;
        }
        else {
            h = (r - g) / d + 4.0f;
        }
    }
    h /= 6.0f;

    *lh = h;
    *ls = s;
    *ll = l;
}

void rgb_to_hsl_v(const float rgb[3], float r_hsl[3])
{
    rgb_to_hsl(rgb[0], rgb[1], rgb[2], &r_hsl[0], &r_hsl[1], &r_hsl[2]);
}

static float colorband_hue_interp(
        const int ipotype_hue,
        const float mfac, const float fac,
        float h1, float h2)
{
    float h_interp;
    int mode = 0;

#define HUE_INTERP(h_a, h_b) ((mfac * (h_a)) + (fac * (h_b)))
#define HUE_MOD(h) (((h) < 1.0f) ? (h) : (h) - 1.0f)

    h1 = HUE_MOD(h1);
    h2 = HUE_MOD(h2);

    BLI_assert(h1 >= 0.0f && h1 < 1.0f);
    BLI_assert(h2 >= 0.0f && h2 < 1.0f);

    switch (ipotype_hue) {
        case COLBAND_HUE_NEAR:
        {
            if      ((h1 < h2) && (h2 - h1) > +0.5f) mode = 1;
            else if ((h1 > h2) && (h2 - h1) < -0.5f) mode = 2;
            else                                     mode = 0;
            break;
        }
        case COLBAND_HUE_FAR:
        {
            if      ((h1 < h2) && (h2 - h1) < +0.5f) mode = 1;
            else if ((h1 > h2) && (h2 - h1) > -0.5f) mode = 2;
            else                                     mode = 0;
            break;
        }
        case COLBAND_HUE_CCW:
        {
            if (h1 > h2) mode = 2;
            else         mode = 0;
            break;
        }
        case COLBAND_HUE_CW:
        {
            if (h1 < h2) mode = 1;
            else         mode = 0;
            break;
        }
    }

    switch (mode) {
        case 0:
            h_interp = HUE_INTERP(h1, h2);
            break;
        case 1:
            h_interp = HUE_INTERP(h1 + 1.0f, h2);
            h_interp = HUE_MOD(h_interp);
            break;
        case 2:
            h_interp = HUE_INTERP(h1, h2 + 1.0f);
            h_interp = HUE_MOD(h_interp);
            break;
    }

    BLI_assert(h_interp >= 0.0f && h_interp < 1.0f);

#undef HUE_INTERP
#undef HUE_MOD

    return h_interp;
}

bool BKE_colorband_evaluate(const ColorBand *coba, float in, float out[4])
{
    const CBData *cbd1, *cbd2, *cbd0, *cbd3;
    float fac;
    int ipotype;
    int a;

    if (coba == NULL || coba->tot == 0) return false;

    cbd1 = coba->data;

    ipotype = (coba->color_mode == COLBAND_BLEND_RGB) ? coba->ipotype : COLBAND_INTERP_LINEAR;

    if (coba->tot == 1) {
        out[0] = cbd1->r;
        out[1] = cbd1->g;
        out[2] = cbd1->b;
        out[3] = cbd1->a;
    }
    else if ((in <= cbd1->pos) && ELEM(ipotype, COLBAND_INTERP_LINEAR, COLBAND_INTERP_EASE)) {
        out[0] = cbd1->r;
        out[1] = cbd1->g;
        out[2] = cbd1->b;
        out[3] = cbd1->a;
    }
    else {
        CBData left, right;

        /* we're looking for first pos > in */
        for (a = 0; a < coba->tot; a++, cbd1++) {
            if (cbd1->pos > in) {
                break;
            }
        }

        if (a == coba->tot) {
            cbd2 = cbd1 - 1;
            right = *cbd2;
            right.pos = 1.0f;
            cbd1 = &right;
        }
        else if (a == 0) {
            left = *cbd1;
            left.pos = 0.0f;
            cbd2 = &left;
        }
        else {
            cbd2 = cbd1 - 1;
        }

        if ((in >= cbd1->pos) && ELEM(ipotype, COLBAND_INTERP_LINEAR, COLBAND_INTERP_EASE)) {
            out[0] = cbd1->r;
            out[1] = cbd1->g;
            out[2] = cbd1->b;
            out[3] = cbd1->a;
        }
        else {

            if (cbd2->pos != cbd1->pos) {
                fac = (in - cbd1->pos) / (cbd2->pos - cbd1->pos);
            }
            else {
                /* was setting to 0.0 in 2.56 & previous, but this
                 * is incorrect for the last element, see [#26732] */
                fac = (a != coba->tot) ? 0.0f : 1.0f;
            }

            if (ipotype == COLBAND_INTERP_CONSTANT) {
                /* constant */
                out[0] = cbd2->r;
                out[1] = cbd2->g;
                out[2] = cbd2->b;
                out[3] = cbd2->a;
            }
            else if (ipotype >= COLBAND_INTERP_B_SPLINE) {
                /* ipo from right to left: 3 2 1 0 */
                float t[4];

                if (a >= coba->tot - 1) cbd0 = cbd1;
                else cbd0 = cbd1 + 1;
                if (a < 2) cbd3 = cbd2;
                else cbd3 = cbd2 - 1;

                CLAMP(fac, 0.0f, 1.0f);

                if (ipotype == COLBAND_INTERP_CARDINAL) {
                    key_curve_position_weights(fac, t, KEY_CARDINAL);
                }
                else {
                    key_curve_position_weights(fac, t, KEY_BSPLINE);
                }

                out[0] = t[3] * cbd3->r + t[2] * cbd2->r + t[1] * cbd1->r + t[0] * cbd0->r;
                out[1] = t[3] * cbd3->g + t[2] * cbd2->g + t[1] * cbd1->g + t[0] * cbd0->g;
                out[2] = t[3] * cbd3->b + t[2] * cbd2->b + t[1] * cbd1->b + t[0] * cbd0->b;
                out[3] = t[3] * cbd3->a + t[2] * cbd2->a + t[1] * cbd1->a + t[0] * cbd0->a;
                CLAMP(out[0], 0.0f, 1.0f);
                CLAMP(out[1], 0.0f, 1.0f);
                CLAMP(out[2], 0.0f, 1.0f);
                CLAMP(out[3], 0.0f, 1.0f);
            }
            else {
                float mfac;

                if (ipotype == COLBAND_INTERP_EASE) {
                    mfac = fac * fac;
                    fac = 3.0f * mfac - 2.0f * mfac * fac;
                }

                mfac = 1.0f - fac;

                if (UNLIKELY(coba->color_mode == COLBAND_BLEND_HSV)) {
                    float col1[3], col2[3];

                    rgb_to_hsv_v(&cbd1->r, col1);
                    rgb_to_hsv_v(&cbd2->r, col2);

                    out[0] = colorband_hue_interp(coba->ipotype_hue, mfac, fac, col1[0], col2[0]);
                    out[1] = mfac * col1[1] + fac * col2[1];
                    out[2] = mfac * col1[2] + fac * col2[2];
                    out[3] = mfac * cbd1->a + fac * cbd2->a;

                    hsv_to_rgb_v(out, out);
                }
                else if (UNLIKELY(coba->color_mode == COLBAND_BLEND_HSL)) {
                    float col1[3], col2[3];

                    rgb_to_hsl_v(&cbd1->r, col1);
                    rgb_to_hsl_v(&cbd2->r, col2);

                    out[0] = colorband_hue_interp(coba->ipotype_hue, mfac, fac, col1[0], col2[0]);
                    out[1] = mfac * col1[1] + fac * col2[1];
                    out[2] = mfac * col1[2] + fac * col2[2];
                    out[3] = mfac * cbd1->a + fac * cbd2->a;

                    hsl_to_rgb_v(out, out);
                }
                else {
                    /* COLBAND_BLEND_RGB */
                    out[0] = mfac * cbd1->r + fac * cbd2->r;
                    out[1] = mfac * cbd1->g + fac * cbd2->g;
                    out[2] = mfac * cbd1->b + fac * cbd2->b;
                    out[3] = mfac * cbd1->a + fac * cbd2->a;
                }
            }
        }
    }
    return true;   /* OK */
}




#define CM_TABLE 256

typedef struct CurveMapPoint {
    float x, y;
    short flag, shorty;     /* shorty for result lookup */
} CurveMapPoint;

typedef struct CurveMap {
    short totpoint, flag;

    float range;                    /* quick multiply value for reading table */
    float mintable, maxtable;       /* the x-axis range for the table */
    float ext_in[2], ext_out[2];    /* for extrapolated curves, the direction vector */
    CurveMapPoint *curve;           /* actual curve */
    CurveMapPoint *table;           /* display and evaluate table */

    CurveMapPoint *premultable;     /* for RGB curves, premulled table */
    float premul_ext_in[2];         /* for RGB curves, premulled extrapolation vector */
    float premul_ext_out[2];
} CurveMap;

/** float rectangle. */
typedef struct rctf {
    float xmin, xmax;
    float ymin, ymax;
} rctf;

typedef struct CurveMapping {
    int flag, cur;                  /* cur; for buttons, to show active curve */
    int preset;
    int changed_timestamp;

    rctf curr, clipr;               /* current rect, clip rect (is default rect too) */

    CurveMap cm[4];                 /* max 4 builtin curves per mapping struct now */
    float black[3], white[3];       /* black/white point (black[0] abused for current frame) */
    float bwmul[3];                 /* black/white point multiply value, for speed */

    float sample[3];                /* sample values, if flag set it draws line and intersection */

    short tone;
    short pad[3];
} CurveMapping;

/* cuma->flag */
#define CUMA_EXTEND_EXTRAPOLATE 1

/* in X, out Y.
 * X is presumed to be outside first or last */
static float curvemap_calc_extend(const CurveMap *cuma, float x, const float first[2], const float last[2])
{
    if (x <= first[0]) {
        if ((cuma->flag & CUMA_EXTEND_EXTRAPOLATE) == 0) {
            /* no extrapolate */
            return first[1];
        }
        else {
            if (cuma->ext_in[0] == 0.0f)
                return first[1] + cuma->ext_in[1] * 10000.0f;
            else
                return first[1] + cuma->ext_in[1] * (x - first[0]) / cuma->ext_in[0];
        }
    }
    else if (x >= last[0]) {
        if ((cuma->flag & CUMA_EXTEND_EXTRAPOLATE) == 0) {
            /* no extrapolate */
            return last[1];
        }
        else {
            if (cuma->ext_out[0] == 0.0f)
                return last[1] - cuma->ext_out[1] * 10000.0f;
            else
                return last[1] + cuma->ext_out[1] * (x - last[0]) / cuma->ext_out[0];
        }
    }
    return 0.0f;
}

/* table should be verified */
float curvemap_evaluateF(const CurveMap *cuma, float value)
{
    float fi;
    int i;

    /* index in table */
    fi = (value - cuma->mintable) * cuma->range;
    i = (int)fi;

    /* fi is table float index and should check against table range i.e. [0.0 CM_TABLE] */
    if (fi < 0.0f || fi > CM_TABLE)
        return curvemap_calc_extend(cuma, value, &cuma->table[0].x, &cuma->table[CM_TABLE].x);
    else {
        if (i < 0) return cuma->table[0].y;
        if (i >= CM_TABLE) return cuma->table[CM_TABLE].y;

        fi = fi - (float)i;
        return (1.0f - fi) * cuma->table[i].y + (fi) * cuma->table[i + 1].y;
    }
}

/* cumapping->flag */
#define CUMA_DO_CLIP            (1 << 0)

float curvemapping_evaluateF(const CurveMapping *cumap, int cur, float value)
{
    const CurveMap *cuma = cumap->cm + cur;
    float val = curvemap_evaluateF(cuma, value);

    /* account for clipping */
    if (cumap->flag & CUMA_DO_CLIP) {
        if (val < cumap->curr.ymin)
            val = cumap->curr.ymin;
        else if (val > cumap->curr.ymax)
            val = cumap->curr.ymax;
    }

    return val;
}




MINLINE void copy_v4_v4(float r[4], const float a[4])
{
    r[0] = a[0];
    r[1] = a[1];
    r[2] = a[2];
    r[3] = a[3];
}

MINLINE void blend_color_mix_float(float dst[4], const float src1[4], const float src2[4])
{
    if (src2[3] != 0.0f) {
        /* premul over operation */
        const float t = src2[3];
        const float mt = 1.0f - t;

        dst[0] = mt * src1[0] + src2[0];
        dst[1] = mt * src1[1] + src2[1];
        dst[2] = mt * src1[2] + src2[2];
        dst[3] = mt * src1[3] + t;
    }
    else {
        /* no op */
        copy_v4_v4(dst, src1);
    }
}

MINLINE void blend_color_add_float(float dst[4], const float src1[4], const float src2[4])
{
    if (src2[3] != 0.0f) {
        /* unpremul > add > premul, simplified */
        dst[0] = src1[0] + src2[0] * src1[3];
        dst[1] = src1[1] + src2[1] * src1[3];
        dst[2] = src1[2] + src2[2] * src1[3];
        dst[3] = src1[3];
    }
    else {
        /* no op */
        copy_v4_v4(dst, src1);
    }
}

MINLINE void blend_color_sub_float(float dst[4], const float src1[4], const float src2[4])
{
    if (src2[3] != 0.0f) {
        /* unpremul > subtract > premul, simplified */
        dst[0] = max_ff(src1[0] - src2[0] * src1[3], 0.0f);
        dst[1] = max_ff(src1[1] - src2[1] * src1[3], 0.0f);
        dst[2] = max_ff(src1[2] - src2[2] * src1[3], 0.0f);
        dst[3] = src1[3];
    }
    else {
        /* no op */
        copy_v4_v4(dst, src1);
    }
}

MINLINE void blend_color_mul_float(float dst[4], const float src1[4], const float src2[4])
{
    if (src2[3] != 0.0f) {
        /* unpremul > multiply > premul, simplified */
        const float t = src2[3];
        const float mt = 1.0f - t;

        dst[0] = mt * src1[0] + src1[0] * src2[0] * src1[3];
        dst[1] = mt * src1[1] + src1[1] * src2[1] * src1[3];
        dst[2] = mt * src1[2] + src1[2] * src2[2] * src1[3];
        dst[3] = src1[3];
    }
    else {
        /* no op */
        copy_v4_v4(dst, src1);
    }
}

MINLINE void blend_color_lighten_float(float dst[4], const float src1[4], const float src2[4])
{
    if (src2[3] != 0.0f) {
        /* remap src2 to have same alpha as src1 premultiplied, take maximum of
         * src1 and src2, then blend it with src1 */
        const float t = src2[3];
        const float mt = 1.0f - t;

        /*
         * replaced
         *     t * max_ff(src1[i], src2[i] * src1[3] / t);
         * with
         *     max_ff(t * src1[i], src2[i] * src1[3]);
         * to avoid bugs when t == 0
         */
        dst[0] = mt * src1[0] + max_ff(t * src1[0], src2[0] * src1[3]);
        dst[1] = mt * src1[1] + max_ff(t * src1[1], src2[1] * src1[3]);
        dst[2] = mt * src1[2] + max_ff(t * src1[2], src2[2] * src1[3]);
        dst[3] = src1[3];
    }
    else {
        /* no op */
        copy_v4_v4(dst, src1);
    }
}

MINLINE void blend_color_darken_float(float dst[4], const float src1[4], const float src2[4])
{
    if (src2[3] != 0.0f) {
        /* remap src2 to have same alpha as src1 premultiplied, take minimum of
         * src1 and src2, then blend it with src1 */
        const float t = src2[3];
        const float mt = 1.0f - t;

        /*
         * same as in blend_color_lighten_float
         */
        dst[0] = mt * src1[0] + min_ff(t * src1[0], src2[0] * src1[3]);
        dst[1] = mt * src1[1] + min_ff(t * src1[1], src2[1] * src1[3]);
        dst[2] = mt * src1[2] + min_ff(t * src1[2], src2[2] * src1[3]);
        dst[3] = src1[3];
    }
    else {
        /* no op */
        copy_v4_v4(dst, src1);
    }
}




#define IMB_MIPMAP_LEVELS	20

typedef struct ImBuf {
    struct ImBuf *next, *prev;	/**< allow lists of ImBufs, for caches or flipbooks */

    /* dimensions */
    /** Width and Height of our image buffer.
     * Should be 'unsigned int' since most formats use this.
     * but this is problematic with texture math in imagetexture.c
     * avoid problems and use int. - campbell */
    int x, y;

    /** Active amount of bits/bitplanes */
    unsigned char planes;
    /** Number of channels in `rect_float` (0 = 4 channel default) */
    int channels;

    /* flags */
    /** Controls which components should exist. */
    int	flags;
    /** what is malloced internal, and can be freed */
    int	mall;

    /* pixels */

    /** Image pixel buffer (8bit representation):
     * - color space defaults to `sRGB`.
     * - alpha defaults to 'straight'.
     */
    unsigned int *rect;
    /** Image pixel buffer (float representation):
     * - color space defaults to 'linear' (`rec709`).
     * - alpha defaults to 'premul'.
     * \note May need gamma correction to `sRGB` when generating 8bit representations.
     * \note Formats that support higher more than 8 but channels load as floats.
     */
    float *rect_float;

    /* resolution - pixels per meter */
    double ppm[2];

    /* tiled pixel storage */
    int tilex, tiley;
    int xtiles, ytiles;
    unsigned int **tiles;

    /* zbuffer */
    /** z buffer data, original zbuffer */
    int	*zbuf;
    /** z buffer data, camera coordinates */
    float *zbuf_float;

    /* parameters used by conversion between byte and float */
    /** random dither value, for conversion from float -> byte rect */
    float dither;

    /* mipmapping */
    /** MipMap levels, a series of halved images */
    struct ImBuf *mipmap[IMB_MIPMAP_LEVELS];
    int miptot, miplevel;

    /* externally used data */
    /** reference index for ImBuf lists */
    int index;
    /** used to set imbuf to dirty and other stuff */
    int	userflags;
    //...
} ImBuf;

/**
 * \brief userflags: Flags used internally by blender for imagebuffers
 */

enum {
    /** image needs to be saved is not the same as filename */
    IB_BITMAPDIRTY =          (1 << 1),
    /** image mipmaps are invalid, need recreate */
    IB_MIPMAP_INVALID =       (1 << 2),
    /** float buffer changed, needs recreation of byte rect */
    IB_RECT_INVALID =         (1 << 3),
    /** either float or byte buffer changed, need to re-calculate display buffers */
    IB_DISPLAY_BUFFER_INVALID =   (1 << 4),
    /** image buffer is persistent in the memory and should never be removed from the cache */
    IB_PERSISTENT =               (1 << 5),
};

typedef struct MovieCacheItem {
    void *cache_owner;
    ImBuf *ibuf;
    //...
} MovieCacheItem;

struct BLI_mempool {
    void *chunks;  /* single linked list of allocated chunks */
    /* keep a pointer to the last, so we can append new chunks there
     * this is needed for iteration so we can loop over chunks in the order added */
    void *chunk_tail;

    unsigned int esize;         /* element size in bytes */
    unsigned int csize;         /* chunk size in bytes */
    unsigned int pchunk;        /* number of elements per chunk */
    unsigned int flag;
    /* keeps aligned to 16 bits */

    void *free;         /* free element list. Interleaved into chunk datas. */
    //...
};

typedef struct MovieCache {
    char name[64];

    void *hash;
    void *hashfp;
    void *cmpfp;
    void *getdatafp;

    void *getprioritydatafp;
    void *getitempriorityfp;
    void *prioritydeleterfp;

    struct BLI_mempool *keys_pool;
    struct BLI_mempool *items_pool;
    //...
} MovieCache;

typedef struct ID {
    void *next, *prev;
    struct ID *newid;
    struct Library *lib;
    /** MAX_ID_NAME. */
    char name[66];
    /**
     * LIB_... flags report on status of the datablock this ID belongs to
     * (persistent, saved to and read from .blend).
     */
    short flag;
    /**
     * LIB_TAG_... tags (runtime only, cleared at read time).
     */
    int tag;
    int us;
    int icon_id;
    int recalc;
    int pad;
    void *properties;

    /** Reference linked ID which this one overrides. */
    void *override_static;

    /**
     * Only set for datablocks which are coming from copy-on-write, points to
     * the original version of it.
     */
    struct ID *orig_id;

    void *py_instance;
} ID;

namespace Blender
{
    typedef struct Image {
        ID id;

        /** File path, 1024 = FILE_MAX. */
        char name[1024];

        /** Not written in file. */
        struct MovieCache *cache;
        //...
    } Image;
}

}




namespace Blender
{
    ImBuf *imBuf(Blender::Image *image);
    const ImBuf *c_imBuf(const Blender::Image *image);

    MovieCacheItem *item(Blender::Image *image);
    const MovieCacheItem *c_item(const Blender::Image *image);
}


bool Blender::colorbandEvaluate(const void *coba, float in, float out[4])
{
    return BKE_colorband_evaluate(reinterpret_cast<const ColorBand*>(coba), in, out);
}

void Blender::ColorRampMinimal::operator=(const ColorBand &colorRamp)
{
    _tot        = colorRamp.tot;
    _ipotype    = colorRamp.ipotype;
    _ipotypeHue = colorRamp.ipotype_hue;
    _colorMode  = colorRamp.color_mode;

    for(int i = 0; i < _tot; i++) {
        float *p = &_data[i * 5];
        const CBData &data = colorRamp.data[i];

        p[0] = data.r;
        p[1] = data.g;
        p[2] = data.b;
        p[3] = data.a;
        p[4] = data.pos;
    }
}

bool Blender::operator==(const ColorBand &colorRamp, const ColorRampMinimal &colorRampMinimal)
{
    if((colorRampMinimal._tot        != colorRamp.tot        ) ||
       (colorRampMinimal._ipotype    != colorRamp.ipotype    ) ||
       (colorRampMinimal._ipotypeHue != colorRamp.ipotype_hue) ||
       (colorRampMinimal._colorMode  != colorRamp.color_mode ))
        return false;

    for(int i = 0; i < colorRampMinimal._tot; i++) {
        const float *p = &colorRampMinimal._data[i * 5];
        const CBData &data = colorRamp.data[i];

        if((p[0] != data.r  ) ||
           (p[1] != data.g  ) ||
           (p[2] != data.b  ) ||
           (p[3] != data.a  ) ||
           (p[4] != data.pos))
            return false;
    }

    return true;
}



float Blender::curvemappingEvaluateF(const void *cumap, float value)
{
    return curvemapping_evaluateF(reinterpret_cast<const CurveMapping*>(cumap), 0, value);
}

int Blender::curveChangedTime(const void *curve)
{
    return reinterpret_cast<const CurveMapping*>(curve)->changed_timestamp;
}

float Blender::getColorChannel(const void *image, int x, int y, int c)
{
    const ImBuf *ib = c_imBuf(reinterpret_cast<const Blender::Image*>(image));
    const int channels = ib->channels;

    if(channels == 3 && c == 3)
        return 1;

    const size_t index = (y * ib->x + x) * channels + c;

    if(ib->rect)
        return ((unsigned char*)ib->rect)[index] / float(255);
    else {
        float *rectFloat = ib->rect_float;

        if(rectFloat == g_image->getPixels())
            rectFloat = g_image->pixels();


        return rectFloat[index];
    }
}

int Blender::width(const void *image)
{
    return c_imBuf(reinterpret_cast<const Blender::Image*>(image))->x;
}

int Blender::height(const void *image)
{
    return c_imBuf(reinterpret_cast<const Blender::Image*>(image))->y;
}

void Blender::blendColorAddFloat(float dst[4], const float src1[4], const float src2[4])
{
    blend_color_add_float(dst, src1, src2);
}

void Blender::blendColorSubFloat(float dst[4], const float src1[4], const float src2[4])
{
    blend_color_sub_float(dst, src1, src2);
}

void Blender::blendColorMixFloat(float dst[4], const float src1[4], const float src2[4])
{
    blend_color_mix_float(dst, src1, src2);
}

void Blender::blendColorMulFloat(float dst[4], const float src1[4], const float src2[4])
{
    blend_color_mul_float(dst, src1, src2);
}

void Blender::blendColorLightenFloat(float dst[4], const float src1[4], const float src2[4])
{
    blend_color_lighten_float(dst, src1, src2);
}

void Blender::blendColorDarkenFloat(float dst[4], const float src1[4], const float src2[4])
{
    blend_color_darken_float(dst, src1, src2);
}

float *Blender::pixels(void *image)
{
    return imBuf(reinterpret_cast<Blender::Image*>(image))->rect_float;
}

const float *Blender::c_pixels(const void *image)
{
    return c_imBuf(reinterpret_cast<const Blender::Image*>(image))->rect_float;
}

void Blender::clean(void *image)
{
    imBuf(reinterpret_cast<Blender::Image*>(image))->userflags &= ~IB_BITMAPDIRTY;
}

ImBuf *Blender::imBuf(Blender::Image *image)
{
    return item(image)->ibuf;
}

const ImBuf *Blender::c_imBuf(const Blender::Image *image)
{
    return c_item(image)->ibuf;
}

MovieCacheItem *Blender::item(Blender::Image *image)
{
    BLI_mempool *pool = image->cache->items_pool;
    return reinterpret_cast<MovieCacheItem*>(size_t(pool->free) - pool->esize);
}

const MovieCacheItem *Blender::c_item(const Blender::Image *image)
{
    const BLI_mempool *pool = image->cache->items_pool;
    return reinterpret_cast<const MovieCacheItem*>(size_t(pool->free) - pool->esize);
}
