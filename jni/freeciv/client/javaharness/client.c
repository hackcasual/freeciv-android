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

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

#include "config.h"

/* utility */
#include "capstr.h"
#include "dataio.h"
#include "fciconv.h"
#include "fcintl.h"
#include "log.h"
#include "mem.h"
#include "rand.h"
#include "support.h"
#include "timing.h"

/* common */
#include "ai.h"
#include "diptreaty.h"
#include "game.h"
#include "idex.h"
#include "map.h"
#include "netintf.h"
#include "packets.h"
#include "version.h"

/* include */
#include "chatline_g.h"
#include "citydlg_g.h"
#include "connectdlg_g.h"
#include "dialogs_g.h"
#include "diplodlg_g.h"
#include "editgui_g.h"
#include "graphics_g.h"
#include "gui_main_g.h"
#include "mapctrl_g.h"
#include "mapview_g.h"
#include "menu_g.h"
#include "messagewin_g.h"
#include "pages_g.h"
#include "plrdlg_g.h"
#include "repodlgs_g.h"
#include "voteinfo_bar_g.h"

/* client */
#include "agents.h"
#include "attribute.h"
#include "audio.h"
#include "cityrepdata.h"
#include "climisc.h"
#include "clinet.h"
#include "cma_core.h"           /* kludge */
#include "connectdlg_common.h"  /* client_kill_server() */
#include "control.h"
#include "editor.h"
#include "global_worklist.h"
#include "ggzclient.h"
#include "helpdata.h"           /* boot_help_texts() */
#include "mapview_common.h"
#include "options.h"
#include "overview_common.h"
#include "packhand.h"
#include "tilespec.h"
#include "themes_common.h"
#include "voteinfo.h"
#include "client_main.h"
#include "client.h"
#include "canvas_g.h"
#include "colors_g.h"


/* For sprite loading */

#include "gui-android/sprite.h"
#include "gui-android/canvas.h"
#include "gui-android/mapview.h"



#include <sys/types.h>
#include <time.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);



void startClient() {
	char *logfile = "/sdcard/client.out";
	FILE *fp;
	  if((fp=freopen("/sdcard/freeciv_out_client.log", "w" ,stdout))==NULL) {
	    printf("Cannot open file.\n");
	    exit(1);
	  }

	  if((fp=freopen("/sdcard/freeciv_err_client.log", "w" ,stderr))==NULL) {
	    printf("Cannot open file.\n");
	    exit(1);
	  }

	  setenv ("HOME", "/sdcard/Freeciv", 0);
	  setenv ("USER", "Sparky", 0);

	LOGI("Hello JNI");
	  log_init(logfile, LOG_DEBUG
			  , NULL);
	int i, loglevel;
	int ui_options = 0;
	bool ui_separator = FALSE;
	char *option=NULL;
	bool user_tileset = FALSE;

	  i_am_client(); /* Tell to libfreeciv that we are client */

	  /* Ensure that all AIs are initialized to unused state */
	  ai_type_iterate(ai) {
	    init_ai(ai);
	  } ai_type_iterate_end;




	init_our_capability();


	  (void)user_username(default_user_name, MAX_LEN_NAME);
	  if (!is_valid_username(default_user_name)) {
	    char buf[sizeof(default_user_name)];

	    my_snprintf(buf, sizeof(buf), "_%s", default_user_name);
	    if (is_valid_username(buf)) {
	      sz_strlcpy(default_user_name, buf);
	    } else {
	      my_snprintf(default_user_name, sizeof(default_user_name),
			  "player%d", myrand(10000));
	    }
	  }

	  game.all_connections = conn_list_new();
	  game.est_connections = conn_list_new();

	  ui_init();

	  fc_init_network();

	  init_our_capability();
	  chatline_common_init();
	  init_player_dlg_common();
	  init_themes();

	  options_init();

	  strcpy(default_sound_plugin_name,"none");

	  server_port = 9999;
	  sz_strlcpy(server_host, "localhost");
	  options_load();

	  mysrand(time(NULL));
	  helpdata_init();
	  boot_help_texts(NULL);




	  LOGI("Reading tilespec");
	  tilespec_try_read("amplio2", TRUE);
	  LOGI("Done reading tilespec");
	  overview_size_changed();

	  audio_real_init("stdsounds", "none");
	  //audio_play_music("music_start", NULL);

	  auto_connect = TRUE;

	  init_mapcanvas_and_overview();

	  /* run gui-specific client */
	  ui_main(0, NULL);

	  /* termination */
	  client_exit();

	return;
}



JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_loadTileset
  (JNIEnv *je, jobject o) {

	//TODO: Allow specifying tileset
	tilespec_try_read("amplio", TRUE);
	tileset_init(tileset);
	tileset_load_tiles(tileset);
}


int width, height;


jmethodID draw_frame = 0;
jmethodID client_connect = 0;
jmethodID update_tileset_progress = 0;
jmethodID measure_text = 0;
jmethodID render_text = 0;
jmethodID unit_constructor = 0;

jclass unit_cls = 0;


JNIEnv *env_nh = 0;
jobject native_harness = 0;
void * bufferData = 0;
void * incommingBuffer = 0;

