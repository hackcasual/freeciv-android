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

public class UnitType extends Buildable {
	final int hitPoints;
	final int attackStrength;
	final int defenseStrength;
	final int moveRate;
	final int transportCapacity;
	final int requiredAdvance;
	
	public UnitType(String name, int id, String helpText, int icon_w, int icon_h,
			int hitPoints, int attackStrength, int defenseStrength, int moveRate,
			int transportCapacity, int requiredAdvance) {
		super(name, id, helpText, icon_w, icon_h);
		
		this.hitPoints = hitPoints;
		this.attackStrength = attackStrength;
		this.defenseStrength = defenseStrength;
		this.moveRate = moveRate;
		this.transportCapacity = transportCapacity;
		this.requiredAdvance = requiredAdvance;
	}


	public int getHitPoints() {
		return hitPoints;
	}


	public int getAttackStrength() {
		return attackStrength;
	}


	public int getDefenseStrength() {
		return defenseStrength;
	}


	public int getMoveRate() {
		return moveRate;
	}


	public int getTransportCapacity() {
		return transportCapacity;
	}


	public int getRequiredAdvance() {
		return requiredAdvance;
	}
}
