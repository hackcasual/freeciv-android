# Android-Freeciv - Copyright (C) 2010 - C Vaughn
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2, or (at your option)
#   any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ai

LOCAL_SRC_FILES += $(notdir $(wildcard $(LOCAL_PATH)/*.c))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
				$(LOCAL_PATH)/../common \
				$(LOCAL_PATH)/../common/aicore \
				$(LOCAL_PATH)/../utility \
				$(LOCAL_PATH)/../server \
				$(LOCAL_PATH)/../server/generator

LOCAL_CFLAGS :=	-DHAVE_NETINET_IN_H \
				-DHAVE_SYS_SELECT_H \
				-DHAVE_SYS_SOCKET_H \
				-DHAVE_SYS_TIME_H \
				-DHAVE_SYS_TYPES_H \
				-DHAVE_UNISTD_H	\
				-DHAVE_SOCKLEN_T \
                -DHAVE_CONFIG_H \
                -DHAVE_ARPA_INET_H \
                -DHAVE_NETDB_H \
                -DHAVE_PWD_H \
                -DHAVE_SYS_UIO_H \
                -DHAVE_SYS_UTSNAME_H \
                -DHAVE_LIBZ
                								
include $(BUILD_STATIC_LIBRARY)