JavaVM *jvm;

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_startClient
  (JNIEnv *je, jobject o) {

	jmethodID register_buffer = 0;
	(*je)->GetJavaVM(je, &jvm);
	env_nh = je;

	jclass cls = (*env_nh)->GetObjectClass(env_nh, o);
	native_harness = (*env_nh)->NewGlobalRef(env_nh,o);

	draw_frame = (*env_nh)->GetMethodID(env_nh, cls, "updateDisplay", "()V");
	client_connect = (*env_nh)->GetMethodID(env_nh, cls, "clientConnected", "()V");
	measure_text = (*env_nh)->GetMethodID(env_nh, cls, "getTextSize", "(Ljava/lang/String;)I");
	render_text = (*env_nh)->GetMethodID(env_nh, cls, "renderString", "(Ljava/lang/String;)I");

	update_tileset_progress = (*env_nh)->GetMethodID(env_nh, cls, "updateTilesetProgress", "(Ljava/lang/String;)V");



	if (draw_frame == 0) {
		LOGE("Failed to get updateDisplay");
		return;
	}

	if (measure_text == 0) {
		LOGE("Failed to get measureText");
		return;
	}

	if (render_text == 0) {
		LOGE("Failed to get renderText");
		return;
	}


	register_buffer = (*env_nh)->GetMethodID(env_nh, cls, "registerNativeBuffers", "(Ljava/nio/ByteBuffer;Ljava/nio/ByteBuffer;)V");

	if (register_buffer == 0) {
		LOGE("Failed to get registerNativeBuffer");
		return;
	}
	if (!bufferData)
		bufferData = fc_malloc(BUFFER_SIZE);

	// 30 pixels high by 300 width by 2 bytes per pixel
	incommingBuffer = fc_malloc(30 * 300 * 2);

	jobject direct_buffer = (*env_nh)->NewDirectByteBuffer(env_nh, bufferData, BUFFER_SIZE);
	jobject incomming_buffer = (*env_nh)->NewDirectByteBuffer(env_nh, incommingBuffer, BUFFER_SIZE);

	(*env_nh)->CallVoidMethod(env_nh, native_harness, register_buffer, direct_buffer, incomming_buffer);

	startClient();

}

void update_display() {
	JNIEnv* env;
	int res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
	(*env)->CallVoidMethod(env, native_harness, draw_frame);
};

void client_connected() {
	JNIEnv* env;
	int res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);
	(*env)->CallVoidMethod(env, native_harness, client_connect);
};

void measure_string(int *width, int *height, const char *text) {
	JNIEnv* env;
	int res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

	jstring str = ((*env)->NewStringUTF(env, text));

	jint packed_size = (*env)->CallIntMethod(env, native_harness, measure_text, str);
	(*env)->DeleteLocalRef(env, str);
	*width = ((unsigned int)packed_size) >> 16;
	*height = ((unsigned int)packed_size) & 0xFFFF;

	//LOGI("%dx%d", width, height);
	//(*jvm)->DetachCurrentThread(jvm);
};

void report_tileset_progress(const char *text) {
	JNIEnv* env;
	int res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

	jstring str = ((*env)->NewStringUTF(env, text));

    (*env)->CallVoidMethod(env, native_harness, update_tileset_progress, str);
	(*env)->DeleteLocalRef(env, str);
};

struct sprite s;

//render_text
struct sprite* sprite_string(const char *text) {
	JNIEnv* env;
	int res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

	jstring str = ((*env)->NewStringUTF(env, text));

	jint packed_size = (*env)->CallIntMethod(env, native_harness, render_text, str);
	(*env)->DeleteLocalRef(env, str);
	int w = ((unsigned int)packed_size) >> 16;
	int h = ((unsigned int)packed_size) & 0xFFFF;


	s.width = w;
	s.height = h;
	s.data = incommingBuffer;

	return &s;

	//LOGI("%dx%d", width, height);
	//(*jvm)->DetachCurrentThread(jvm);
};


JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_init
  (JNIEnv *je, jobject o,  jint w, jint h) {
	civ_lock();
	width = w;
	height = h;
	android_width = w;
	android_height = h;

	  lastTouchX = w / 2;
	  lastTouchY = h / 2;
	  draw_city_productions = TRUE;
	  smooth_move_unit_msec = 200;
	  smooth_center_slide_msec = 100;
	  auto_center_on_combat = TRUE;

	if (!bufferData)
		bufferData = fc_malloc(BUFFER_SIZE);


    if (!displayCanvas || displayCanvas->width != w || displayCanvas->height != h) {
    	displayCanvas = fc_malloc(sizeof(struct sprite));
    	displayCanvas->width = w;
    	displayCanvas->height = h;
    	displayCanvas->data = bufferData;
    }
    LOGI("Setting display size: %dx%d", android_width, android_height);
    map_canvas_resized(w, h);
    civ_unlock();
}
// Updates displayCanvas, which is backed by bufferData, after a data operation
JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_reloadMap
  (JNIEnv *je, jobject o) {
	LOGI("reloadMap displayCanvas %p data %p", displayCanvas, displayCanvas->data);
	canvas_copy(displayCanvas, mapview.store, 0, 0, 0, 0, displayCanvas->width, displayCanvas->height);

}

