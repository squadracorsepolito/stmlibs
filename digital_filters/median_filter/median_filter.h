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

                             ##### PERFORMANCE #####
 ===============================================================================
 The filter has O(WINDOW_LEN) space complexity and O(WINDOW_LEN) time complexity
 when new nodes are added.

                          ##### Internal Details #####
 ===============================================================================
 The window is a statically declared buffer of linked nodes making it a double
 linked list with circular linking.

 When a new node/sample is inserted in the list the oldest inserted node is used
 as the instance for the new node.
 The oldest node is always relocated in the list when inserting a new one.

 The window has two main views depending on which type of pointers in a node are
 used to traverse it:
 - `nextByAge`: iterates to the next oldest inserted node, itrerating through
                these pointers is like using a queue (first in first out). The
                pointer `oldestNode` points to the oldest inserted node in
                the window a.k.a. the head of a queue
- `nextByValue`: iterates to the next minimal valued node in the window:
                 the pointed node will have a greater or equal value.
                 The `minValueNode` always points to the node with smallest
                 value in the window.
- `prevByValue`: iterates to the previous minimal valued node in the window:
                 the pointed node will have a smaller or equal value.
 One additional pointer is `medianValueNode` that points to the current
 median value node of the window.
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

/* Internal macros -----------------------------------------------------------*/

/**
 * @brief Insert (byValue view) a node before a pivot node (selected node)
 *        keeping linking consistent (its predecessor gets linked to newNode)
 * @param nodePivot_ptr A pointer to struct MEDFILT_Node that is the pivot node
 *        newNode will become its prev link
 * @param newNode_ptr A pointer to struct MEDFILT_Node that will have as next
 *        link the pivotNode
 * @note  the node preceding pivotNode will be correctly linked to newNode
 */
#define __BYVALUE_INSERT_NODE_BEFORE(nodePivot_ptr, newNode_ptr)              \
    do {                                                                      \
        newNode_ptr->prevByValue                = nodePivot_ptr->prevByValue; \
        newNode_ptr->nextByValue                = nodePivot_ptr;              \
        nodePivot_ptr->prevByValue->nextByValue = newNode_ptr;                \
        nodePivot_ptr->prevByValue              = newNode_ptr;                \
    } while (0)

/**
 * @brief Link (byValue view) two nodes so that next/prevByValue pointers link
 *        to each other.
 * @param nodePrev_ptr A pointer to struct MEDFILT_Node that will be the link
 *        prevByValue to node nodeNext_ptr
 * @param nodeNext_ptr A pointer to struct MEDFILT_Node that will be the link
 *        next to node nodePrev_ptr
 */
#define __BYVALUE_LINK_NODES(nodePrev_ptr, nodeNext_ptr) \
    do {                                                 \
        nodeNext_ptr->prevByValue = nodePrev_ptr;        \
        nodePrev_ptr->nextByValue = nodeNext_ptr;        \
    } while (0)

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
    size_t window_len;                    /*!< nodes window length @note: MUST BE ODD and >1 */
    struct MEDFILT_Node *window;          /*!< pointer to nodes buffer called window */
    struct MEDFILT_Node *oldestNode;      /*!< pointer to the oldest inserted node in the window */
    struct MEDFILT_Node *minValueNode;    /*!< pointer to smallest value */
    struct MEDFILT_Node *medianValueNode; /*!< pointer to node with median value */
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
        handle->oldestNode      = handle->window;
        handle->minValueNode    = handle->window;
        handle->medianValueNode = &handle->window[handle->window_len / 2];

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
  * @note   PERFORMANCE: the insert procedure is O(WINDOW_LEN) in time
            O(1) in space
  * @note   The insert procedure relocates always the oldest node in the window
  *         and uses it as the fresh node for the sample value.
  */
static inline MEDFILT_value_t MEDFILT_Insert(struct MEDFILT_Handle *handle, MEDFILT_value_t sample) {
    struct MEDFILT_Node *newNode, *it;

    if (handle->oldestNode == handle->minValueNode) {
        // if oldest inserted node (relocated node) is also the smallest node
        // update minValueNode pointer to next minimum (see @note above)
        handle->minValueNode = handle->minValueNode->nextByValue;
    }

    // Move in advance the medianValue pointer to its predecessor if either:
    // - the oldest inserted node (relocated node) is the current medium
    //   to avoid losing the medium reference
    // - the oldest inserted node has a bigger value then the current medium
    // If the move was not correct it will later be fixed
    if ((handle->oldestNode == handle->medianValueNode) ||
        (handle->oldestNode->value > handle->medianValueNode->value)) {
        handle->medianValueNode = handle->medianValueNode->prevByValue;
    }

    // use oldestNode as newNode
    newNode        = handle->oldestNode;
    newNode->value = sample;

    // relocation procedure of oldestNode -----------------------------------------
    // - in the byValue list view of the window, keep min value sorting.
    //   When oldestNode pointer will be relocated its neighbours have to be linked
    __BYVALUE_LINK_NODES(handle->oldestNode->prevByValue, handle->oldestNode->nextByValue);
    // - in the byAge list view of the window, keep insert time linking.
    //   The oldestNode pointer is updated to the second-last inserted node
    handle->oldestNode = handle->oldestNode->nextByAge;
    // ----------------------------------------------------------------------------

    // Find where to insert the new node in the "sorted by value" view of the window
    // start from the smallest valued node
    it = handle->minValueNode;

    size_t i;
    for (i = 0; i < handle->window_len - 1; i++) {  // -1 because we "popped" the oldest node
        if (sample < it->value) {
            if (i == 0) {
                // if minValueNode is no more the smallest valued node, update minValueNode
                handle->minValueNode = newNode;
            }
            break;
        }
        it = it->nextByValue;
    }

    // `it` will point to the future greater neighbour of newValue: insert newNode before `it`
    __BYVALUE_INSERT_NODE_BEFORE(it, newNode);

    // update median node
    if (i >= (handle->window_len / 2)) {
        // since only one node was added, medianValueNode shifts to the next one
        handle->medianValueNode = handle->medianValueNode->nextByValue;
    }

    return handle->medianValueNode->value;
}

#ifdef MEDFILT_DEBUG_FN_ON
#include <stdio.h>

static inline void MEDFILT_printByValue(struct MEDFILT_Handle *handle) {
    struct MEDFILT_Node *it = handle->minValueNode;
    printf("Value: [ ");
    for (int i = 0; i < handle->window_len; i++) {
        if (it == handle->medianValueNode) {
            printf("(%u) ", it->value);
        } else {
            printf("%u ", it->value);
        }
        it = it->nextByValue;
    }
    printf("]\n");
};

static inline void MEDFILT_printByAge(struct MEDFILT_Handle *handle) {
    struct MEDFILT_Node *it = handle->oldestNode;
    printf("Age:   [ ");
    for (int i = 0; i < handle->window_len; i++) {
        if (it == handle->medianValueNode) {
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
