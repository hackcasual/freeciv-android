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

import net.hackcasual.freeciv.CityPresentUnitAdapter;
import net.hackcasual.freeciv.Civ;
import net.hackcasual.freeciv.GovernmentAdapter;
import net.hackcasual.freeciv.NativeHarness;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;
import net.hackcasual.freeciv.models.Player;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

public class PlayerView extends Activity {
	NativeHarness nh;
	@Override
	public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.playerinfo);
        
	}
	
	@Override
	public void onResume() {
		super.onResume();
        nh = ((Civ)this.getApplication()).getNativeHarness();
        
        Player player = NativeHarness.getPlayerInfo();
        int lastBulbs = player.getLastBulbs();
        if (lastBulbs == 0) lastBulbs = 1;
        ((TextView)this.findViewById(R.id.player_name)).setText(player.getName());
        ((TextView)this.findViewById(R.id.nation_name)).setText(player.getNationName());       
        ((ImageView)this.findViewById(R.id.player_flag)).setImageBitmap(player.getFlag());
        ((TextView)this.findViewById(R.id.turn_count)).setText(String.valueOf(player.getTurns()));
        ((TextView)this.findViewById(R.id.last_bulbs)).setText(String.valueOf(player.getLastBulbs()));
        ((TextView)this.findViewById(R.id.government_type_name)).setText(player.getCurrentGovernment().getName() + 
        		(player.getAnarchyTurns() > 0 ? " for " + player.getAnarchyTurns() + " turns":""));

        
        ((TextView)this.findViewById(R.id.username)).setText(player.getUserName());
        ((TextView)this.findViewById(R.id.year)).setText(player.getYear());
        ((TextView)this.findViewById(R.id.score)).setText(String.valueOf(player.getScore()));
        ((TextView)this.findViewById(R.id.gold)).setText(String.valueOf(player.getGold()));
        ((TextView)this.findViewById(R.id.population)).setText(String.valueOf(player.getPopulation() * 1000));        
        
        ((ImageView)this.findViewById(R.id.government_type_icon)).setImageDrawable(player.getCurrentGovernment().getIcon());        
        if (player.getCurrentlyResearching() != null) {
	        ((ImageView)this.findViewById(R.id.impr_icon)).setImageDrawable(player.getCurrentlyResearching().getIcon());
	        ((TextView)this.findViewById(R.id.impr_desc)).setText(player.getCurrentlyResearching().getName());
	        ((ProgressBar)this.findViewById(R.id.tech_progress)).setProgress((int)(100.0 * (player.getCurrentBulbsForAdvance() * 1.0 / player.getTotalBulbsForAdvance())));
	        ((TextView)this.findViewById(R.id.research_progress)).setText(String.format(
	        		"%d bulbs out of %d, %d bulbs a turn, completion in %d turns", 
	        		player.getCurrentBulbsForAdvance(),
	        		player.getTotalBulbsForAdvance(),
	        		player.getLastBulbs(),
	        		(player.getTotalBulbsForAdvance() - player.getCurrentBulbsForAdvance()) / lastBulbs
	        		));
        }		
	}
	
    public void clickGovernment(View target) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		
		builder.setTitle("Revolution");
		ListView governments = new ListView(this);
		governments.setFocusable(true);
		final GovernmentAdapter adapter = new GovernmentAdapter(this);
		governments.setAdapter(adapter);

		int[] availableGovernments = NativeHarness.getAvailableGovernments();
		for (int i: availableGovernments) {
			adapter.add(Civ.getGovernmentById(i));
		}
		final AlertDialog shown = builder.create();
		governments.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int pos,
					long arg3) {
				
				nh.setGovernmentType(adapter.getItem(pos).getId());
				shown.dismiss();
				
			}
			
		});
		shown.setView(governments);
		shown.show();

    }
    
    public void showDiplomacy(View target) {
    	Intent diploIntent = new Intent(this, Diplomacy.class);
    	this.startActivity(diploIntent);
    }
}
