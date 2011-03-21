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

import net.hackcasual.freeciv.NativeHarness;
import net.hackcasual.freeciv.i18n.TranslateStrings;

import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.util.DisplayMetrics;

public abstract class Universal {
	final String name;
	final BitmapDrawable icon;
	final int id;
	final String helpText;
	static final Bitmap none = Bitmap.createBitmap(20, 20, Bitmap.Config.ARGB_4444);
	
	public Universal(String name, int id, String helpText, int icon_w, int icon_h) {
		this.name = TranslateStrings.translate(name);
		this.id = id;
		this.helpText = TranslateStrings.translate(helpText);
				
		Bitmap iconBitmap;
		if (icon_w < 0 || icon_h < 0) {
			iconBitmap = none;
		} else {
			iconBitmap = Bitmap.createBitmap(icon_w, icon_h, Bitmap.Config.ARGB_4444);
			iconBitmap.copyPixelsFromBuffer(NativeHarness.getIncomingBuffer());
		}
		
		icon = new BitmapDrawable(iconBitmap);
		icon.setTargetDensity(DisplayMetrics.DENSITY_XHIGH);
	}

	public String getName() {
		return name;
	}

	public BitmapDrawable getIcon() {
		return icon;
	}

	public int getId() {
		return id;
	}

	public String getHelpText() {
		return helpText;
	}
	
	
}
