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

import net.hackcasual.freeciv.Civ;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;
import net.hackcasual.freeciv.models.Advance;
import net.hackcasual.freeciv.models.Improvement;
import net.hackcasual.freeciv.models.UnitType;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class ImprovementView extends Activity {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        setContentView(R.layout.imprlist);
        
        LinearLayout mainNode = (LinearLayout)findViewById(R.id.impr_list);//new LinearLayout(this);
        //mainNode.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));

        for (Advance i: ((Civ)getApplicationContext()).getAdvances()) {
        	LinearLayout impr = new LinearLayout(this);
            impr.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
            impr.setOrientation(LinearLayout.HORIZONTAL);

            ImageView iv = new ImageView(this);
            iv.setImageBitmap(i.getIcon());
            TextView name = new TextView(this);
            name.setText(i.getName() + " " + i.getHelpText());
            
            impr.addView(iv);
            impr.addView(name);
            Log.i("Freeciv", i.getHelpText());
            mainNode.addView(impr);
        }
        

	}
}
