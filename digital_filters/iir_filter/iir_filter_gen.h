/**
 * @file      iir_filter_gen.h
 * @prefix    IIRFILT
 * @author    Simone Ruffini [simone.ruffini@squadracorse.com
 *                            simone.ruffini.work@gmail.com]
 * @date      mar 19 dic 2023, 16:54:43, CET
 *
 * @brief     Iinfinite Impulse Response digital-filter generator header
 *
 * @note      Refactoring/Overhaul from https://github.com/wooters/miniDSP/blob/master/biquad.c
 * @license   Licensed under "THE BEER-WARE LICENSE", Revision 69 (see LICENSE)
 *
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IIRFILT_GEN_
#define _IIRFILT_GEN_

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

/* Exported constants --------------------------------------------------------*/

#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Exported macros -----------------------------------------------------------*/

/* Internal macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* filter types */
enum IIRFILT_TYPE {
    IIRFILT_LPF,   /*!< low pass filter */
    IIRFILT_HPF,   /*!< High pass filter */
    IIRFILT_BPF,   /*!< band pass filter */
    IIRFILT_NOTCH, /*!< Notch Filter */
    IIRFILT_PEQ,   /*!< Peaking band EQ filter */
    IIRFILT_LSH,   /*!< Low shelf filter */
    IIRFILT_HSH    /*!< High shelf filter */
};

enum IIRFILT_ORDER {
    IIRFILT_FIRST_ORDER  = 1, /*!< IIR FILTER First Order Implementation */
    IIRFILT_SECOND_ORDER = 2  /*!< IIR FILTER Second Order Implementation */
};

#define __IS_IIRFILT_TYPE(FILTER_TYPE)    \
    (FILTER_TYPE == IIRFILT_LPF)     ? 1U \
    : (FILTER_TYPE == IIRFILT_HPF)   ? 1U \
    : (FILTER_TYPE == IIRFILT_BPF)   ? 1U \
    : (FILTER_TYPE == IIRFILT_NOTCH) ? 1U \
    : (FILTER_TYPE == IIRFILT_PEQ)   ? 1U \
    : (FILTER_TYPE == IIRFILT_LSH)   ? 1U \
    : (FILTER_TYPE == IIRFILT_HSH)   ? 1U \
                                     : 0U

#define __IS_IIRFILT_ORDER(FILTER_ORDER) \
    (FILTER_ORDER == IIRFILT_FIRST_ORDER) ? 1U : (FILTER_ORDER == IIRFILT_SECOND_ORDER) ? 1U : 0U

#define IIRFILT_DECL_FILTER_HANDLE(smpl_t, smpl_t_short) \
    struct IIRFILT_Init_##smpl_t_short {                 \
        enum IIRFILT_TYPE filt_type;                     \
        enum IIRFILT_ORDER filt_order;                   \
        smpl_t dbGain;                                   \
        smpl_t freq;                                     \
        smpl_t srate;                                    \
        smpl_t bandwidth;                                \
    };                                                   \
    struct IIRFILT_Handle_##smpl_t_short {               \
        struct IIRFILT_Init_##smpl_t_short *Init;        \
        smpl_t a[5];                                     \
        smpl_t x[2];                                     \
        smpl_t y[2];                                     \
    }

/* sets up a BiQuad Filter */
/* Note that dbGain is only used when the type is LSH or HSH */
#define IIRFILT_DECL_INIT_FN(smpl_t, smpl_t_short)                                 \
    int IIRFILT_Init_##smpl_t_short(struct IIRFILT_Handle_##smpl_t_short *hfilt) { \
        if (!hfilt)                                                                \
            return (-1);                                                           \
        if (!hfilt->Init || !hfilt->a || !hfilt->x || !hfilt->y)                   \
            return (-1);                                                           \
        if (!__IS_IIRFILT_TYPE(hfilt->Init->filt_type))                            \
            return (-1);                                                           \
        if (!__IS_IIRFILT_ORDER(hfilt->Init->filt_order))                          \
            return (-1);                                                           \
        int ret = 0;                                                               \
        switch (hfilt->Init->filt_order) {                                         \
            case IIRFILT_FIRST_ORDER:                                              \
                ret = IIRFILT_OnePoleInit_##smpl_t_short(hfilt);                   \
                break;                                                             \
            case IIRFILT_SECOND_ORDER:                                             \
                ret = IIRFILT_BiQuadInit_##smpl_t_short(hfilt);                    \
                break;                                                             \
            default:                                                               \
                ret = -1;                                                          \
                break;                                                             \
        }                                                                          \
        return ret;                                                                \
    }

