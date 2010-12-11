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

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import net.hackcasual.freeciv.models.Advance;
import net.hackcasual.freeciv.models.AdvanceExpense;
import net.hackcasual.freeciv.models.City;
import net.hackcasual.freeciv.models.Government;
import net.hackcasual.freeciv.models.Improvement;
import net.hackcasual.freeciv.models.Player;
import net.hackcasual.freeciv.models.Unit;
import net.hackcasual.freeciv.models.UnitType;
import net.hackcasual.freeciv.views.Freeciv;

import com.google.common.collect.BiMap;
import com.google.common.collect.HashBiMap;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.Typeface;
import android.text.Editable;
import android.util.Log;
import android.widget.EditText;
import android.widget.ImageView;

public class NativeHarness {
	private DialogManager dm;
	private Freeciv mainActivity;
	private final static NativeHarness nh = new NativeHarness();
	private Semaphore displayLock = new Semaphore(1);
	private String lastTilesetProgress = null;
	private boolean lastConnectionStatus = false;
	
	
	private NativeEventListener nativeListener = null;
	
	public static NativeHarness getHarness() {
		return nh;
	}
	
	private NativeHarness() {
		System.loadLibrary("server");
		System.loadLibrary("client");
		serverRunning = false;
		clientRunning = false;
		this.mainActivity = null;
		this.dm = new DialogManager(this);
	}
	
	public void setMainActivity(Freeciv act) {
		this.mainActivity = act;
	}
	
	public DialogManager getDialogManager() {
		//this.dm = dm;
		return dm;
	}

	Bitmap overview = Bitmap.createBitmap(156, 104, Bitmap.Config.RGB_565);
	
	public native void startServer();
	public native void startClient();
	public static native void init(int width, int height);
	public synchronized static native int touchEvent(int x, int y, int type);

	public native void registerDialogManager(DialogManager dm);
	public native void reloadMap();
	
	public native void cityNameResponse(String name, int unit_id);
	
	public native void clickCityMap(int cityPtr, int x, int y);	
	
	private native int[] getUnitsInCity(int cityId);
	public native int[] getUnitsOnTile(int x, int y);
	public native Unit getUnitById(int unitId);
	
	public List<Unit> getUnitsInCity(City city) {
		List<Unit> units = new ArrayList<Unit>();
		
		for (int id: getUnitsInCity(city.getCityId())) {
			units.add(getUnitById(id));
		}
		
		return units;
	}
	
	private native byte[] getAvailableCommandsForActiveUnit();
	private native void sendNativeCommand(int command);
	
	public native Improvement[] loadImprovements();
	public native UnitType[] loadUnitTypes();
	public native Advance[] loadAdvances();
	public native Government[] loadGovernments();
	public native AdvanceExpense[] getAdvanceExpenses();
	public native City getCity(int id);
	public static native int[] getBuildableImprovementsForCity(int id);
	public static native int[] getBuildableUnitsForCity(int id);
	public static native int[] getAvailableGovernments();
	public static native Player getPlayerInfo();
	public static native void setAdvanceGoal(int goalId);
	public native void focusOnUnit(int unitId);
	
	public native void positionFromOverview(int x, int y);
	
	public native void setGovernmentType(int governmentId);
	
	public static native void setCityProductionToImprovement(int cityId, int imprId);
	public static native void setCityProductionToUnit(int cityId, int unitId);
	
	public static native void save();
	
	public static native void tellServer(String cmd);
	public static native void exercise1();
	
	public native int getFocusedUnitType();
	
	private native void pullOverview();
	
	private boolean serverRunning, clientRunning;
	
	private ByteBuffer nativeBuffer;
	private ByteBuffer javaBuffer;
	
	public void registerNativeBuffers(ByteBuffer nativeToJavaBuffer, ByteBuffer javaToNativeBuffer) {
		//Log.i("Freeciv","Got a bitmap buffer");
		this.nativeBuffer = nativeToJavaBuffer;
		this.javaBuffer = javaToNativeBuffer;
	}
	
	public Bitmap getOverview() {
		pullOverview();		
		overview.copyPixelsFromBuffer(getIncomingBuffer());
		return overview;
	}
	
	public int getTextSize(String s) {
		Paint fontPaint = new Paint();
		
		fontPaint.setColor(0xFFFFFFFF);
		
		fontPaint.setTypeface(Typeface.SANS_SERIF);
		fontPaint.setTextSize(18.0f);
		fontPaint.setAntiAlias(true);
		Rect bounds = new Rect();
		
		fontPaint.getTextBounds(s, 0, s.length(), bounds);
				
		int packedReturn = ((bounds.width() << 16) | (int)(fontPaint.descent() - fontPaint.ascent())); 
		
		Log.i("Freeciv", String.format("Measured size of %s [%dx%d] %x", s, bounds.width(), (int)(fontPaint.descent() - fontPaint.ascent()), packedReturn));
		
		return packedReturn;
	}

