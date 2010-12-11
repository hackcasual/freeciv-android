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

import net.hackcasual.freeciv.Civ;
import net.hackcasual.freeciv.NativeHarness;

import android.graphics.Bitmap;

public class Player {
	private final int number;
	private final String name;
	private final boolean male;
	private final int turns;
	private final String nationName;
	private final Bitmap flag;
	private final int lastBulbs;
	private final Advance currentlyResearching;
	private final int currentBulbsForAdvance;
	private final int totalBulbsForAdvance;
	private final Government currentGovernment;
	private final Government targetGovernment;
	private final int anarchyTurns;
	
	public Player(int number, String name, boolean male, int turns, String nationName, int flag_w, int flag_h, 
			int lastBulbs, int advanceId, int curBulbs, int totalBulbs, int governmentId, int targetGovernmentId, int anarchyTurns) {
		this.number = number;
		this.name = name;
		this.male = male;
		this.turns = turns;
		this.nationName = nationName;
		this.lastBulbs = lastBulbs;
		
		this.flag = Bitmap.createBitmap(flag_w, flag_h, Bitmap.Config.ARGB_4444);
		this.flag.copyPixelsFromBuffer(NativeHarness.getHarness().getIncomingBuffer());
		this.currentlyResearching = Civ.getAdvanceById(advanceId);
		this.currentBulbsForAdvance = curBulbs;
		this.totalBulbsForAdvance = totalBulbs;
		
		this.currentGovernment = Civ.getGovernmentById(governmentId);
		this.targetGovernment = Civ.getGovernmentById(targetGovernmentId);;
		this.anarchyTurns = anarchyTurns;
	}

	public int getNumber() {
		return number;
	}

	public Government getCurrentGovernment() {
		return currentGovernment;
	}

	public Government getTargetGovernment() {
		return targetGovernment;
	}

	public int getAnarchyTurns() {
		return anarchyTurns;
	}

	public String getName() {
		return name;
	}

	public boolean isMale() {
		return male;
	}

	public int getTurns() {
		return turns;
	}

	public String getNationName() {
		return nationName;
	}

	public Bitmap getFlag() {
		return flag;
	}

	public int getLastBulbs() {
		return lastBulbs;
	}

	public Advance getCurrentlyResearching() {
		return currentlyResearching;
	}

	public int getCurrentBulbsForAdvance() {
		return currentBulbsForAdvance;
	}

	public int getTotalBulbsForAdvance() {
		return totalBulbsForAdvance;
	}
}
