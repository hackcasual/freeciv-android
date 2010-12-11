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

package net.hackcasual.freeciv.models;

import android.graphics.Bitmap;
import android.graphics.Matrix;

public class Government extends Universal {
	final int requiredAdvance;
	final Bitmap scaledIcon;

	public Government(String name, int id, String helpText, int icon_w,
			int icon_h, int requiredAdvance) {
		super(name, id, helpText, icon_w, icon_h);
		this.requiredAdvance = requiredAdvance;

		
		scaledIcon = Bitmap.createScaledBitmap(icon, icon.getWidth() * 2, icon.getHeight() * 2, false);
		icon.recycle();	
	}

	public int getRequiredAdvance() {
		return requiredAdvance;
	}
	
	@Override
	public Bitmap getIcon() {
		return scaledIcon;
	}
}
