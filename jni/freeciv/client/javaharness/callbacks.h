/**********************************************************************
 Android-Freeciv - Copyright (C) 2010 - C Vaughn
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifndef FC__JNI_CALLBACKS_H
#define FC__JNI_CALLBACKS_H

#include "net_hackcasual_freeciv_NativeHarness.h"
#include "config.h"
#include "city.h"

#include <jni.h>

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);

void java_new_city_dialog(char * suggestion, int unit_id);
void java_city_dialog(const struct city *pcity);
void java_set_turn_done_state(bool state);
void java_update_info();

#endif				/* FC__JNI_CALLBACKS_H */
