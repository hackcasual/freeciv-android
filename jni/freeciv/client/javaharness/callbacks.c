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

#include "callbacks.h"

#include "gui-android/mapview.h"

struct canvas {
	int width, height;
	void * data;
};

jobject dialog_manager = 0;

JavaVM *jvm = 0;

jmethodID new_city_dialog_mid = 0;
jmethodID city_dialog_mid = 0;
jmethodID update_messages_mid = 0;

JNIEnv *env = 0;
JNIEXPORT void JNICALL Java_net_hackcasual_freeciv_NativeHarness_registerDialogManager
  (JNIEnv * e, jobject obj, jobject dm) {
	env = e;
	int res;
	res = (*env)->GetJavaVM(env, &jvm);

	jclass cls = (*env)->GetObjectClass(env, dm);
	dialog_manager = (*env)->NewGlobalRef(env,dm);

	new_city_dialog_mid = (*env)->GetMethodID(env, cls, "showCityNameDialog", "(Ljava/lang/String;I)V");
	if (new_city_dialog_mid == 0) {
		LOGE("Failed to get showCityNameDialog");
		return;
	}

	city_dialog_mid = (*env)->GetMethodID(env, cls, "showCityDialog", "(I)V");
	if (city_dialog_mid == 0) {
		LOGE("Failed to get showCityDialog");
		return;
	}

	update_messages_mid = (*env)->GetMethodID(env, cls, "showMessages", "([Ljava/lang/String;)V");
	if (update_messages_mid == 0) {
		LOGE("Failed to get updateMessages");
		return;
	}

}

void java_new_city_dialog(char * suggestion, int unit_id) {
	int res = (*jvm)->AttachCurrentThread(jvm, &env, 0);
	jstring sugg = ((*env)->NewStringUTF(env, suggestion));
	(*env)->CallVoidMethod(env, dialog_manager, new_city_dialog_mid, sugg, unit_id);
}


void java_city_dialog(const struct city *pcity) {
	//JNIEnv *env2 = 0;
	//int res;
	int res = (*jvm)->AttachCurrentThread(jvm, &env, 0);

	(*env)->CallVoidMethod(env, dialog_manager, city_dialog_mid,
			pcity->id);

	//(*jvm)->DetachCurrentThread(jvm);
}

#include "messagewin_common.h"

void java_show_messages() {
	//JNIEnv *env2 = 0;
	//int res;

	int res = (*jvm)->AttachCurrentThread(jvm, &env, NULL);

	int i;
	int msg_count = get_num_messages();
	if (msg_count > 0) {
		 struct message *pmsg = NULL;
		jclass jstr = (*env)->FindClass(env, "java/lang/String");
		jobjectArray msgs = (*env)->NewObjectArray(env, msg_count, jstr,
                NULL);

		for (i = 0; i < msg_count; i++) {
			pmsg = get_message(i);
			(*env)->SetObjectArrayElement(env, msgs, i, (*env)->NewStringUTF(env, pmsg->descr));
		}

		int res = (*jvm)->AttachCurrentThread(jvm, &env, 0);



		(*env)->CallVoidMethod(env, dialog_manager, update_messages_mid,
				msgs);



	}
}