JNIEXPORT jboolean JNICALL Java_net_hackcasual_freeciv_NativeHarness_touchEvent
  (JNIEnv *je, jobject o, jint x, jint y, jint type) {

	struct timeval tvs, tve;
    int nif = 0;
    static int oldx = -1;
    static int oldy = -1;

    lastTouchX = x;
    lastTouchY = y;

    int deltax;
    int deltay;
	static bool cancelUp;

	struct tile *plasttile;

	switch (type) {
	case 0: {
		cancelUp = FALSE;
		oldx = x;
		oldy = y;
		plasttile = canvas_pos_to_tile(x,y);

		break;
	}
	case 1: {
		if (!cancelUp && abs(x - oldx) < 10 && abs(y - oldy) < 10) {
			can_slide = FALSE;
			LOGI("ABP Lock");
			civ_lock();
			action_button_pressed(x,y,SELECT_POPUP);
			can_slide = TRUE;
			flush_dirty();
			struct tile *ptile = canvas_pos_to_tile(x,y);
			civ_unlock();
			return (ptile && ptile->units)?unit_list_size(ptile->units):0;
		} else if (!cancelUp) {
			LOGI("Recenter Lock");
			civ_lock();
			can_slide = FALSE;
			recenter_button_pressed(x, y);
			can_slide = TRUE;
			flush_dirty();
			civ_unlock();
		}
		break;
	}
	case 2: {
		/*hover_state = HOVER_GOTO;
		control_mouse_cursor(canvas_pos_to_tile(x,y));
		update_line(x,y);
		break;*/
		if (cancelUp || abs(x - oldx) > 10 || abs(y - oldy) > 10) {
			cancelUp = TRUE;
			LOGI("Slide Lock");
			civ_lock();
			can_slide = FALSE;
			deltax = oldx - x;
			deltay = oldy - y;
			set_mapview_origin(mapview.gui_x0 + deltax, mapview.gui_y0 + deltay);
			flush_dirty();
			civ_unlock();
			oldx = x;
			oldy = y;
			can_slide = TRUE;
		}
		break;
	}
	case 3: {
		cancelUp = TRUE;
		break;
	}
	}

	return 0;
}

