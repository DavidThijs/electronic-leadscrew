// Clough42 Electronic Leadscrew
// https://github.com/clough42/electronic-leadscrew
//
// MIT License
//
// Copyright (c) 2019 James Clough
//
// Modified by Denis Tikhonov to use an additional MPG input and a left/right lever
// for lathes that have no half-nuts and the nut is directly coupled to the apron.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

/*
  ___  _                     _                     _ _   _
 / _ \| |                   | |                   | | | | |
/ /_\ \ |__   __ _ _ __   __| | ___  _ __     __ _| | | | |__   ___  _ __   ___
|  _  | '_ \ / _` | '_ \ / _` |/ _ \| '_ \   / _` | | | | '_ \ / _ \| '_ \ / _ \
| | | | |_) | (_| | | | | (_| | (_) | | | | | (_| | | | | | | | (_) | |_) |  __/_
\_| |_/_.__/ \__,_|_| |_|\__,_|\___/|_| |_|  \__,_|_|_| |_| |_|\___/| .__/ \___( )
                                                                    | |        |/
                                                                    |_|
                       _                        _              _
                      | |                      | |            | |
 _   _  ___  __      _| |__   ___     ___ _ __ | |_ ___ _ __  | |__   ___ _ __ ___
| | | |/ _ \ \ \ /\ / / '_ \ / _ \   / _ \ '_ \| __/ _ \ '__| | '_ \ / _ \ '__/ _ \
| |_| |  __/  \ V  V /| | | | (_) | |  __/ | | | ||  __/ |    | | | |  __/ | |  __/
 \__, |\___|   \_/\_/ |_| |_|\___/   \___|_| |_|\__\___|_|    |_| |_|\___|_|  \___|
  __/ |
 |___/



 Seriously, if you are reading this and still hope to modify some of the code and
 use it for your project, you better think again. Seven days of misery, suffering
 and pain have gone into modifying the simple and elegant code  written by James.
 The code has not been tested on anything other than my setup and will most likely
 misbehave if you change things to other leadscrew parameters or wish to switch to
 fixed point calculations.

 You have been warned.
  */


//================================================================================
//                                  LEADSCREW
//
// Define the type and pitch of leadscrew you have in your lathe.  If you have an
// imperial leadscrew, define LEADSCREW_TPI.  If you have a metric leadscrew,
// define LEADSCREW_HMM (pitch in hundredths of a millimeter).  Do not define
// both.
//================================================================================

// For Imperial leadscrews: pitch in Threads Per Inch (TPI)
// #define LEADSCREW_TPI 12

// For metric leadscrews: pitch in hundredths of a millimeter (HMM)
// Example: 200hmm = 2mm
#define LEADSCREW_HMM 500

#define MOSTLY_METRIC   // The feed tables are so big we may run out of memory. Setting Mostly Metric will decrease the number of available Inch feeds to save space.
// Disregard that, I most likely have messed up the inch calculations anyway. Beware.
//#define MOSTLY_INCH // In theory, this decreases the number of available metric feeds (not threads), but I have a suspicion it won't work.


//================================================================================
//                                STEPPER/SERVO
//
// Define the number of steps and microsteps for your stepper motor, the pin
// polarity of the driver, and whether to use additional features, like servo
// alarm feedback.
//================================================================================

// Steps and microsteps
#define STEPPER_MICROSTEPS 10
#define STEPPER_RESOLUTION 200

// Step, direction and enable pins are normally active-high
// #define INVERT_STEP_PIN true
// #define INVERT_DIRECTION_PIN true
#define INVERT_ENABLE_PIN true
#define INVERT_ALARM_PIN true

#define INVERT_MPG_DIR
#define INVERT_LEVER_LEFT_PIN true
#define INVERT_LEVER_RIGHT_PIN true

// Enable servo alarm feedback
#define USE_ALARM_PIN




