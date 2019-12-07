#ifndef MESSAGE_H
#define MESSAGE_H

typedef enum message {
    POSITION_UPDATE,
    BLOCK_UPDATE,
    POSITIONS_UPDATE,
        BLOCK_UPDATES,
} message_t;

typedef struct position_update {
    message_t message;
    int pid;
    float x;
    float y;
    float z;
} position_update_t;

typedef struct block_update {
    message_t message;
    int pid;
    int x;
    int y;
    int z;
    int block;
} block_update_t;

typedef struct positions_update {
    message_t message;
    float positions[5][3];
} positions_update_t;

typedef struct block_updates {
    message_t message;
    int length;
    block_update_t updates[1024];
} block_updates_t;

typedef struct bulk_update {
    positions_update_t position_update;
    block_updates_t block_updates;
} bulk_update_t;

#endif /* MESSAGE_H */