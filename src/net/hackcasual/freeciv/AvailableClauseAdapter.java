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


import java.util.LinkedList;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.models.DiplomacyClause;
import net.hackcasual.freeciv.models.Unit;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;



public class AvailableClauseAdapter extends ArrayAdapter<DiplomacyClause> {
	LayoutInflater viewInflater;

	public interface clickCallback {
		void onItemClicked(int index);
	}
	
	final clickCallback callback;
	
	public AvailableClauseAdapter(Context context, clickCallback callback) {
		super(context, 0, new LinkedList<DiplomacyClause>());
		viewInflater = ((LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE));
		this.callback = callback;
	}

	@Override
	public View getView(final int position, View convertView, ViewGroup parent) {
		DiplomacyClause clause = this.getItem(position);
		LinearLayout curView;
		if (convertView != null) {
			curView = (LinearLayout) convertView;
		} else {
			curView = (LinearLayout) viewInflater.inflate(R.layout.available_pact, null);
		}

		((TextView)curView.findViewById(R.id.pact_name)).setText(clause.getName());
		((ImageButton)curView.findViewById(R.id.pact_button)).setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				callback.onItemClicked(position);
			}
		});

		return curView;
	}
}