// TODO: Support irrigation modes
JNIEXPORT jbyteArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getAvailableCommandsForActiveUnit
  (JNIEnv * env, jobject obj) {
	struct unit * pUnit;
	char availCommands[COM_UNIT_LAST];
	int commandCount = 0;

	pUnit = head_of_units_in_focus();

	if (pUnit) {

	    struct tile *pTile = pUnit->tile;
	    struct city *pCity = tile_city(pTile);
	    struct terrain *pTerrain = tile_terrain(pTile);
	    struct base_type *pbase;
	    struct unit_list *punits = get_units_in_focus();


		if (can_unit_build_city(pUnit)) {
			availCommands[commandCount++] = COM_BUILD_CITY;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_GOTO)) {
			availCommands[commandCount++] = COM_GO_TO;
			availCommands[commandCount++] = COM_GO_TO_CITY;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_SENTRY)) {
			availCommands[commandCount++] = COM_SENTRY;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_FORTIFYING)) {
			availCommands[commandCount++] = COM_FORTIFY;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_EXPLORE)) {
			availCommands[commandCount++] = COM_EXPLORE;
		}

		availCommands[commandCount++] = COM_DISBAND;

		if (can_unit_do_activity(pUnit, ACTIVITY_ROAD) || can_unit_do_activity(pUnit, ACTIVITY_RAILROAD)) {
			availCommands[commandCount++] = COM_ROAD;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_IRRIGATE)) {
			availCommands[commandCount++] = COM_IRRIGATION;
		}

		if (can_unit_do_autosettlers(pUnit)) {
			availCommands[commandCount++] = COM_AUTO_WORKER;
		}

		if (can_unit_do_connect(pUnit, ACTIVITY_IRRIGATE)) {
			availCommands[commandCount++] = COM_CONNECT_IRRIGATION;
		}

		if (can_unit_do_connect(pUnit, ACTIVITY_ROAD)) {
			availCommands[commandCount++] = COM_CONNECT_ROAD;
		}

		if (can_unit_do_connect(pUnit, ACTIVITY_RAILROAD)) {
			availCommands[commandCount++] = COM_CONNECT_RAILROAD;
		}

		availCommands[commandCount++] = COM_WAIT;

		if (unit_can_help_build_wonder_here(pUnit)) {
			availCommands[commandCount++] = COM_BUILD_WONDER;
		}

		if (unit_can_est_trade_route_here(pUnit)) {
			availCommands[commandCount++] = COM_TRADE_ROUTE;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_MINE)) {
			availCommands[commandCount++] = COM_MINE;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_TRANSFORM)) {
			availCommands[commandCount++] = COM_TRANSFORM;
		}

		pbase = get_base_by_gui_type(BASE_GUI_FORTRESS, pUnit, pUnit->tile);

		if (!pCity && pbase) {
			availCommands[commandCount++] = COM_FORTRESS;
		}

		pbase = get_base_by_gui_type(BASE_GUI_AIRBASE, pUnit, pUnit->tile);

		if (!pCity && pbase) {
			availCommands[commandCount++] = COM_AIRBASE;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_POLLUTION)) {
			availCommands[commandCount++] = COM_POLLUTION;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_FALLOUT)) {
			availCommands[commandCount++] = COM_FALLOUT;
		}

		if (can_unit_paradrop(pUnit)) {
			availCommands[commandCount++] = COM_PARADROP;
		}

		if (can_unit_do_activity(pUnit, ACTIVITY_PILLAGE)) {
			availCommands[commandCount++] = COM_FALLOUT;
		}

	    if (pCity && can_unit_change_homecity(pUnit)
			&& pCity->id != pUnit->homecity) {
	    	  availCommands[commandCount++] = COM_HOMECITY;
	    }

	    if (pUnit->occupy && get_transporter_occupancy(pUnit) > 0) {
	    	availCommands[commandCount++] = COM_UNLOAD_TRANSPORT;
	    }

	    if (units_can_load(punits)) {
	    	availCommands[commandCount++] = COM_LOAD;
	    }

	    if (units_can_unload(punits)) {
	    	availCommands[commandCount++] = COM_UNLOAD;
	    }

	    if (pCity && pCity->airlift) {
	    	availCommands[commandCount++] = COM_AIRLIFT;
	    }

	    if (pCity && can_upgrade_unittype(client.conn.playing, unit_type(pUnit))) {
	    	availCommands[commandCount++] = COM_UPGRADE;
	    }

	    if (diplomat_can_do_action(pUnit, DIPLOMAT_ANY_ACTION, pUnit->tile)) {
			availCommands[commandCount++] = COM_DIPLOMAT;
		}

	    if (unit_has_type_flag(pUnit, F_NUCLEAR)) {
	    	availCommands[commandCount++] = COM_NUKE;
	    }

	}



	jbyteArray commands;

	commands = (*env)->NewByteArray(env, commandCount);

	(*env)->SetByteArrayRegion(env, commands, 0,
			commandCount, (jbyte *)availCommands);

	return commands;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_sendNativeCommand
  (JNIEnv * env, jobject obj, jint command) {

	civ_lock();
	switch (command) {
	case COM_GO_TO: key_unit_goto(); break;
	case COM_BUILD_CITY: key_unit_build_city(); break;
	case COM_END_TURN: dsend_packet_player_phase_done(&client.conn, game.info.turn); break;
	case COM_EXPLORE: key_unit_auto_explore(); break;
	case COM_AUTO_WORKER: key_unit_auto_settle(); break;
	case COM_SENTRY: key_unit_sentry(); break;
	case COM_FORTIFY: key_unit_fortify(); break;
	case COM_DISBAND: key_unit_disband(); break;
	case COM_ROAD: key_unit_road(); break;
	case COM_IRRIGATION: key_unit_irrigate(); break;
	case COM_CONNECT_ROAD: key_unit_connect(ACTIVITY_ROAD); break;
	case COM_CONNECT_IRRIGATION: key_unit_connect(ACTIVITY_IRRIGATE); break;
	case COM_CONNECT_RAILROAD: key_unit_connect(ACTIVITY_RAILROAD); break;
	case COM_WAIT: key_unit_wait(); break;
	case COM_BUILD_WONDER: key_unit_build_wonder(); break;
	case COM_TRADE_ROUTE: key_unit_trade_route(); break;
	case COM_MINE: key_unit_mine(); break;
	case COM_TRANSFORM: key_unit_transform(); break;
	case COM_FORTRESS: key_unit_fortress(); break;
	case COM_AIRBASE: key_unit_airbase(); break;
	case COM_POLLUTION: key_unit_pollution(); break;
	case COM_FALLOUT: key_unit_fallout(); break;
	case COM_PARADROP: key_unit_paradrop(); break;
	case COM_PILLAGE: key_unit_pillage(); break;
	case COM_HOMECITY: key_unit_homecity(); break;
	case COM_UNLOAD_TRANSPORT: key_unit_unload_all(); break;
	case COM_LOAD:
		unit_list_iterate(get_units_in_focus(), punit) {
			request_unit_load(punit, NULL);
		} unit_list_iterate_end;
		break;
	case COM_UNLOAD:
	      unit_list_iterate(get_units_in_focus(), punit) {
	    	  request_unit_unload(punit);
	      } unit_list_iterate_end;
	      break;
	case COM_AIRLIFT: /* TODO: Implement */ break;
	case COM_UPGRADE: /* TODO: Implement */ break;
	case COM_DIPLOMAT: key_unit_diplomat_actions(); break;
	case COM_NUKE: key_unit_nuke(); break;

	}
	civ_unlock();
	flush_dirty();


}

