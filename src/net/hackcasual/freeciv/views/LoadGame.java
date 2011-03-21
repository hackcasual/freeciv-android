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

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import net.hackcasual.freeciv.Civ;
import net.hackcasual.freeciv.Civ.SaveType;
import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.game.Game;
import net.hackcasual.freeciv.game.SaveGameDB;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;

public class LoadGame extends Activity {

	public static String SAVED_GAME_TAG;
	
	private SaveGameDB gameDB = new SaveGameDB(this);
	private Set<View> autoGames;
	private Set<View> manualGames;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
    	setContentView(R.layout.loadgame);
    	
        final LinearLayout saves = (LinearLayout) findViewById(R.id.saved_games);
        
        Spinner saveFilter = (Spinner) findViewById(R.id.save_type_filter);
        
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
                this, R.array.save_game_types, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        saveFilter.setAdapter(adapter);
        
        saveFilter.setOnItemSelectedListener(new OnItemSelectedListener() {

			@Override
			public void onItemSelected(AdapterView<?> arg0, View arg1,
					int pos, long arg3) {
				
				int manualVisibility = View.VISIBLE;
				int autoVisibility = View.VISIBLE;
				
				if (pos == 1) // Manual
				{
					autoVisibility = View.GONE;
				} else if (pos == 2) {
					manualVisibility = View.GONE;
				}
				
				for (View v: autoGames) {
					v.setVisibility(autoVisibility);
				}
				
				for (View v: manualGames) {
					v.setVisibility(manualVisibility);
				}
				
			}

			@Override
			public void onNothingSelected(AdapterView<?> arg0) {
				//Do nothing...				
			}
        	
        });
        
        autoGames = new HashSet<View>();
        manualGames = new HashSet<View>();
        
