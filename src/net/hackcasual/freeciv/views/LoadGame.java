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

import java.io.File;
import java.io.FileFilter;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class LoadGame extends Activity {

	public static String SAVED_GAME_TAG;
	
	private ArrayAdapter<String> savedGames;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
    	setContentView(R.layout.loadgame);
    	
    	savedGames = new ArrayAdapter<String>(this, R.layout.savedgame);
    	
        ListView pairedListView = (ListView) findViewById(R.id.saved_games);
        pairedListView.setAdapter(savedGames);
        pairedListView.setOnItemClickListener(gameClickListener);
        
        final Activity container = this;
        
        (new Thread() {
        	@Override
        	public void run() {
        		File dir = new File("/sdcard/Freeciv");
        		
        		List<File> files = new ArrayList<File>(Arrays.asList(dir.listFiles(new FileFilter() {

					@Override
					public boolean accept(File pathname) {
						return pathname.isFile() && (pathname.getAbsolutePath().endsWith("sav") 
								|| pathname.getAbsolutePath().endsWith("sav.gz"));
					}})));
        		
        		Collections.sort(files, new Comparator<File>() {

					@Override
					public int compare(File object1, File object2) {
						return (int) (object2.lastModified() - object1.lastModified());
					}
        			
        		});
        		
        		for (final File save: files) 
        		{
        			container.runOnUiThread(new Runnable() {

						@Override
						public void run() {
							savedGames.add(save.getName());
							
						}});
        			
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
}
