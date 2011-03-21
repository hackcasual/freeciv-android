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
import net.hackcasual.freeciv.models.Government;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;



public class GovernmentAdapter extends ArrayAdapter<Government> {
	LayoutInflater viewInflater;
	
	public GovernmentAdapter(Context context) {
		super(context, 0);
		viewInflater = ((LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE));
		
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		Government g = this.getItem(position);
		LinearLayout curView;
		if (convertView != null) {
			curView = (LinearLayout) convertView;
		} else {
			curView = (LinearLayout) viewInflater.inflate(R.layout.gov_info, null);
		}
		((ImageView)curView.findViewById(R.id.gov_icon)).setImageDrawable(g.getIcon());		
		((TextView)curView.findViewById(R.id.gov_name)).setText(g.getName());
		return curView;
	}
}