        (new Thread() {
        	@Override
        	public void run() {
        		final List<Game> allSaves = gameDB.listGames();
        		gameDB.close();
    			for (final Game g: allSaves) {
        			if (g.saveExists() && Civ.getSaveType(g) != SaveType.FORCED) { // Since it may take some time to check if the file exists, don't stall showing the entire loading screen
        				saves.post(new Runnable(){

						@Override
						public void run() {
							View gameView = gameAsShortInfo(g);
							if (Civ.getSaveType(g) == SaveType.AUTO) {
								autoGames.add(gameView);
							} else if (Civ.getSaveType(g) == SaveType.MANUAL) {
								manualGames.add(gameView);
							}							
	        				saves.addView(gameView);		        		
						}
        			
	        		});

        			}
        		}
        		
        	}
        }).start();
    }
    
    private OnItemClickListener gameClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int arg2, long arg3) {
            // Get the device MAC address, which is the last 17 chars in the View
            String name = ((TextView) v).getText().toString();

            // Create the result Intent and include the MAC address
            Intent intent = new Intent();
            intent.putExtra(SAVED_GAME_TAG, name);
            
            // Set result and finish this Activity
            setResult(Activity.RESULT_OK, intent);
            finish();
        }
    };
    
    private LinearLayout gameAsShortInfo(final Game g) {
    	LinearLayout shortInfoLayout = (LinearLayout)this.getLayoutInflater().inflate(R.layout.saved_game, null);
    	
    	TextView rulerInfo = (TextView) shortInfoLayout.findViewById(R.id.ruler_of);
    	TextView lastPlayed = (TextView) shortInfoLayout.findViewById(R.id.last_played_value);
    	
    	TextView year = (TextView) shortInfoLayout.findViewById(R.id.year_value);
    	TextView score = (TextView) shortInfoLayout.findViewById(R.id.score_value);
 		
    	rulerInfo.setText(String.format(rulerInfo.getText().toString(), g.getCivLeader(), g.getCivName()));
    	lastPlayed.setText(nicePastTimeFormat(g.getLastPlayed()));
    	
    	year.setText(String.format("%s (%d)", g.getFormattedYear(), g.getTurn()));
    	score.setText(Integer.toString(g.getScore()));

    	ImageButton goButton = (ImageButton) shortInfoLayout.findViewById(R.id.go_button);
    	
    	goButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
	            Intent intent = new Intent();
	            intent.putExtra(SAVED_GAME_TAG, g.getSavedGame().getAbsolutePath());
	            
	            // Set result and finish this Activity
	            setResult(Activity.RESULT_OK, intent);
	            finish();
			}
    	});
    	
    	shortInfoLayout.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(final View v) {
				LinearLayout extraInfoAnchor = (LinearLayout)v.findViewById(R.id.full_information);
				
				LinearLayout fullInfoLayout = (LinearLayout)LoadGame.this.getLayoutInflater().inflate(R.layout.saved_game_full, null);
				
				final ImageView screenShot = (ImageView)fullInfoLayout.findViewById(R.id.screen_shot);
				ImageView flag = (ImageView)fullInfoLayout.findViewById(R.id.nation_flag);
				final ImageView overlay = (ImageView)fullInfoLayout.findViewById(R.id.overlay_map);
				
				final ProgressBar screenShotProgress = (ProgressBar)fullInfoLayout.findViewById(R.id.screenshot_progress);
				final ProgressBar overlayProgress = (ProgressBar)fullInfoLayout.findViewById(R.id.map_and_flag_progress);
				
				screenShot.setVisibility(View.GONE);
				flag.setImageBitmap(Civ.getBigFlag(g.getFlagResource()));
				overlay.setVisibility(View.GONE);
				
				// Asynchronously load the potentially slow screenshot and overlay
				(new Thread() {
					@Override
					public void run() {
						final Bitmap screenShotBitmap = BitmapFactory.decodeFile(g.getSnapShot().getAbsolutePath());
						final Bitmap overlayBitmap = BitmapFactory.decodeFile(g.getOverlayView().getAbsolutePath());
						
						v.post(new Runnable() {

							@Override
							public void run() {
								screenShotProgress.setVisibility(View.GONE);
								overlayProgress.setVisibility(View.GONE);
								
								screenShot.setImageBitmap(screenShotBitmap);
								overlay.setImageBitmap(overlayBitmap);
								screenShot.setVisibility(View.VISIBLE);
								overlay.setVisibility(View.VISIBLE);
							}
							
						});
						

					}
				}).start();
				
				extraInfoAnchor.addView(fullInfoLayout);
				
				final AlertDialog.Builder builder=new AlertDialog.Builder(LoadGame.this);
				builder.setTitle("Delete Game");
				builder.setMessage("Are you sure you want to delete this game?");
				builder.setIcon(android.R.drawable.ic_dialog_alert);

				builder.setPositiveButton(R.string.Ok, new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						((LinearLayout)v.getParent()).removeView(v);
						Civ.deleteGame(g);
					}
				});

				builder.setNegativeButton(R.string.Cancel, new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						// Don't do anything
					}
				});
				
				((Button)fullInfoLayout.findViewById(R.id.delete_button)).setOnClickListener(new OnClickListener() {

					@Override
					public void onClick(View v) {
						builder.show();						
					}
					
				});
			}
    		
    	});
    	
    	return shortInfoLayout;
    }
    
    static SimpleDateFormat lastPlayedFormat = new SimpleDateFormat("MMMMM dd, yyyy");
    
    private static String nicePastTimeFormat(Date when) {
    	long timeDelta = System.currentTimeMillis() - when.getTime();
    	
    	timeDelta /= 1000;
    	
    	if (timeDelta < 0)
    		return "The Future";
    	
    	if (timeDelta == 1)
    		return "1 second ago";
    	
    	if (timeDelta < 60)
    		return String.format("%d seconds ago", timeDelta);
    	
    	timeDelta /= 60;

    	if (timeDelta == 1)
    		return "1 minute ago";
    	
    	if (timeDelta < 60)
    		return String.format("%d minutes ago", timeDelta);
    	
    	timeDelta /= 60;
    	
    	if (timeDelta == 1)
    		return "1 hour ago";
    	
    	if (timeDelta < 24)
    		return String.format("%d hours ago", timeDelta);

    	timeDelta /= 24;

    	if (timeDelta == 1)
    		return "1 day ago";
    	
    	if (timeDelta < 7)
    		return String.format("%d days ago", timeDelta);
    	
    	return lastPlayedFormat.format(when);
    }
}
