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

import android.graphics.Bitmap;

public class Improvement extends Buildable {
	private final Genus genus;
	private final int requiredAdvance;
	public enum Genus {
		  GREAT_WONDER,
		  SMALL_WONDER,
		  IMPROVEMENT,
		  SPECIAL
	}
	
	public Improvement(String name, int id, String helpText, int genus_id, int requiredAdvance, int icon_w, int icon_h) {
		super(name, id, helpText, icon_w, icon_h);
		
		switch (genus_id) {
		case 0: genus = Genus.GREAT_WONDER; break;
		case 1: genus = Genus.SMALL_WONDER; break;
		case 2: genus = Genus.IMPROVEMENT; break;
		case 3: genus = Genus.SPECIAL; break;
		default: genus = Genus.IMPROVEMENT; break;
		}
		
		this.requiredAdvance = requiredAdvance;
	}

	public Genus getGenus() {
		return genus;
	}

	public int getRequiredAdvance() {
		return requiredAdvance;
	}
}