#define IIRFILT_DECL_FILT_FN(smpl_t, smpl_t_short)                                                       \
    smpl_t IIRFILT_Filt_##smpl_t_short(struct IIRFILT_Handle_##smpl_t_short *hfilt, smpl_t new_sample) { \
        smpl_t ret;                                                                                      \
        switch (hfilt->Init->filt_order) {                                                               \
            case IIRFILT_FIRST_ORDER:                                                                    \
                ret = IIRFILT_OnePoleFilt_##smpl_t_short(hfilt, new_sample);                             \
                break;                                                                                   \
            case IIRFILT_SECOND_ORDER:                                                                   \
                ret = IIRFILT_BiQuadFilt_##smpl_t_short(hfilt, new_sample);                              \
                break;                                                                                   \
            default:                                                                                     \
                ret = -1;                                                                                \
                break;                                                                                   \
        }                                                                                                \
        return ret;                                                                                      \
    }

#define IIRFILT_DECL_ONEPOLEINIT_FN(smpl_t, smpl_t_short)                                 \
    int IIRFILT_OnePoleInit_##smpl_t_short(struct IIRFILT_Handle_##smpl_t_short *hfilt) { \
        return 0;                                                                         \
    }

#define IIRFILT_DECL_ONEPOLEFILT_FN(smpl_t, smpl_t_short)                                                       \
    smpl_t IIRFILT_OnePoleFilt_##smpl_t_short(struct IIRFILT_Handle_##smpl_t_short *hfilt, smpl_t new_sample) { \
        return 0;                                                                                               \
    }

#define IIRFILT_DECL_BIQUADINIT_FN(smpl_t, smpl_t_short)                                 \
    int IIRFILT_BiQuadInit_##smpl_t_short(struct IIRFILT_Handle_##smpl_t_short *hfilt) { \
        smpl_t a[3], b[3];                                                               \
        smpl_t A, omega, sn, cs, alpha, beta;                                            \
                                                                                         \
        /* setup variables */                                                            \
        A     = pow(10, hfilt->Init->dbGain / 40);                                       \
        omega = 2 * M_PI * hfilt->Init->freq / hfilt->Init->srate;                       \
        sn    = sin(omega);                                                              \
        cs    = cos(omega);                                                              \
        alpha = sn * sinh(M_LN2 / 2 * hfilt->Init->bandwidth * omega / sn);              \
        beta  = sqrt(A + A);                                                             \
                                                                                         \
        switch (hfilt->Init->filt_type) {                                                \
            case IIRFILT_LPF:                                                            \
                b[0] = (1 - cs) / 2;                                                     \
                b[1] = 1 - cs;                                                           \
                b[2] = (1 - cs) / 2;                                                     \
                a[0] = 1 + alpha;                                                        \
                a[1] = -2 * cs;                                                          \
                a[2] = 1 - alpha;                                                        \
                break;                                                                   \
            case IIRFILT_HPF:                                                            \
                b[0] = (1 + cs) / 2;                                                     \
                b[1] = -(1 + cs);                                                        \
                b[2] = (1 + cs) / 2;                                                     \
                a[0] = 1 + alpha;                                                        \
                a[1] = -2 * cs;                                                          \
                a[2] = 1 - alpha;                                                        \
                break;                                                                   \
            case IIRFILT_BPF:                                                            \
                b[0] = alpha;                                                            \
                b[1] = 0;                                                                \
                b[2] = -alpha;                                                           \
                a[0] = 1 + alpha;                                                        \
                a[1] = -2 * cs;                                                          \
                a[2] = 1 - alpha;                                                        \
                break;                                                                   \
            case IIRFILT_NOTCH:                                                          \
                b[0] = 1;                                                                \
                b[1] = -2 * cs;                                                          \
                b[2] = 1;                                                                \
                a[0] = 1 + alpha;                                                        \
                a[1] = -2 * cs;                                                          \
                a[2] = 1 - alpha;                                                        \
                break;                                                                   \
            case IIRFILT_PEQ:                                                            \
                b[0] = 1 + (alpha * A);                                                  \
                b[1] = -2 * cs;                                                          \
                b[2] = 1 - (alpha * A);                                                  \
                a[0] = 1 + (alpha / A);                                                  \
                a[1] = -2 * cs;                                                          \
                a[2] = 1 - (alpha / A);                                                  \
                break;                                                                   \
            case IIRFILT_LSH:                                                            \
                b[0] = A * ((A + 1) - (A - 1) * cs + beta * sn);                         \
                b[1] = 2 * A * ((A - 1) - (A + 1) * cs);                                 \
                b[2] = A * ((A + 1) - (A - 1) * cs - beta * sn);                         \
                a[0] = (A + 1) + (A - 1) * cs + beta * sn;                               \
                a[1] = -2 * ((A - 1) + (A + 1) * cs);                                    \
                a[2] = (A + 1) + (A - 1) * cs - beta * sn;                               \
                break;                                                                   \
            case IIRFILT_HSH:                                                            \
                b[0] = A * ((A + 1) + (A - 1) * cs + beta * sn);                         \
                b[1] = -2 * A * ((A - 1) + (A + 1) * cs);                                \
                b[2] = A * ((A + 1) + (A - 1) * cs - beta * sn);                         \
                a[0] = (A + 1) - (A - 1) * cs + beta * sn;                               \
                a[1] = 2 * ((A - 1) - (A + 1) * cs);                                     \
                a[2] = (A + 1) - (A - 1) * cs - beta * sn;                               \
                break;                                                                   \
            default:                                                                     \
                return -1;                                                               \
        }                                                                                \
                                                                                         \
        /* precompute the coefficients */                                                \
        hfilt->a[0] = b[0] / a[0];                                                       \
        hfilt->a[1] = b[1] / a[0];                                                       \
        hfilt->a[2] = b[2] / a[0];                                                       \
        hfilt->a[3] = a[1] / a[0];                                                       \
        hfilt->a[4] = a[2] / a[0];                                                       \
                                                                                         \
        /* zero initial samples */                                                       \
        hfilt->x[0] = hfilt->x[1] = 0;                                                   \
        hfilt->y[0] = hfilt->y[1] = 0;                                                   \
                                                                                         \
        return 0;                                                                        \
    }

