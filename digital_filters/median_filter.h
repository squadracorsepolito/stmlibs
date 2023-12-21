/**
 * @file      median_filter.h
 * @prefix    MEDFILT
 * @author    Simone Ruffini [simone.ruffini@squadracorse.com
 *                            simone.ruffini.work@gmail.com]
 * @date      mar 19 dic 2023, 16:54:43, CET
 *
 * @brief     One-Dimensional Median Digital Filter single file library
 *            Useful for spike and noise removal from analog signals
 *
 * @note      Refactoring from https://github.com/accabog/MedianFilter
 * @license   Licensed under "THE BEER-WARE LICENSE", Revision 69 (see LICENSE)
 *
 @verbatim
                    ##### How to use this filter #####
 ===============================================================================
 (#) Initialze a MEDFILT_Handle struct and an array of MEDFILT_Node strutct of
     constant length.
     ** NOTE: The array length must be odd and >1 ** eg:
     #define MEDFILT_WINDOW_LEN 7
     static struct MEDFILT_Handle hmedfilt;
     static struct MEDFILT_Node medianfilter_window[MEDFILT_WINDOW_LEN];

 (#) Assign the previous instantiated MEDFILT_Handle struct with correct values:
     hmedfilt.window = medianfilter_window;
     hmedfilt.window_len = MEDFILT_WINDOW_LEN;
 (#) Call MEDFILT_Init, e.g.:
     MEFILT_Init(&hmedfilt);
 (#) In the sample acquisition loop/callback run:
     MEDFILT_value_t new_sample,filtered_sample;
     ADC_GetNewValue(&hadc,(uint16_t*)&new_sample);
     filtered_sample = MEDFILT_Insert(&hMedianFilter,new_sample);
 @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MEDFILT_
#define _MEDFILT_

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

/* Exported constants --------------------------------------------------------*/

// On most 32bit microcontroller platforms unsigned short is 16bit
// enough for the output of an ADC
#ifndef MEDFILT_NODE_VALUE_T
#define MEDFILT_NODE_VALUE_T unsigned short
#endif

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef MEDFILT_NODE_VALUE_T MEDFILT_value_t;

struct MEDFILT_Node {
    MEDFILT_value_t value; /*!< sample value */

    /* Pointer for fifo (queue) view of the window */
    struct MEDFILT_Node *nextByAge; /*!< pointer to next oldest inserted node */

    /* Pointers for sorted view of the window */
    struct MEDFILT_Node *nextByValue; /*!< pointer to next node with smallest value */
    struct MEDFILT_Node *prevByValue; /*!< pointer to previous node with smallest value */
};

struct MEDFILT_Handle {
    size_t window_len;                        /*!< nodes window length @note: MUST BE ODD and >1 */
    struct MEDFILT_Node *window;              /*!< pointer to nodes buffer called window */
    struct MEDFILT_Node *oldestNode_ptr;      /*!< pointer to the oldest inserted node in the window */
    struct MEDFILT_Node *minValueNode_ptr;    /*!< pointer to smallest value */
    struct MEDFILT_Node *medianValueNode_ptr; /*!< pointer to node with median value */
};

/* Exported functions --------------------------------------------------------*/

/**
 * @biref Initialize a median filter with contents of the given handle
 * @param  handle pointer to a handle of MEDFILT
 * @note   The handle must be correctly preconfigured:
 *         handle->window must point to an existing array of ODD length >1
 *         handle->window_len must be assigned the length of the array
 * @return 0 Init OK, -1 Init NotOK
 */
static inline int MEDFILT_Init(struct MEDFILT_Handle *handle) {
    if (handle && handle->window && (handle->window_len % 2) && (handle->window_len > 1)) {
        // initialize window buffer: all nodes are linked circulary
        // node0.prev==node[window_len-1], node"window_len-1".next==node0
        for (size_t i = 0; i < handle->window_len; i++) {
            handle->window[i].value                                  = 0;
            handle->window[i].nextByAge                              = &handle->window[(i + 1) % handle->window_len];
            handle->window[i].nextByValue                            = &handle->window[(i + 1) % handle->window_len];
            handle->window[(i + 1) % handle->window_len].prevByValue = &handle->window[i];
        }
        // initialize main pointers
        handle->oldestNode_ptr      = handle->window;
        handle->minValueNode_ptr    = handle->window;
        handle->medianValueNode_ptr = &handle->window[handle->window_len / 2];

        return 0;
    }

    return -1;
}

/**
  * @brief  Insert a new sample in the window and get back the new median value
  *         after insertion. The median value is the filtered output.
  * @param  handle pointer to a handle of MEDFILT
  * @param  sample   new sample value to insert in the window
  * @return The updated median value of the window (a.k.a. filtered output)
  * @note   The insert procedure relocates always the oldest node in the window
  *         and uses it as the fresh node for the sample value.
  */
