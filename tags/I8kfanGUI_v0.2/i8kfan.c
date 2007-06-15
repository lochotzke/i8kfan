/*
 *  i8kfan.c
 *  I8kfanGUI
 *
 *  Created by Andrew Bennett on 5/9/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <sys/types.h>
#include <sys/sysctl.h>
#include "i8kfan.h"

int i8k_get_fan_speed(int fan)
{
    char *name;
    if (fan == I8K_FAN_PRIMARY) {
        name = I8K_FAN_PRIMARY_SPEED;
    } else {
        name = I8K_FAN_SECONDARY_SPEED;
    }
    
    int bufsize, retval;
    size_t len = 4;
    retval = sysctlbyname(name, &bufsize, &len, NULL, 0);
    return bufsize / I8K_FAN_MULT;
}

int i8k_get_fan_status(int fan)
{
    char *name;
    if (fan == I8K_FAN_PRIMARY) {
        name = I8K_FAN_PRIMARY_STATUS;
    } else {
        name = I8K_FAN_SECONDARY_STATUS;
    }
    
    int bufsize, retval;
    size_t len = 4;
    retval = sysctlbyname(name, &bufsize, &len, NULL, 0);
    return bufsize;
}

void i8k_set_fan(int fan, int status)
{
    char *name;
    if (fan == I8K_FAN_PRIMARY) {
        name = I8K_FAN_PRIMARY_STATUS;
    } else {
        name = I8K_FAN_SECONDARY_STATUS;
    }
    
    int retval;
    size_t len = 4;
    retval = sysctlbyname(name, NULL, 0, &status, &len);
}

int i8k_get_cpu_temp(void)
{
    char *name = I8K_CPU_TEMP;
    int bufsize, retval;
    size_t len = 4;
    retval = sysctlbyname(name, &bufsize, &len, NULL, 0);
    return bufsize;
}
