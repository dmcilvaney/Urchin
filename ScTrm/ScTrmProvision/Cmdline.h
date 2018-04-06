#pragma once

#define CLEAR_ALL_SLOTS 0xFFFF

typedef struct _CMD_PARAM
{
    PSTR vComPort;
    BOOLEAN force;
    BOOLEAN readEK;
    PSTR ekFilePath;
    BOOLEAN enroll;
    unsigned int enrollSlot;
    unsigned int test;
    BOOLEAN clear;
    unsigned int clearSlot;
} CMD_PARAM;

int
GetCmdlineParams( 
    int argc, 
    char *argv[],
    CMD_PARAM *param
);