static inline MEDFILT_value_t MEDFILT_Insert(struct MEDFILT_Handle *handle, MEDFILT_value_t sample) {
    struct MEDFILT_Node *newNode, *it;

    if (handle->oldestNode_ptr == handle->minValueNode_ptr) {
        // if oldest inserted node (relocated node) is also the smallest node
        // update minValueNode pointer to next minimum (see @note above)
        handle->minValueNode_ptr = handle->minValueNode_ptr->nextByValue;
    }

    // if the oldest inserted node (reolcated node) is the current medium
    // or a bigger value (a node on the mediums right)
    if ((handle->oldestNode_ptr == handle->medianValueNode_ptr) ||
        (handle->oldestNode_ptr->value > handle->medianValueNode_ptr->value)) {
        // move the current medium to the left (a smaller value)
        handle->medianValueNode_ptr = handle->medianValueNode_ptr->prevByValue;
    }

    // use oldestNode as newNode
    newNode        = handle->oldestNode_ptr;
    newNode->value = sample;

    // "popping" procedure of oldestNode ---------------------------------------
    // - in the byValue list view of the window (keep min value sorting)
    //   BEFORE
    //   |          node1        | <-> | newNode === node2 === OLDEST | <-> |        node3         |
    //   | ...<-prev next->node2 |     | node1<-prev      next->node3 |     | node2<-prev next->...|
    handle->oldestNode_ptr->nextByValue->prevByValue = handle->oldestNode_ptr->prevByValue;
    handle->oldestNode_ptr->prevByValue->nextByValue = handle->oldestNode_ptr->nextByValue;
    //   AFTER
    //           +-| newNode === node2 === OLDEST |-+
    //          /  | node1<-prev      next->node2 |  \
    //         V                                      V
    //   |         node1          | <-> |         node3         |
    //   | ...<-prev  next->node3 |     | node1<-prev next->... |
    // - in the byAge list view of the window (keep insert time linking)
    //   BEFORE (node2 is oldest inserted, node7 second-last inserted)
    //   |       node7         |     | newNode === node2 === OLDEST |
    //   | ...<-prev next->... |     | node1<-prev      next->node3 |
    //   | ...<-nextByAge      | <-> | node7<-nextByAge             |
    handle->oldestNode_ptr = handle->oldestNode_ptr->nextByAge;
    //   AFTER
    //   |  node7 === OLDEST   |     |     newNode === node2        |
    //   | ...<-prev next->... |     | node1<-prev      next->node3 |
    //   | ...<-nextByAge      | <-> | node7<-nextByAge             |
    // -------------------------------------------------------------------------

    // Find where to insert the new node in the "sorted by value" view of the window
    // start from the smallest valued node
    it = handle->minValueNode_ptr;

    size_t i;
    for (i = 0; i < handle->window_len - 1; i++) {  // -1 because we "popped" the oldest node
        if (sample < it->value) {
            // at i == 0 we are comparing the new sample with minValueNode
            if (i == 0) {
                // use the new value as the new minValueNode
                handle->minValueNode_ptr = newNode;
            }
            break;
        }
        it = it->nextByValue;
    }
    // `it` will point to the future greater neighbour of newValue: insert newNode before `it`
    // BEFORE
    //                  |   newNode === node10   |
    //                  | node1<-prev next->node3|
    // |       node4           | <-> |        it === node5    |
    // | ...<-prev next->node5 |     | node4<-prev  next->... |
    it->prevByValue->nextByValue = newNode;
    newNode->prevByValue         = it->prevByValue;
    it->prevByValue              = newNode;
    newNode->nextByValue         = it;
    // AFTER
    // |         node4           | <-> |  newNode === node10     | <-> |       it === node5     |
    // | ...<-prev  next->node10 |     | node4<-prev next->node5 |     | node10<-prev next->... |

    // update median node
    if (i >= (handle->window_len / 2)) {
        // since only onde node was added, medianValueNode shifts to the next one
        handle->medianValueNode_ptr = handle->medianValueNode_ptr->nextByValue;
    }

    return handle->medianValueNode_ptr->value;
}

#ifdef MEDFILT_DEBUG_FN_ON
#include <stdio.h>

static inline void MEDFILT_printByValue(struct MEDFILT_Handle *handle) {
    struct MEDFILT_Node *it = handle->minValueNode_ptr;
    printf("Value: [ ");
    for (int i = 0; i < handle->window_len; i++) {
        if (it == handle->medianValueNode_ptr) {
            printf("(%u) ", it->value);
        } else {
            printf("%u ", it->value);
        }
        it = it->nextByValue;
    }
    printf("]\n");
};

static inline void MEDFILT_printByAge(struct MEDFILT_Handle *handle) {
    struct MEDFILT_Node *it = handle->oldestNode_ptr;
    printf("Age:   [ ");
    for (int i = 0; i < handle->window_len; i++) {
        if (it == handle->medianValueNode_ptr) {
            printf("(%u) ", it->value);
        } else {
            printf("%u ", it->value);
        }
        it = it->nextByAge;
    }
    printf("]\n");
};
#endif  // MEDFILT_DEBUG_FN_ON

#endif  //_MEDFILT_
