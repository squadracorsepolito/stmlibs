#ifndef STMLIBS_STATUS_H
#define STMLIBS_STATUS_H

typedef enum {
    STMLIBS_OK      = 0x00U,
    STMLIBS_ERROR   = 0x01U,
    STMLIBS_BUSY    = 0x02U,
    STMLIBS_TIMEOUT = 0x03U
} STMLIBS_StatusTypeDef;

#endif  //STMLIBS_STATUS_H