#define IIRFILT_DECL_BIQUADFILT_FN(smpl_t, smpl_t_short)                                                       \
    smpl_t IIRFILT_BiQuadFilt_##smpl_t_short(struct IIRFILT_Handle_##smpl_t_short *hfilt, smpl_t new_sample) { \
        smpl_t result;                                                                                         \
                                                                                                               \
        /* compute result */                                                                                   \
        result = hfilt->a[0] * new_sample + hfilt->a[1] * hfilt->x[0] + hfilt->a[2] * hfilt->x[1] -            \
                 hfilt->a[3] * hfilt->y[0] - hfilt->a[4] * hfilt->y[1];                                        \
                                                                                                               \
        /* shift x1 to x2, sample to x1 */                                                                     \
        hfilt->x[1] = hfilt->x[0];                                                                             \
        hfilt->x[0] = new_sample;                                                                              \
                                                                                                               \
        /* shift y1 to y2, result to y1 */                                                                     \
        hfilt->y[1] = hfilt->y[0];                                                                             \
        hfilt->y[0] = result;                                                                                  \
                                                                                                               \
        return result;                                                                                         \
    }

#define IIRFILT_DECL_FILTER(smpl_t, smpl_t_short)      \
    IIRFILT_DECL_FILTER_HANDLE(smpl_t, smpl_t_short);  \
    IIRFILT_DECL_ONEPOLEINIT_FN(smpl_t, smpl_t_short); \
    IIRFILT_DECL_ONEPOLEFILT_FN(smpl_t, smpl_t_short); \
    IIRFILT_DECL_BIQUADINIT_FN(smpl_t, smpl_t_short);  \
    IIRFILT_DECL_BIQUADFILT_FN(smpl_t, smpl_t_short);  \
    IIRFILT_DECL_FILT_FN(smpl_t, smpl_t_short);        \
    IIRFILT_DECL_INIT_FN(smpl_t, smpl_t_short);

#endif  // _IIRFILT_GEN_
