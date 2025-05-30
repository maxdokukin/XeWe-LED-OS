#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib_old/FastLED-3.9.16/src/platforms/arm/d21/led_sysdefs_arm_d21.h"
#ifndef __INC_LED_SYSDEFS_ARM_D21_H
#define __INC_LED_SYSDEFS_ARM_D21_H


#define FASTLED_ARM
#define FASTLED_ARM_M0_PLUS

#ifndef INTERRUPT_THRESHOLD
#define INTERRUPT_THRESHOLD 1
#endif

// Default to allowing interrupts
#ifndef FASTLED_ALLOW_INTERRUPTS
#define FASTLED_ALLOW_INTERRUPTS 1
#endif

#if FASTLED_ALLOW_INTERRUPTS == 1
#define FASTLED_ACCURATE_CLOCK
#endif

// reusing/abusing cli/sei defs for due
#define cli()  __disable_irq();
#define sei() __enable_irq();


#endif
