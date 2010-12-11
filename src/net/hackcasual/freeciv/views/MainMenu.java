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

import java.util.HashMap;
import java.util.Map;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

class TilesetProgress {
	public final String name;
	public final int progressVal;
	public TilesetProgress(String n, int v) {
		name = n;
		progressVal = v;
	}
}


public class MainMenu extends NativeAwareActivity {
	private static final int LOAD_GAME = 1;

	
	private static final Map<String, TilesetProgress> progressMap = new HashMap<String, TilesetProgress>();
	
	static {
		progressMap.put("loading_indicators", new TilesetProgress("Indicators", 4));
		progressMap.put("loading_mask", new TilesetProgress("Masks", 8));
		progressMap.put("loading_spaceship", new TilesetProgress("Spaceship", 12));		
		progressMap.put("loading_cursors", new TilesetProgress("Cursors", 17));
		progressMap.put("loading_roads", new TilesetProgress("Roads", 21));
		progressMap.put("loading_unitinfo", new TilesetProgress("Units", 25));
		progressMap.put("loading_cities", new TilesetProgress("Cities", 30));
		progressMap.put("loading_overlays", new TilesetProgress("Overlays", 34));
		progressMap.put("loading_farmland", new TilesetProgress("Farmland", 38));		
		progressMap.put("loading_fog", new TilesetProgress("Fog", 43));
		progressMap.put("Lake", new TilesetProgress("Lakes", 47));
		progressMap.put("Ocean", new TilesetProgress("Oceans", 51));
		progressMap.put("Deep Ocean", new TilesetProgress("Deep Oceans", 56));
		progressMap.put("Glacier", new TilesetProgress("Glaciers", 60));
		progressMap.put("Desert", new TilesetProgress("Deserts", 64));		
		progressMap.put("Forest", new TilesetProgress("Forests", 69));
		progressMap.put("Grassland", new TilesetProgress("Grasslands", 73));
		progressMap.put("Hills", new TilesetProgress("Hills", 77));
		progressMap.put("Jungle", new TilesetProgress("Jungles", 82));
		progressMap.put("Mountains", new TilesetProgress("Mountains", 86));
		progressMap.put("Plains", new TilesetProgress("Plains", 90));		
		progressMap.put("Swamp", new TilesetProgress("Swamps", 95));
		progressMap.put("Tundra", new TilesetProgress("Tundra", 100));						
	}
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
    	setContentView(R.layout.mainmenu);
    	
    	/*Thread t = new Thread() {
    		@Override
    		public void run() {
    			try {
					Thread.sleep(3000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
    			
				
				that.runOnUiThread(new Runnable() {

					@Override
					public void run() {
						Animation aa = new AlphaAnimation(1.0f, 0.0f);
						aa.setDuration(1000);
						aa.setFillAfter(true);
						View v = that.findViewById(R.id.tileset_progress_bar);
						v.startAnimation(aa);
						//v.
						//aa.start();
					}
					
				});
    		}
    	};
    	
    	t.start();*/
    	
    }

	@Override
	public void recieveTilesetUpdate(String info) {


		if (progressMap.containsKey(info)) {
			final TilesetProgress tp = progressMap.get(info);
			final ProgressBar progressBar = (ProgressBar)this.findViewById(R.id.current_progress_bar);
			final TextView progressItem = (TextView)this.findViewById(R.id.current_progress_item);
			this.runOnUiThread(new Runnable() {

				@Override
				public void run() {
					if (progressBar.getProgress() <= tp.progressVal) {
						progressBar.setProgress(tp.progressVal);						
						progressItem.setText(tp.name);
					}
				}
				
			});
		}
		
	}
	
	public void newGameListener(View v) {
		Intent freeCiv = new Intent(this, Freeciv.class);
		startActivity(freeCiv);
		finish();
	}
	
	public void loadGameListener(View v) {
		Intent loadGame = new Intent(this, LoadGame.class);
		//startActivity(freeCiv);
		this.startActivityForResult(loadGame, LOAD_GAME);
	}
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (LOAD_GAME == requestCode && Activity.RESULT_OK == resultCode) {
			Intent freeCiv = new Intent(this, Freeciv.class);
			freeCiv.putExtra(LoadGame.SAVED_GAME_TAG, data.getStringExtra(LoadGame.SAVED_GAME_TAG));
			startActivity(freeCiv);
			finish();			
		}
	}
}