// TODO: Refactor the dialog based code into callbacks
JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_cityNameResponse
  (JNIEnv * env, jobject obj, jstring name, jint unitid) {
	jboolean iscopy;
	const char *cityname = (*env)->GetStringUTFChars(
	                env, name, &iscopy);
	LOGI("CNR");
	civ_lock();
	dsend_packet_unit_build_city(&client.conn, unitid, cityname);
	civ_unlock();

	(*env)->ReleaseStringUTFChars(env, name, cityname);
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_tellServer
  (JNIEnv * env, jobject obj, jstring cmd) {
	jboolean iscopy;
	const char *srvrcmd = (*env)->GetStringUTFChars(
	                env, cmd, &iscopy);

	civ_lock();
	send_chat(srvrcmd);
	civ_unlock();

	(*env)->ReleaseStringUTFChars(env, cmd, srvrcmd);
}

/*JNIEXPORT jobject JNICALL Java_net_hackcasual_freeciv_NativeHarness_getCityById
  (JNIEnv * env, jobject obj, jint cityPtr, jint x, jint y) {
	struct city * pcity = (struct city *)cityPtr;

	int row, col;
	struct canvas *city_map_canvas;
	civ_lock();
	canvas_to_city_pos(&col, &row, x, y);
	city_toggle_worker(pcity, col, row);



	  city_map_canvas = canvas_create(get_citydlg_canvas_width(),
	                                  get_citydlg_canvas_height());


	  city_dialog_redraw_map(pcity, city_map_canvas);
	  civ_unlock();

	  memcpy(bufferData, city_map_canvas->data, city_map_canvas->height * city_map_canvas->width * 4);

	canvas_free(city_map_canvas);


}*/

#include "improvement.h"

JNIEXPORT jobjectArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_loadImprovements
  (JNIEnv * env, jobject obj) {
	int num = improvement_count();
	int i;
	jobjectArray result;
	jmethodID cid;
	jclass imprCls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/Improvement");

	if (!imprCls) {
		LOGE("Couldn't find the improvement class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, imprCls,
	                               "<init>", "(Ljava/lang/String;ILjava/lang/String;IIII)V");

	if (!cid) {
		LOGE("Couldn't find the improvement constructor");
		return NULL;
	}

	result = (*env)->NewObjectArray(env, num, imprCls,
	                                     NULL);


	struct sprite* impr_sprite;

	for (i = 0; i < num; i++) {
		struct impr_type *impr = valid_improvement_by_number(i);

		if (impr) {
			jstring name = ((*env)->NewStringUTF(env, impr->name.vernacular));
			jstring help = ((*env)->NewStringUTF(env, impr->helptext));

			impr_sprite = get_building_sprite(tileset, impr);

			memcpy(bufferData, impr_sprite->data, impr_sprite->width * impr_sprite->height * 2);

			int advance_id;

			  requirement_vector_iterate(&impr->reqs, preq) {
				  if (VUT_ADVANCE == preq->source.kind) {
					  advance_id = preq->source.value.advance->item_number;
				  }
			  } requirement_vector_iterate_end;

			(*env)->SetObjectArrayElement(env, result, i, (*env)->NewObject(env, imprCls, cid, name, impr->item_number, help, impr->genus,
					advance_id, impr_sprite->width, impr_sprite->height));

			LOGI("Added improvement %s", impr->name.vernacular);
		}
	}

	return result;
}

#include "unittype.h"

JNIEXPORT jobjectArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_loadUnitTypes
  (JNIEnv * env, jobject obj) {
	int num = utype_count();
	int i;
	jobjectArray result;
	jmethodID cid;
	jclass unitCls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/UnitType");

	if (!unitCls) {
		LOGE("Couldn't find the unit class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, unitCls,
	                               "<init>", "(Ljava/lang/String;ILjava/lang/String;IIIIIIII)V");

	if (!cid) {
		LOGE("Couldn't find the unit constructor");
		return NULL;
	}

	result = (*env)->NewObjectArray(env, num, unitCls,
	                                     NULL);


	struct sprite* unit_sprite;

	for (i = 0; i < num; i++) {
		struct unit_type *putype = utype_by_number(i);

		if (putype) {
			jstring name = ((*env)->NewStringUTF(env, putype->name.vernacular));
			jstring help = ((*env)->NewStringUTF(env, putype->helptext));

			unit_sprite = get_unittype_sprite(tileset, putype);

			memcpy(bufferData, unit_sprite->data, unit_sprite->width * unit_sprite->height * 2);

			(*env)->SetObjectArrayElement(env, result, i, (*env)->NewObject(env, unitCls, cid, name, putype->item_number, help, unit_sprite->width, unit_sprite->height,
					putype->hp, putype->attack_strength, putype->defense_strength, putype->move_rate, putype->transport_capacity, putype->require_advance->item_number));
		}
	}

	return result;
}

#include "government.h"

JNIEXPORT jobjectArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_loadGovernments
  (JNIEnv * env, jobject obj) {
	int num = government_count();
	int i;
	jobjectArray result;
	jmethodID cid;
	jclass govCls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/Government");

	if (!govCls) {
		LOGE("Couldn't find the government class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, govCls,
	                               "<init>", "(Ljava/lang/String;ILjava/lang/String;III)V");

	if (!cid) {
		LOGE("Couldn't find the government constructor");
		return NULL;
	}

	result = (*env)->NewObjectArray(env, num, govCls,
	                                     NULL);

	struct sprite* gov_sprite;

	for (i = 0; i < num; i++) {
		struct government *pgov = government_by_number(i);

		if (pgov) {
			jstring name = ((*env)->NewStringUTF(env, pgov->name.vernacular));
			jstring help = ((*env)->NewStringUTF(env, pgov->helptext));

			int advance_id;

			  requirement_vector_iterate(&pgov->reqs, preq) {
				  if (VUT_ADVANCE == preq->source.kind) {
					  advance_id = preq->source.value.advance->item_number;
				  }
			  } requirement_vector_iterate_end;

			gov_sprite = get_government_sprite(tileset, pgov);
			LOGI("Government sprite: %dx%d", gov_sprite->width, gov_sprite->height);
			memcpy(bufferData, gov_sprite->data, gov_sprite->width * gov_sprite->height * 2);

			(*env)->SetObjectArrayElement(env, result, i, (*env)->NewObject(env, govCls, cid, name, pgov->item_number, help, gov_sprite->width, gov_sprite->height, advance_id));
		}
	}

	return result;
}

#include "game.h"
#include "city.h"

struct canvas *city_map_canvas = 0;

JNIEXPORT jobject JNICALL Java_net_hackcasual_freeciv_NativeHarness_getCity
  (JNIEnv * env, jobject obj, jint id) {
	jmethodID cid;
	jclass cls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/City");

	if (!cls) {
		LOGE("Couldn't find the city class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, cls,
	                               "<init>", "(Ljava/lang/String;IIIIIIIIIIIIIIIIIII)V");

	if (!cid) {
		LOGE("Couldn't find the unit constructor");
		return NULL;
	}

	struct city * pcity = game_find_city_by_number(id);

	if (!city_map_canvas) {
		city_map_canvas = canvas_create(get_citydlg_canvas_width(),
									  get_citydlg_canvas_height());
	}



	city_dialog_redraw_map(pcity, city_map_canvas);

	memcpy(bufferData, city_map_canvas->data, city_map_canvas->width * city_map_canvas->height * CANVAS_BYTE_WIDTH);

	int build_type = 0;
	int build_id = 0;

	if (VUT_UTYPE == pcity->production.kind) {
		build_type = 1;
		build_id = pcity->production.value.utype->item_number;
	} else if (VUT_IMPROVEMENT == pcity->production.kind){
		build_type = 0;
		build_id = pcity->production.value.building->item_number;
	}



	jstring name = ((*env)->NewStringUTF(env, pcity->name));
//O_SHIELD, O_TRADE, O_GOLD, O_LUXURY, O_SCIENCE, O_LAST
	jobject builtCity = (*env)->NewObject(env, cls, cid,
			name,
			pcity->id,
			city_map_canvas->width,
			city_map_canvas->height,
			pcity->size,
			pcity->prod[O_FOOD],
			pcity->surplus[O_FOOD],
			pcity->prod[O_SHIELD],
			pcity->surplus[O_SHIELD],
			pcity->prod[O_TRADE],
			pcity->surplus[O_TRADE],
			pcity->prod[O_GOLD],
			pcity->surplus[O_GOLD],
			pcity->prod[O_SCIENCE],
			pcity->prod[O_LUXURY],
			build_type,
			build_id,
			city_production_turns_to_build(pcity, TRUE),
			pcity->shield_stock,
	  	    city_production_build_shield_cost(pcity)
		);

	memcpy(bufferData, mapview.store->data, city_map_canvas->width * city_map_canvas->height * CANVAS_BYTE_WIDTH);

	return builtCity;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_pullOverview
  (JNIEnv * env, jobject obj) {
	memcpy(bufferData, overview_canvas->data, overview_canvas->width * overview_canvas->height * CANVAS_BYTE_WIDTH);
}

#include "tech.h"

JNIEXPORT jobjectArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_loadAdvances
  (JNIEnv * env, jobject obj) {
	int num = advance_count();
	int i;
	jobjectArray result;
	jmethodID cid;
	jclass cls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/Advance");

	if (!cls) {
		LOGE("Couldn't find the advance class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, cls,
	                               "<init>", "(Ljava/lang/String;ILjava/lang/String;IIIII)V");

	if (!cid) {
		LOGE("Couldn't find the advance constructor");
		return NULL;
	}

	result = (*env)->NewObjectArray(env, num, cls,
	                                     NULL);

	struct sprite* sprite;

	int r1, r2, rr;

	for (i = 0; i < num; i++) {
		struct advance *adv = valid_advance_by_number(i);

		int w,h;

		if (adv) {
			jstring name = ((*env)->NewStringUTF(env, adv->name.vernacular));
			jstring help = ((*env)->NewStringUTF(env, adv->helptext));

			sprite = get_tech_sprite(tileset, adv->item_number);


			if (sprite)
				memcpy(bufferData, sprite->data, sprite->width * sprite->height * 2);

			r1 = r2 = rr = -1;
			w = -1;
			h = -1;

			if (adv->require[AR_ONE]) {
				r1 = adv->require[AR_ONE]->item_number;
			}
			if (adv->require[AR_TWO]) {
				r2 = adv->require[AR_TWO]->item_number;
			}
			if (adv->require[AR_ROOT]) {
				rr = adv->require[AR_ROOT]->item_number;
			}

			if (sprite) {
				w = sprite->width;
				h = sprite->height;
			}


			(*env)->SetObjectArrayElement(env, result, i, (*env)->NewObject(env, cls, cid,
					name,
					adv->item_number,
					help,
					w,
					h,
					r1,
					r2,
					rr));
		}
	}

	return result;
}

JNIEXPORT jobjectArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getAdvanceExpenses
  (JNIEnv * env, jobject obj) {
	int num = advance_count();
	int i;
	jobjectArray result;
	jmethodID cid;
	jclass cls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/AdvanceExpense");

	if (!cls) {
		LOGE("Couldn't find the advanceExpense class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, cls,
	                               "<init>", "(III)V");

	if (!cid) {
		LOGE("Couldn't find the advanceExpense constructor");
		return NULL;
	}

	result = (*env)->NewObjectArray(env, num, cls,
	                                     NULL);

	advance_index_iterate(A_FIRST, i) {
		if (player_invention_reachable(client.conn.playing, i)
		 && (TECH_UNKNOWN == player_invention_state(client.conn.playing, i)
			 || TECH_PREREQS_KNOWN ==
				  player_invention_state(client.conn.playing, i))) {

			(*env)->SetObjectArrayElement(env, result, i, (*env)->NewObject(env, cls, cid,
					i,
					num_unknown_techs_for_goal(client.conn.playing, i),
					total_bulbs_required_for_goal(client.conn.playing, i)));



		}
	} advance_index_iterate_end;

	return result;
}

JNIEXPORT jobjectArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_setAdvanceGoal
  (JNIEnv * env, jobject obj, jint goal) {
	civ_lock();
	dsend_packet_player_tech_goal(&client.conn, goal);
	if (1 == num_unknown_techs_for_goal(client.conn.playing, goal)) {
		dsend_packet_player_research(&client.conn, goal);
		civ_unlock();
		return;
	}

	dsend_packet_player_research(&client.conn, player_research_step(client.conn.playing, goal));
	civ_unlock();
}

#include "player.h"

JNIEXPORT jobject JNICALL Java_net_hackcasual_freeciv_NativeHarness_getPlayerInfo
  (JNIEnv * env, jobject obj) {
	jmethodID cid;
	jclass cls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/Player");

	if (!cls) {
		LOGE("Couldn't find the player class!");
		return NULL;
	}

	cid = (*env)->GetMethodID(env, cls,
	                               "<init>", "(ILjava/lang/String;ZILjava/lang/String;IIIIIIIII)V");

	if (!cid) {
		LOGE("Couldn't find the player constructor");
		return NULL;
	}

	civ_lock();

	struct player * pplayer = client.conn.playing;
	struct sprite* sprite = get_nation_flag_sprite(tileset, pplayer->nation);

	int w, h;

	w = h = -1;

	if (sprite) {
		memcpy(bufferData, sprite->data, sprite->width * sprite->height * 2);
		w = sprite->width;
		h = sprite->height;
	}

	jstring name = ((*env)->NewStringUTF(env, pplayer->name));
	jstring nation_name = ((*env)->NewStringUTF(env, pplayer->nation->adjective.vernacular));



	jobject playa = (*env)->NewObject(env, cls, cid,
			player_number(pplayer),
			name,
			pplayer->is_male,
			pplayer->user_turns,
			nation_name,
			w,
			h,
			pplayer->bulbs_last_turn,
			get_player_research(pplayer)->researching,
			get_player_research(pplayer)->bulbs_researched,
			total_bulbs_required(pplayer),
			pplayer->government?pplayer->government->item_number:-1,
			pplayer->target_government?pplayer->target_government->item_number:-1,
			pplayer->revolution_finishes - game.info.turn
		);

	civ_unlock();

	return playa;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_clickCityMap
  (JNIEnv * env, jobject obj, jint cityId, jint x, jint y) {
	civ_lock();

	int col, row;

	struct city * pcity = game_find_city_by_number(cityId);

	canvas_to_city_pos(&col, &row, x, y);
	LOGI("City #%d clicked %d, %d [%d, %d]", cityId, x, y, col, row);
	city_toggle_worker(pcity, col, row);

	civ_unlock();
}

JNIEXPORT jintArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getBuildableImprovementsForCity
  (JNIEnv * env, jobject obj, jint cityId) {
	struct city * pcity = game_find_city_by_number(cityId);
	int imprCount = 0;
	int buildableImprs[U_LAST];
  improvement_iterate(pimprove) {
	  if (can_city_build_improvement_now(pcity, pimprove)) {
		  buildableImprs[imprCount] = pimprove->item_number;
		  imprCount++;
	  }
  } improvement_iterate_end;


	jintArray imprs = (*env)->NewIntArray(env, imprCount);
	(*env)->SetIntArrayRegion(env, imprs, 0, imprCount, buildableImprs);

	return imprs;
}

JNIEXPORT jintArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getBuildableUnitsForCity
  (JNIEnv * env, jobject obj, jint cityId) {

	int unitCount = 0;

	int buildableUnits[U_LAST];
	struct city * pcity = game_find_city_by_number(cityId);
	unit_type_iterate(punittype) {
		if (can_city_build_unit_now(pcity, punittype)) {
			buildableUnits[unitCount] = punittype->item_number;
			unitCount++;
		}
	} unit_type_iterate_end

	jintArray units = (*env)->NewIntArray(env, unitCount);
	(*env)->SetIntArrayRegion(env, units, 0, unitCount, buildableUnits);

	return units;
}

#include "citydlg_common.h"

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_setCityProductionToImprovement
  (JNIEnv * env, jobject obj, jint cityId, jint imprId) {

	struct universal target;

	target.kind = VUT_IMPROVEMENT;
	target.value.building = improvement_by_number(imprId);

	struct city * pcity = game_find_city_by_number(cityId);
	civ_lock();
	city_change_production(pcity, target);
	civ_unlock();
	return;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_setCityProductionToUnit
  (JNIEnv * env, jobject obj, jint cityId, jint unitId) {

	struct universal target;

	target.kind = VUT_UTYPE;
	target.value.utype = utype_by_number(unitId);

	struct city * pcity = game_find_city_by_number(cityId);
	civ_lock();
	city_change_production(pcity, target);
	civ_unlock();
	return;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_focusOnUnit
  (JNIEnv * env, jobject obj, jint unitId) {

	civ_lock();
	struct unit * punit = game_find_unit_by_number(unitId);
	set_unit_focus(punit);
	civ_unlock();
}

//

JNIEXPORT jobject JNICALL Java_net_hackcasual_freeciv_NativeHarness_getUnitById
  (JNIEnv * env, jobject obj, jint unitId) {

	struct unit * punit = game_find_unit_by_number(unitId);

	//TODO: Support alpha canvas for transparency
	int w = tileset_full_tile_width(tileset);
	int h = tileset_full_tile_height(tileset);
	struct canvas *destcanvas = canvas_create_alpha(w, h);

	memset(destcanvas->data, 0, w * h * CANVAS_BYTE_WIDTH);

	put_unit(punit, destcanvas, 0, 0);

	memcpy(bufferData, destcanvas->data, w * h * CANVAS_BYTE_WIDTH);

	canvas_free(destcanvas);

	jclass cls = (*env)->FindClass(env, "net/hackcasual/freeciv/models/Unit");

	if (!cls) {
		LOGE("Couldn't find the unit class!");
		return;
	}

	jmethodID cid = (*env)->GetMethodID(env, cls,
								   "<init>", "(IIIIIIII)V");

	if (!cid) {
		LOGE("Couldn't find the unit constructor");
		return;
	}


	return (*env)->NewObject(env, cls, cid,
			punit->id,
			punit->utype->item_number,
			0,
			punit->moves_left,
			punit->veteran,
			punit->hp,
			w,
			h
		);
}

JNIEXPORT jintArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getUnitsOnTile(JNIEnv * env, jobject obj, jint x, jint y) {

	int tile_units[1000];

	civ_lock();
	struct tile *ptile = canvas_pos_to_tile(x,y);
	civ_unlock();

	int unit_count = unit_list_size(ptile->units);

	int idx = 0;

	unit_list_iterate(ptile->units, punit) {
		tile_units[idx++] = punit->id;
	} unit_list_iterate_end;

	jintArray units = (*env)->NewIntArray(env, unit_count);
	(*env)->SetIntArrayRegion(env, units, 0, unit_count, tile_units);

	return units;
}

JNIEXPORT jintArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getUnitsInCity(JNIEnv * env, jobject obj, jint cityId) {

	int city_units[1000];
	struct city * pcity = game_find_city_by_number(cityId);

	int unit_count = unit_list_size(pcity->tile->units);

	int idx = 0;

	unit_list_iterate(pcity->tile->units, punit) {
		city_units[idx++] = punit->id;
	} unit_list_iterate_end;

	jintArray units = (*env)->NewIntArray(env, unit_count);
	(*env)->SetIntArrayRegion(env, units, 0, unit_count, city_units);

	return units;
}

JNIEXPORT jintArray JNICALL Java_net_hackcasual_freeciv_NativeHarness_getAvailableGovernments(JNIEnv * env, jobject obj) {

	int governments[10];

	int gov_count = 0;

	int i;

	for (i = 0; i < government_count(); i++) {
		struct government * target = government_by_number(i);
		if (can_change_to_government(client.conn.playing,target)) {
			governments[gov_count++] = target->item_number;
		}
	}

	jintArray govs = (*env)->NewIntArray(env, gov_count);
	(*env)->SetIntArrayRegion(env, govs, 0, gov_count, governments);

	return govs;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_save
  (JNIEnv * env, jobject obj) {
	send_chat("/save");
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_exercise1
  (JNIEnv * env, jobject obj) {


	int steps;
	int step_size = 30;
	int xdir, ydir;
	int c;

	int sc = 0;

	for (steps = 2; steps < 10; steps++) {
		xdir = 1;
		ydir = 1;


		for (c = 0; c < steps; c++) {
			civ_lock();
			can_slide = FALSE;
			set_mapview_origin(mapview.gui_x0 + step_size * xdir, mapview.gui_y0 + step_size * ydir);
			sc++;
			flush_dirty();
			civ_unlock();
		}

		xdir = -1;
		ydir = 1;

		for (c = 0; c < steps; c++) {
			civ_lock();
			can_slide = FALSE;
			set_mapview_origin(mapview.gui_x0 + step_size * xdir, mapview.gui_y0 + step_size * ydir);
			sc++;
			flush_dirty();
			civ_unlock();

		}

		xdir = -1;
		ydir = -1;

		for (c = 0; c < steps; c++) {
			civ_lock();
			can_slide = FALSE;
			set_mapview_origin(mapview.gui_x0 + step_size * xdir, mapview.gui_y0 + step_size * ydir);
			sc++;
			flush_dirty();
			civ_unlock();

		}

		xdir = 1;
		ydir = -1;

		for (c = 0; c < steps; c++) {
			civ_lock();
			can_slide = FALSE;
			set_mapview_origin(mapview.gui_x0 + step_size * xdir, mapview.gui_y0 + step_size * ydir);
			sc++;
			flush_dirty();
			civ_unlock();

		}


	}

	LOGI("Total frames: %d", sc);
	can_slide = TRUE;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_positionFromOverview
  (JNIEnv * env, jobject obj, jint overview_x, jint overview_y) {
	civ_lock();
	can_slide = FALSE;
	int map_x, map_y;
	overview_to_map_pos(&map_x, &map_y, overview_x, overview_y);
	center_tile_mapcanvas(map_pos_to_tile(map_x, map_y));
	refresh_overview_canvas();
	flush_dirty();
	civ_unlock();
	return;
}

JNIEXPORT jint JNICALL Java_net_hackcasual_freeciv_NativeHarness_getFocusedUnitType
  (JNIEnv * env, jobject obj) {

	struct unit * punit = head_of_units_in_focus();

	if (punit)
		return punit->utype->item_number;
	return -1;
}

JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_setGovernmentType
  (JNIEnv * env, jobject obj, jint govNum) {
	civ_lock();
	set_government_choice(government_by_number(govNum));
	civ_unlock();
}
