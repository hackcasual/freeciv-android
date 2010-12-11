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


import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.models.Unit;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;



public class CityPresentUnitAdapter extends ArrayAdapter<Unit> {
	LayoutInflater viewInflater;
	
	public CityPresentUnitAdapter(Context context) {
		super(context, 0);
		viewInflater = ((LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE));
		
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		Unit u = this.getItem(position);
		LinearLayout curView;
		if (convertView != null) {
			curView = (LinearLayout) convertView;
		} else {
			curView = (LinearLayout) viewInflater.inflate(R.layout.city_presentunit, null);
		}

		((ImageView)curView.findViewById(R.id.Unit_Icon)).setImageBitmap(u.getSprite());	
		((TextView)curView.findViewById(R.id.Unit_Info)).setText(u.getType().getName() + "\n" + String.format("(%d,%d,%d/%d) %d/%d", u.getType().getAttackStrength(), u.getType().getDefenseStrength(), u.getMovesLeft(), u.getType().getMoveRate(), u.getHitPoints(), u.getType().getHitPoints()));

		return curView;
	}
}
