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

#include <jni.h>
#include <stdio.h>
#include "net_hackcasual_freeciv_NativeHarness.h"

#include "config.h"

#include <stdlib.h>

#include "capstr.h"
#include "srv_main.h"

#include "civserver.h"
#include "log.h"
#include <android/log.h>

#include <sys/types.h>
#include <time.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);


JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_startServer
  (JNIEnv *je, jobject o) {

	FILE *fp;
	  if((fp=freopen("/sdcard/freeciv_out.log", "w" ,stdout))==NULL) {
	    printf("Cannot open file.\n");
	    exit(1);
	  }

	  if((fp=freopen("/sdcard/freeciv_err.log", "w" ,stderr))==NULL) {
	    printf("Cannot open file.\n");
	    exit(1);
	  }

	  setenv ("HOME", "/sdcard/Freeciv", 0);




	srv_init();

	  srvarg.log_filename="/sdcard/log.out";
	  srvarg.loglevel=LOG_DEBUG;


	init_our_capability();
	srv_main();
    /* test */
	return;
}

