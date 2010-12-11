package net.hackcasual.freeciv.views;

import java.util.ArrayList;
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

import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;

import net.hackcasual.freeciv.Civ;
import net.hackcasual.freeciv.NativeHarness;
import net.hackcasual.freeciv.models.Advance;
import net.hackcasual.freeciv.models.AdvanceExpense;
import net.hackcasual.freeciv.models.Improvement;
import net.hackcasual.freeciv.models.UnitType;
import net.hackcasual.freeciv.widgets.CheckableLayout;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class ResearchView extends Activity {
	Set<FrameLayout> checked;
	Map<Integer, FrameLayout> layoutMap;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.imprlist);
        
        LinearLayout mainNode = (LinearLayout)findViewById(R.id.impr_list);

        checked = new HashSet<FrameLayout>();
        layoutMap = new HashMap<Integer, FrameLayout>();
        
        
        for (AdvanceExpense e: ((Civ)getApplicationContext()).getCurrentResearchStatus()) {
        	FrameLayout impr = (FrameLayout) this.getLayoutInflater().inflate(R.layout.imprinfo, null);

            ImageView iv = (ImageView)impr.findViewById(R.id.impr_icon);
            iv.setImageBitmap(e.getTarget().getIcon());
            TextView name = (TextView)impr.findViewById(R.id.impr_desc);
            name.setText(String.format("%s Steps: %d Bulbs: %d", e.getTarget().getName(), e.getSteps(), e.getBulbs()));
            
            LinearLayout enables = (LinearLayout)impr.findViewById(R.id.advance_enables);
            
            for (Advance a: e.getTarget().getEnabledAdvances()) {
            	ImageView img = new ImageView(this);
            	img.setImageBitmap(a.getIcon());
            	enables.addView(img);
            }
            for (UnitType u: e.getTarget().getEnabledUnitTypes()) {
            	ImageView img = new ImageView(this);
            	img.setImageBitmap(u.getIcon());
            	enables.addView(img);
            }
            for (Improvement i: e.getTarget().getEnabledImprovements()) {
            	ImageView img = new ImageView(this);
            	img.setImageBitmap(i.getIcon());
            	enables.addView(img);
            }

            
            layoutMap.put(e.getTarget().getId(), impr);
            final int id = e.getTarget().getId();
            
            
            
            impr.setOnClickListener(new OnClickListener() {

				@Override
				public void onClick(View arg0) {
					
					for (FrameLayout fl: checked) {
						((LinearLayout)fl.findViewById(R.id.improvement_highlight)).setVisibility(View.INVISIBLE);
					}
					
					checked.clear();
					
					Stack<Integer> toCheck = new Stack<Integer>();
					List<Integer> checkThis = new ArrayList<Integer>();
					
					toCheck.push(id);
										
					while (!toCheck.isEmpty()) {
						int id = toCheck.pop();
						checkThis.add(id);
						if (Civ.getAdvanceById(id).getRequire1() > 0)
							toCheck.push(Civ.getAdvanceById(id).getRequire1());
						if (Civ.getAdvanceById(id).getRequire2() > 0)
							toCheck.push(Civ.getAdvanceById(id).getRequire2());
					}
					
					for (int id: checkThis) {
						if (layoutMap.containsKey(id)) {
							checked.add(layoutMap.get(id));
							((LinearLayout)layoutMap.get(id).findViewById(R.id.improvement_highlight)).setVisibility(View.VISIBLE);
							Log.i("Freeciv", "Checking ID# " + id);
						}
					}
					
					
					NativeHarness.setAdvanceGoal(id);
				}
            	
            });
            
            mainNode.addView(impr);
        }
        

	}
}
