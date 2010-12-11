libpng is not distributed with FreeCiv, but is needed to build on Android.

Download libpng, from here: 

https://github.com/android/platform_external_libpng

into this directory.

Then make the following modifications to Android.mk

1) Remove the "for the host" section
2) Remove the LOCAL_C_INCLUDES and LOCAL_SHARED_LIBRARIES line
3) Replace those with 

LOCAL_LDLIBS    := -lz