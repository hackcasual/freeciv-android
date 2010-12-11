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

package net.hackcasual.freeciv;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class CivService extends Service {

	@Override
	public IBinder onBind(Intent arg0) {
		Log.i("FreecivServer", "Bind request");
		return null;
	}
	
	@Override
	public void onCreate() {
		Log.i("FreecivServer", "Starting the server");
		((Civ)this.getApplication()).getNativeHarness().runServer();
		Log.i("FreecivServer", "Running");		
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		return START_STICKY;
	}
	
    @Override
    public void onDestroy() {
    	Log.i("FreecivServer", "Destroyed");
    }

}