	public int renderString(final String s) {
		Paint fontPaint = new Paint();

		fontPaint.setColor(0xFFFFFFFF);
		
		fontPaint.setTypeface(Typeface.SANS_SERIF);
		fontPaint.setTextSize(18.0f);
		
		
		fontPaint.setAntiAlias(true);
		
		Rect bounds = new Rect();
		
		fontPaint.getTextBounds(s, 0, s.length(), bounds);
				
		int packedReturn = ((bounds.width() << 16) | (int)(fontPaint.descent() - fontPaint.ascent())); 
		
		final Bitmap textAsSprite = Bitmap.createBitmap(bounds.width(), (int)(fontPaint.descent() - fontPaint.ascent()), Bitmap.Config.ARGB_4444);
		Canvas cvs = new Canvas(textAsSprite);
		cvs.drawARGB(0, 0, 0, 0);
		cvs.drawText(s, 0f, -fontPaint.ascent(), fontPaint);
		
		javaBuffer.rewind();
		textAsSprite.copyPixelsToBuffer(javaBuffer);
		//packedRetu
		
		
		return packedReturn;
	}
	
	public void updateDisplay() {
		Log.i("Freeciv", String.format("Updating display, acquiring display lock: Avail: %d: TID: %d", displayLock.availablePermits(), Thread.currentThread().getId()));
		try {
			displayLock.acquire();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (mainActivity != null) {
			mainActivity.updateMapview(nativeBuffer, displayLock);			
		} else {
			Log.i("Freeciv", "Premature release");
			displayLock.release();
		}
		Log.i("Freeciv", "Done updating display, acquiring to sync with gui thread");
		try {
			displayLock.tryAcquire(100, TimeUnit.MILLISECONDS);
		} catch (InterruptedException e) {
			//Ignore
			Log.i("Freeciv","Gave up on display lock");			
		}
		Log.i("Freeciv", String.format("acquired, now release lock: Avail: %d TID: %d", displayLock.availablePermits(), Thread.currentThread().getId()));
		displayLock.release();
		Log.i("Freeciv", "released");
	}
	
	
	
	public void clientConnected() {
		/*send_chat("/set dispersion=5");
		send_chat("/start");*/
		
		/*tellServer("/set dispersion=5");
		tellServer("/load /sdcard/Freeciv/civgame-T0000-Y-4000-manual.sav");*/
		//tellServer("/start");
		
		lastConnectionStatus = true;
		
		if (nativeListener != null) {
			nativeListener.setConnectionStatus(true);

		}
		
	}
	
	public Buffer getIncomingBuffer() {
		return nativeBuffer;
	}
		
	public enum AvailableCommand {
		GO_TO, 
		BUILD_CITY,
		FORTIFY,
		SENTRY,
		EXPLORE,
		GO_TO_CITY,
		DISBAND,
		ROAD,
		IRRIGATION,
		AUTO_WORKER,
		CONNECT_ROAD,
		CONNECT_IRRIGATION,
		CONNECT_RAILROAD,
		WAIT,
		BUILD_WONDER,
		TRADE_ROUTE,
		MINE,
		TRANSFORM,
		FORTRESS,
		AIRBASE,
		POLLUTION,
		FALLOUT,
		PARADROP,
		PILLAGE,
		HOMECITY,
		UNLOAD_TRANSPORT,
		LOAD,
		UNLOAD,
		AIRLIFT,
		RETURN,
		PATROL,
		UPGRADE,
		DIPLOMAT,
		NUKE;
	}

	
	private final static BiMap<Integer, AvailableCommand> codeMap;
	
	static {
		codeMap = HashBiMap.create();
		
		for (AvailableCommand ac: AvailableCommand.values()) {
			codeMap.put(ac.ordinal(), ac);
		}
	}
	
	public List<AvailableCommand> getAvailableCommandsForUnit() {
		byte[] comms = getAvailableCommandsForActiveUnit();
		List<AvailableCommand> rval = new ArrayList<AvailableCommand>();
 		for (Byte b: comms) {
 			Log.i("Freeciv", String.format("Got a command: %d [%s]", b, codeMap.get((int)b).toString()));
			rval.add(codeMap.get((int)b));
		}
 		Log.i("Freeciv", rval.toString());
 		return rval;
	}
	
	public void sendCommand(int command) {
		sendNativeCommand(command);
	}

	
	
	public void runServer() {
		if (!serverRunning) {
			serverRunning = true;
			(new ServerThread(this)).start();
		}
	}

	public void runClient() {
		if (!clientRunning) {
			clientRunning = true;
			(new ClientThread(this)).start();
		}
	}

	public void updateTilesetProgress(String chunk) {
		Log.i("Freeciv", String.format("Got a tileset chunk: %s", chunk));
		lastTilesetProgress = chunk;
		if (this.nativeListener != null) {
			this.nativeListener.recieveTilesetUpdate(chunk);
		}
	}
	
	public void hookNativeEventListener(NativeEventListener nel) {
		nativeListener = nel;
		if (lastTilesetProgress != null)
			nativeListener.recieveTilesetUpdate(lastTilesetProgress);
		nativeListener.setConnectionStatus(lastConnectionStatus);
	}
	
	public void unhookNativeEventListener(NativeEventListener nel) {
		// Prevent an OoO situation from trashing the listener
		if (this.nativeListener == nel)
			this.nativeListener = null;		
	}
	
	private class ServerThread extends Thread {
		private NativeHarness container;
		public ServerThread(NativeHarness container) {
			this.container = container;
		}
		public void run() {
			container.startServer();
		}
	}
	
	private class ClientThread extends Thread {
		private NativeHarness container;
		public ClientThread(NativeHarness container) {
			this.container = container;
		}
		public void run() {
			container.registerDialogManager(dm);
			container.startClient();
		}
	}	
}
