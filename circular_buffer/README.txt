This library, once initiated with the function

STMLIBS_StatusTypeDef CIRCULAR_BUFFER_init(CIRCULAR_BUFFER_HandleTypeDef *handle,
                                           void *buffer,
                                           uint32_t length,
                                           uint32_t el_size);

allows to operate on a circolar buffer with the following functions:

uint8_t CIRCULAR_BUFFER_is_full(CIRCULAR_BUFFER_HandleTypeDef *handle); returns 1 in case of error or if the buffer is full

uint8_t CIRCULAR_BUFFER_is_empty(CIRCULAR_BUFFER_HandleTypeDef *handle); returns 1 in case of error or if the buffer is empty

STMLIBS_StatusTypeDef CIRCULAR_BUFFER_enqueue(CIRCULAR_BUFFER_HandleTypeDef *handle, void *obj);  returns STMLIBS_ERROR in case of error otherwise it puts the obj in the head of the buffer

STMLIBS_StatusTypeDef CIRCULAR_BUFFER_dequeue(CIRCULAR_BUFFER_HandleTypeDef *handle, void *obj); returns STMLIBS_ERROR in case of error otherwise removes the obj from the buffer
