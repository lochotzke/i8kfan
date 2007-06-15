/*
 *  i8kfan.h
 *  I8kfanGUI
 *
 *  Created by Andrew Bennett on 5/9/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __I8KFAN_H__
#define __I8KFAN_H__
#endif

#define I8K_POWER_STATUS            "i8k.power_status"
#define I8K_CPU_TEMP                "i8k.cpu_temp"
#define I8K_BIOS_VERSION            "i8k.bios_version"
#define I8K_FAN_PRIMARY_SPEED       "i8k.fan0_speed"
#define I8K_FAN_SECONDARY_SPEED     "i8k.fan1_speed"
#define I8K_FAN_PRIMARY_STATUS      "i8k.fan0_status"
#define I8K_FAN_SECONDARY_STATUS    "i8k.fan1_status"

#define I8K_FAN_MULT            30
#define I8K_MAX_TEMP            127

#define I8K_FAN_PRIMARY         0
#define I8K_FAN_SECONDARY       1
#define I8K_FAN_OFF             0
#define I8K_FAN_LOW             1
#define I8K_FAN_HIGH            2
#define I8K_FAN_MAX             I8K_FAN_HIGH

int i8k_get_fan_speed(int fan);
int i8k_get_fan_status(int fan);
void i8k_set_fan(int fan, int status);
int i8k_get_cpu_temp(void);
