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
import net.hackcasual.freeciv.i18n.TranslateStrings;

import android.graphics.Bitmap;


public class Player {
	private final int number;
	private final String name;
	private final String userName;
	private final String year;
	private final int population;
	private final int gold;
	private final int score;
	private final boolean male;
	private final int turns;
	private final String nationName;
	private final int lastBulbs;
	private final Advance currentlyResearching;
	private final int currentBulbsForAdvance;
	private final int totalBulbsForAdvance;
	private final Government currentGovernment;
	private final Government targetGovernment;
	private final int anarchyTurns;
	private final String flagName;
	private final String peopleName;
	private final String rulerName;	
	
	public String getRulerName() {
		return rulerName;
	}

	public String getPeopleName() {
		return peopleName;
	}

	public Player(int number, String userName, String year, int population, int gold, int score, String name, boolean male, int turns, String rulerName, String nationName, String peopleName, String flagName, int flag_w, int flag_h, 
			int lastBulbs, int advanceId, int curBulbs, int totalBulbs, int governmentId, int targetGovernmentId, int anarchyTurns) {
		this.number = number;
		this.name = TranslateStrings.translate(name);
		this.male = male;
		this.turns = turns;
		this.nationName = TranslateStrings.translate(nationName);
		this.peopleName = TranslateStrings.translate(peopleName);
		this.lastBulbs = lastBulbs;
		this.userName = userName;
		this.year = year;
		this.population = population;
		this.gold = gold;
		this.score = score;
		this.flagName = flagName;
		this.rulerName = TranslateStrings.translate(rulerName);
				
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
		return Civ.getBigFlag(flagName);
	}
	
	public String getFlagName() {
		return flagName;
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

	public String getUserName() {
		return userName;
	}

	public String getYear() {
		return year;
	}

	public int getPopulation() {
		return population;
	}

	public int getGold() {
		return gold;
	}

	public int getScore() {
		return score;
	}
}