//================================================================================
//                                 ENCODER
//
// Define the type of encoder you are using on the spindle.  The firmware assumes
// the encoder is turning at a 2:1 ratio with the spindle.
//================================================================================
#define ENCODER_RESOLUTION 8192 // 1024 line encoder on a 15 tooth pulley, 30 tooth pulley on spindle

#define MPG_RESOLUTION 400 // Clicks per Rotation *4. Typical Chinese MPGs have 100 clicks.
#define MPG_FEED_PER_REV 100 //  hundredths of a mm OR thou per revolution - keep an eye on it!
//#define HMM_MPG_NUMERATOR 200000 // MPG_FEED_PER_REV*STEPPER_RESOLUTION*STEPPER_MICROSTEPS
//#define HMM_MPG_DENOMINATOR 200000// MPG_RESOLUTION*LEADSCREW_HMM
#define MPG_FEED 1 // Numerator / Denominator // Only "1" works so don't even try different values
#define _MPG_MAX_COUNT 800// Don't even ask why


//================================================================================
//                               CPU / TIMING
//
// Define the CPU clock, interrupt, and refresh timing.  Most users will not need
// to touch these settings.
//================================================================================

// Stepper state machine cycle time, in microseconds
// Two cycles are required per step
#define STEPPER_CYCLE_US 5

// User interface refresh rate, in Hertz
#define UI_REFRESH_RATE_HZ 100

// RPM recalculation rate, in Hz
#define RPM_CALC_RATE_HZ 10

// Microprocessor system clock
#define CPU_CLOCK_MHZ 100
#define CPU_CLOCK_HZ (CPU_CLOCK_MHZ * 1000000)





#if defined(LEADSCREW_TPI)
#define THOU_MPG_NUMERATOR ((Uint32)MPG_FEED_PER_REV*LEADSCREW_TPI*STEPPER_RESOLUTION*STEPPER_MICROSTEPS)
#define THOU_MPG_DENOMINATOR ((Uint32)MPG_RESOLUTION*1000)
#endif
#if defined(LEADSCREW_HMM)
#define THOU_MPG_NUMERATOR ((Uint32)MPG_FEED_PER_REV*254*STEPPER_RESOLUTION*STEPPER_MICROSTEPS)
#define THOU_MPG_DENOMINATOR ((Uint32)MPG_RESOLUTION*100*LEADSCREW_HMM)
#endif

#if defined(LEADSCREW_TPI)
#define HMM_MPG_NUMERATOR ((Uint32)MPG_FEED_PER_REV*10*LEADSCREW_TPI*STEPPER_RESOLUTION*STEPPER_MICROSTEPS)
#define HMM_MPG_DENOMINATOR ((Uint32)MPG_RESOLUTION*254*100)
#endif
#if defined(LEADSCREW_HMM)
//#define HMM_MPG_NUMERATOR ((Uint32)MPG_FEED_PER_REV*STEPPER_RESOLUTION*STEPPER_MICROSTEPS)
//#define HMM_MPG_DENOMINATOR ((Uint32)MPG_RESOLUTION*LEADSCREW_HMM)
#endif

#ifdef MOSTLY_METRIC
//#define MPG_FEED HMM_MPG_NUMERATOR / HMM_MPG_DENOMINATOR
#define MPG_FEED_NUMERATOR HMM_MPG_NUMERATOR
#define MPG_FEED_DENOMINATOR HMM_MPG_DENOMINATOR
#else
#ifdef MOSTLY_INCH
#error "Not Implemented inch math"
//#define MPG_FEED THOU_MPG_NUMERATOR / THOU_MPG_DENOMINATOR
#define MPG_FEED_NUMERATOR THOU_MPG_NUMERATOR
#define MPG_FEED_DENOMINATOR THOU_MPG_DENOMINATOR
#else
#error "Define either MOSTLY_METRIC or MOSTLY_INCH"
#endif
#endif



#endif // __CONFIGURATION_H
