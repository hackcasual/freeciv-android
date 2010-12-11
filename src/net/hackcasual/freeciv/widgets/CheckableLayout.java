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

package net.hackcasual.freeciv.widgets;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.Checkable;
import android.widget.LinearLayout;

public class CheckableLayout extends LinearLayout implements Checkable {
    
	boolean checked;
	
	public CheckableLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        checked = true;
	}
    
	@Override
	public boolean isChecked() {
		return checked;
	}

	@Override
	public void setChecked(boolean checked) {
		this.checked = checked;
		refreshDrawableState();
	}

	@Override
	public void toggle() {
		checked = !checked;
	}
	
	

}
