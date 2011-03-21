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

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;

import net.hackcasual.freeciv.models.CityView;
import net.hackcasual.freeciv.views.CityDialog;

import net.hackcasual.freeciv.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.text.Editable;
import android.util.Log;
import android.widget.EditText;
import android.widget.TextView;

public class DialogManager {
	
	private Activity container;
	private final NativeHarness nh;
		
	public DialogManager(Activity a, NativeHarness nh) {
		container = a;
		this.nh = nh;
	}
	
	public DialogManager(NativeHarness nh) {
		container = null;
		this.nh = nh;
	}
	
	public void bindActivity(Activity container) {
		this.container = container;
	}
	
	public void infoUpdate() {
		(new Thread() {
			@Override
			public void run() {
				Civ.checkForNewTurn();
			}
		}).start();
	}
	
	public void setTurnDoneButton(final boolean state) {
		//Log.d("Freeciv","Setting turn done button " + state);
	}
	
	public void showCityNameDialog(final String suggested, final int unit_id) {
		if (container == null)
			return;
		
		container.runOnUiThread(new Runnable(){

			@Override
			public void run() {
				Log.i("Freeciv",String.format("New city suggested name: %s [%d]", suggested, unit_id));
				
				AlertDialog.Builder alert = new AlertDialog.Builder(container);

				alert.setTitle(R.string.new_city);
				alert.setMessage(R.string.city_name);

				// Set an EditText view to get user input 
				final EditText input = new EditText(container);
				input.setText(suggested);
				alert.setView(input);

				alert.setPositiveButton(R.string.Ok, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int whichButton) {
				  Editable value = input.getText();
				  nh.cityNameResponse(value.toString(), unit_id);
				  }
				});

				alert.setNegativeButton(R.string.Cancel, new DialogInterface.OnClickListener() {
				  public void onClick(DialogInterface dialog, int whichButton) {
				    // Canceled.
				  }
				});

				alert.show();
			}
			
		});


	}

	public void showCityDialog(int id) {
		Log.i("Freeciv", "Showing city dialog");
		if (container == null)
			return;
		
		final Intent dialogLauncher = new Intent(container.getApplicationContext(), CityDialog.class);
		
		dialogLauncher.putExtra("city-id", id);
		
		container.startActivity(dialogLauncher);
	}
	
	public void showMessages(final String[] msgs) {
		if (container == null)
			return;
		
		final StringBuilder messageContents = new StringBuilder();
		for (String m: msgs) {
			messageContents.append(m);
			messageContents.append("\n");
		}
		
		Log.i("Freeciv", "Messages: " + messageContents.toString());
		
		container.runOnUiThread(new Runnable(){

			@Override
			public void run() {
				Log.i("Freeciv", "Messages set");
				
				((TextView)container.findViewById(R.id.msg_bar)).setText(messageContents.toString());
			}
			
		});
	}
}
