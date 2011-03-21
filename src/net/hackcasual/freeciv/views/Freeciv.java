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

package net.hackcasual.freeciv.views;

import java.nio.ByteBuffer;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;

import net.hackcasual.freeciv.CityPresentUnitAdapter;
import net.hackcasual.freeciv.Civ;
import net.hackcasual.freeciv.DialogManager;
import net.hackcasual.freeciv.NativeHarness;
import net.hackcasual.freeciv.NativeHarness.AvailableCommand;
import net.hackcasual.freeciv.Settings;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;
import net.hackcasual.freeciv.models.*;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.ScaleDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Vibrator;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SubMenu;
import android.view.TouchDelegate;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class Freeciv extends NativeAwareActivity {
	NativeHarness nh;
	List<AvailableCommand> currentOptions;
	DialogManager dm;
	static Bitmap mapView;
	boolean unitMenu = false;
	long tsBuf[] = new long[10];
	int tsi = -1, tsc = 0;
	AlertDialog overviewDialog;
	boolean isPaused;
	int lastSavedTurn;
	GestureDetector gestures;
	MotionEvent lastLongPress, lastMoveAfterLongPress;
	
	 DisplayMetrics metrics;
	 
	 
	 public Freeciv() {
		 metrics = new DisplayMetrics();
	 }
	 

	
	final BlockingQueue<MotionEvent> touchQueue = new LinkedBlockingQueue<MotionEvent>();
	
	public static Bitmap getMapView() {
		return mapView;
	}
		
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Settings.updateSettings(this);
        
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
    	setContentView(R.layout.main);
    	
        nh = ((Civ)(this.getApplication())).getNativeHarness();

        nh.getDialogManager().bindActivity(this);
        nh.setMainActivity(this);
      
        Display display = getWindowManager().getDefaultDisplay(); 
        int width = getMapWidth(display);
        int height = getMapHeight(display);;
        
        NativeHarness.init(width, height);
        
        mapView = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        
        
        ((ImageView)findViewById(R.id.map_view)).setImageBitmap(mapView);
        ((ImageView)findViewById(R.id.map_view)).getDrawable().setFilterBitmap(false);
        
        final Freeciv me = this;
        

        
        /*(new Thread() {
        	public void run() {
        		MotionEvent toProcess = null;
        		while (true) {
        			try {
	        			if (touchQueue.isEmpty()) {
	        				toProcess = touchQueue.take();
	        			} else {
	        				while (!touchQueue.isEmpty()) {
	        					toProcess = touchQueue.take();
	        				}
	        			}
	        			
        			} catch (InterruptedException e) {
        				//Nothing
        			}
        			int unitCount = NativeHarness.touchEvent((int)toProcess.getX(), (int)toProcess.getY(), toProcess.getAction());
        			if (unitCount > 0) {
	        			if (unitCount == 1) {
	        				me.runOnUiThread(new Runnable() {
								@Override
								public void run() {
			        				me.showUnitMenu();
								}
	        				});
	        			} else {
	        				final int x = (int)toProcess.getX();
	        				final int y = (int)toProcess.getY();
	        				me.runOnUiThread(new Runnable() {
								@Override
								public void run() {
			        				me.showUnitSelection(x, y);
								}
	        				});	        				
	        			}
        			}
        			
        		}
        	}
        }).start();*/
    }
    
    @Override
    public void onResume() {
    	super.onResume();
    	this.isPaused = false;
        
    	gestures = new GestureDetector (this, new MapViewGestureProcessor());



    	//nh.reloadMap();
    	//nh.updateDisplay();
    }

    @Override
    public void onPause() {
    	super.onPause();
    	Civ.saveGame(Civ.SaveType.FORCED);
    	this.isPaused = true;
    }    
    
    private void stopTrackingPushMove() {
		lastLongPress = null;
		lastMoveAfterLongPress = null;
    }

    private boolean isTrackingPushMove() {
		return (lastLongPress != null && lastMoveAfterLongPress != null);
    }

    
    @Override
	public boolean dispatchTouchEvent(MotionEvent event) {
    	if (event.getAction() == MotionEvent.ACTION_MOVE && lastLongPress != null) {
    		NativeHarness.showCursorAt((int)event.getX() / 2, (int)event.getY() / 2);
    		lastMoveAfterLongPress = event;
    		return true;
    	} else if (isTrackingPushMove() && event.getAction() == MotionEvent.ACTION_DOWN &&
    		Math.abs(event.getX() - lastMoveAfterLongPress.getX()) < 40 && Math.abs(event.getY() - lastMoveAfterLongPress.getY()) < 40) {
    		//Resume moving the selector
    		return gestures.onTouchEvent(event);
    	} else if (event.getAction() != MotionEvent.ACTION_UP){
    		stopTrackingPushMove();
    		NativeHarness.cancelAction();
    	}
    	
    	return gestures.onTouchEvent(event);
	}
	
	public void showUnitMenu() {
		unitMenu = true;
		openOptionsMenu();
		unitMenu = false;
	}

	public void showUnitSelection(int x, int y) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		
		builder.setTitle("Choose Unit");
		ListView units = new ListView(this);
		units.setFocusable(true);
		final CityPresentUnitAdapter adapter = new CityPresentUnitAdapter(this);
		units.setAdapter(adapter);
		//builder.setView(units);
		
		for (int i: NativeHarness.getUnitsOnTile(x, y)) {
			adapter.add(NativeHarness.getUnitById(i));
		}

		builder.setView(units);
		final AlertDialog shown = builder.create();		
		units.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int pos,
					long arg3) {
				nh.focusOnUnit(adapter.getItem(pos).getUnitId());
				shown.dismiss();
				unitMenu = true;
				openOptionsMenu();
				unitMenu = false;
				
			}
			
		});

		

		shown.show();

	}

	public void updateMapview(final ByteBuffer image, final Semaphore renderLock) {
		
		/*if (this.isPaused) {
			renderLock.release();
			return;
		}*/
		
		Log.i("Freeciv", String.format("Updating mapview on thread: %d", Thread.currentThread().getId()));
		
		this.runOnUiThread(new Runnable() {

			@Override
			public void run() {
				Log.i("Freeciv", String.format("In UI thread %d", Thread.currentThread().getId()));
				
				image.rewind();
				mapView.copyPixelsFromBuffer(image);
				((ImageView)findViewById(R.id.map_view)).setImageBitmap(mapView);
				((ImageView)findViewById(R.id.map_view)).getDrawable().setFilterBitmap(false);
				//renderLock.release();
				Log.i("Freeciv", "gui thread released lock");
			}
		
		});
	}
	
	void runExercise(final Activity that) {
		Thread t = new Thread() {

			@Override
			public void run() {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				Log.i("Freeciv", "Running exercise1");
				
				long curTime = System.currentTimeMillis();
				
				NativeHarness.exercise1();

				final long t1 = System.currentTimeMillis() - curTime;
				that.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						((TextView)findViewById(R.id.msg_bar)).setText(String.format("Exercise 1[1] took: %d : ms", t1));							
					}
				
				});

				
				curTime = System.currentTimeMillis();
				
				NativeHarness.exercise1();
				final long t2 = System.currentTimeMillis() - curTime;
				that.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						((TextView)findViewById(R.id.msg_bar)).setText(String.format("Exercise 1[2] took: %d : ms", t2));							
					}
				
				});
				


				curTime = System.currentTimeMillis();
				
				NativeHarness.exercise1();
				final long t3 = System.currentTimeMillis() - curTime;
				that.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						((TextView)findViewById(R.id.msg_bar)).setText(String.format("Exercise 1[3] took: %d : ms", t3));							
					}
				
				});



			}
			
		};
		
		t.start();
	}
	
	public void writeText(final String s, final Paint p, final int x, final int y) {
		this.runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				Canvas cvs = new Canvas(mapView);
				cvs.drawText(s, x, y, p);
				((ImageView)findViewById(R.id.map_view)).setImageBitmap(mapView);
			}
		
		});
	}
	
	public boolean onPrepareOptionsMenu (Menu menu) {
		menu.clear();
		currentOptions = nh.getAvailableCommandsForUnit();
		if (unitMenu) {
						
			int count = 0;
			SubMenu more = null;
			for (AvailableCommand command: currentOptions) {
				Log.d("Freeciv", String.valueOf(command));
	
				//menu.add(command.name());
				
				if (++count > 5 && currentOptions.size() > 6) {
					if (6 == count)
						more = menu.addSubMenu("More");
					
					more.add(0, command.ordinal(), 0, command.name());
				} else {
					menu.add(0, command.ordinal(), 0, command.name());
				}
				
			}
		} else {
			menu.clear();
			
			menu.add(0, 100, 0, "End Turn");
			
			if (currentOptions.size() > 0) {
				SubMenu unitOrders = menu.addSubMenu(0, 101, 0, Civ.getUnitTypeById(nh.getFocusedUnitType()).getName());
				
				BitmapDrawable bd = Civ.getUnitTypeById(nh.getFocusedUnitType()).getIcon();
				unitOrders.setIcon(bd);
				
				for (AvailableCommand command: currentOptions) {
					Log.d("Freeciv", String.valueOf(command));				
					unitOrders.add(0, command.ordinal(), 0, command.name());
				}
			}
			
			menu.add(0, 102, 0, "Adjust Research");
			menu.add(0, 103, 0, "Status");
			menu.add(0, 104, 0, "Save");
			//TODO: Only cancel if we are currently going to perform an action
			//TODO: or maybe cancel anytime we hit menu
			menu.add(0, 105, 0, "Cancel");
		}
		return true;
	}

	public boolean onOptionsItemSelected(MenuItem item) {
		
		
		switch (item.getItemId()) {
		case 101: showUnitMenu(); break;
		case 102: showResearchActivity(); break;
		case 103: showPlayerInfo(); break;
		case 104: Civ.saveGame(Civ.SaveType.MANUAL); break;
		default: nh.sendCommand(item.getItemId()); break;
		}
		
		

		return true;
	}
	
	void showResearchActivity() {
		Intent researchViewer = new Intent(this, ResearchView.class);
		
		startActivity(researchViewer);
	}
	
	void showPlayerInfo() {
		Intent playerViewer = new Intent(this, PlayerView.class);
		
		startActivity(playerViewer);
	}

	@Override
	public void recieveTilesetUpdate(String info) {
		// TODO Auto-generated method stub
		
	}
	
	@Override
	public void setConnectionStatus(boolean status) {
		if (status && !isConnected) {
			((Civ)this.getApplicationContext()).loadUniversals();
			isConnected = true;
			NativeHarness.tellServer("/set dispersion=5");
			NativeHarness.tellServer("/set saveturns=0");
			
			if (this.getIntent().hasExtra(LoadGame.SAVED_GAME_TAG)) {
				String savedGame = this.getIntent().getStringExtra(LoadGame.SAVED_GAME_TAG);
				NativeHarness.tellServer(String.format("/load %s", savedGame));
			}
			
			NativeHarness.tellServer("/start");
		}
	}
	
    @Override 
    public void onConfigurationChanged(Configuration newConfig) { 
        super.onConfigurationChanged(newConfig); 
        
        if (overviewDialog != null && overviewDialog.isShowing())
        	overviewDialog.cancel();
        
        Display display = getWindowManager().getDefaultDisplay(); 
        int width = getMapWidth(display);
        int height = getMapHeight(display);
        
        NativeHarness.init(width, height);
        
        mapView = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        
        ((ImageView)findViewById(R.id.map_view)).setImageBitmap(mapView);
    }
    
    @Override
    public boolean onSearchRequested() {
    	//((ImageView)findViewById(R.id.map_view)).setImageBitmap(nh.getOverview());
    	
    	AlertDialog.Builder builder = new AlertDialog.Builder(this);// overviewDialog = new AlertDialog(this);

        Display display = getWindowManager().getDefaultDisplay(); 
        int width = display.getWidth();
        int height = display.getHeight();
        
        float imageScale = 0.0f;
        int image_width, image_height;
        
        Bitmap theOverview = NativeHarness.getOverview();
        
        if (width < height) {
        	float destWidth = width * 0.66f;
        	imageScale = destWidth / theOverview.getWidth();        		
        } else {
        	float destHeight = height * 0.55f;
        	imageScale = destHeight / theOverview.getHeight();        		        	
        }

    	if (imageScale > 2.5f)
    		imageScale = 2.5f; 
    	
    	image_width = (int) (theOverview.getWidth() * imageScale);
    	image_height = (int) (theOverview.getHeight() * imageScale);
        
    	final ImageView overviewView = new ImageView(this);
    	overviewView.setScaleType(ImageView.ScaleType.FIT_XY);
    	overviewView.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.FILL_PARENT));

    	final LinearLayout overviewHolder = new LinearLayout(this);
    	overviewHolder.setLayoutParams(new ViewGroup.LayoutParams(image_width, image_height));
    	overviewHolder.addView(overviewView, image_width, image_height);
    	
    	Log.i("Freeciv", String.format("From: %d, %d To: %d, %d", theOverview.getWidth(), theOverview.getHeight(), image_width, image_height));
    	
    	final BitmapDrawable filtered =new BitmapDrawable(theOverview);
    	filtered.setAntiAlias(false);
    	filtered.setFilterBitmap(false);
    	
    	
    	final float scaleFactor = imageScale;
    	

    	
    	//overviewView.setTouchDelegate(new TouchDelegate(null, overviewView) {});
    	overviewView.setOnTouchListener(new OnTouchListener() {

			@Override
			public boolean onTouch(View arg0, MotionEvent arg1) {
				Log.i("Freeciv", String.format("Touch: %d, %d", (int)(arg1.getX()), (int)(arg1.getY())));
				nh.positionFromOverview((int)(arg1.getX() / scaleFactor), (int)(arg1.getY() / scaleFactor));
				BitmapDrawable filtered = new BitmapDrawable(NativeHarness.getOverview());
		    	filtered.setAntiAlias(false);
		    	filtered.setFilterBitmap(false);
				overviewView.setImageDrawable(filtered);
				return false;
			}
    		
    	});
    	overviewView.setImageDrawable(filtered);
    	
    	
    	//builder.setView();
    	//builder.
    	
    	overviewDialog = builder.create();

    	
    	overviewDialog.setView(overviewHolder);
    	overviewDialog.setCanceledOnTouchOutside(true);
    	overviewDialog.show();

    	
    	
        return false;  // don't go ahead and show the search box
    }
    
    private int getMapWidth(Display d) {
    	if (Settings.getCurrentSettings().isHalveResolution()) {
    		return d.getWidth() / 2;
    	}
    	
    	return d.getWidth();
    }

    private int getMapHeight(Display d) {
    	if (Settings.getCurrentSettings().isHalveResolution()) {
    		return d.getHeight() / 2;
    	}
    	
    	return d.getHeight();
    }
    
    private int scalePos(float p) {
    	if (Settings.getCurrentSettings().isHalveResolution()) {
    		return (int)p / 2;
    	}
    	
    	return (int)p;    	
    }
    
    private int scaleMotionX(MotionEvent e) {
    	return scalePos(e.getX());
    }
    
    private int scaleMotionY(MotionEvent e) {
    	return scalePos(e.getY());	    	
    }
    
    private class MapViewGestureProcessor extends GestureDetector.SimpleOnGestureListener {
    	@Override
		public boolean onScroll(MotionEvent e1, MotionEvent e2,
				float distanceX, float distanceY) {

    		if (isTrackingPushMove())
    			return true;

			NativeHarness.moveMapView(scalePos(distanceX), scalePos(distanceY));
			
			return true;
		}

		@Override
		public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
				float velocityY) {
    		if (isTrackingPushMove())
    			return true;
			
			// Need to handle flings
			return super.onFling(e1, e2, velocityX, velocityY);
		}

		@Override
		public void onLongPress(MotionEvent e) {
			Log.d("FreeCiv", String.format("LongPress - e: %s", e.toString()));
			lastLongPress = e;
			
			if (isTrackingPushMove())
				return;
			
			int[] unitsOnTile = NativeHarness.getUnitsOnTile(scaleMotionX(e), scaleMotionY(e));
			
			if (unitsOnTile.length > 0) {
				//NativeHarness.actionSelectPopup(scaleMotionX(e), scaleMotionY(e));
				NativeHarness.focusOnUnit(unitsOnTile[0]);
				NativeHarness.sendCommand(0); //Trigger goto
 				Vibrator vibrator = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
				vibrator.vibrate(50);
			} else {
				stopTrackingPushMove();
			}
			
			super.onLongPress(e);
		}

		@Override
		public boolean onSingleTapConfirmed(MotionEvent e) {
			Log.d("FreeCiv", String.format("Tapped - e: %s", e.toString()));
			
			if (isTrackingPushMove())
				if (Math.abs(e.getX() - lastMoveAfterLongPress.getX()) < 40 && Math.abs(e.getY() - lastMoveAfterLongPress.getY()) < 40) {
					NativeHarness.actionSelectPopup(scaleMotionX(e), scaleMotionY(e));
				} else {
					stopTrackingPushMove();
				}
			else {
				
				int[] unitsOnTile = NativeHarness.getUnitsOnTile(scaleMotionX(e), scaleMotionY(e));
				
				if (NativeHarness.getCityOnTile(scaleMotionX(e), scaleMotionY(e)) == -1) {
	    			if (unitsOnTile.length > 0) {
	        			if (unitsOnTile.length == 1) {
	        				showUnitMenu();
	        			} else {
	        				showUnitSelection(scaleMotionX(e), scaleMotionY(e));
	        			}
	    			}
				}
				
				NativeHarness.actionSelectPopup(scaleMotionX(e), scaleMotionY(e));
			}
			
			return super.onSingleTapConfirmed(e);
		}

    }
}