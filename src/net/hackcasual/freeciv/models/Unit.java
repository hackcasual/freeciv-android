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


public class Unit {
	final int unitId;
	final UnitType type;
	final int ownerId;
	final int movesLeft;
	final int veteranLevel;
	final int hitPoints;
	final Bitmap sprite;
	
	public Unit (int unitId, int typeId, int ownerId, int movesLeft, int veteranLevel, int hitPoints, int sprite_w, int sprite_h) {
		this.unitId = unitId;
		this.type = Civ.getUnitTypeById(typeId);
		this.ownerId = ownerId;
		this.movesLeft = movesLeft;
		this.veteranLevel = veteranLevel;
		this.hitPoints = hitPoints;
		
		this.sprite = Bitmap.createBitmap(sprite_w, sprite_h, Bitmap.Config.ARGB_4444);
		this.sprite.copyPixelsFromBuffer(NativeHarness.getHarness().getIncomingBuffer());
		
	}

	public int getUnitId() {
		return unitId;
	}

	public UnitType getType() {
		return type;
	}

	public int getOwnerId() {
		return ownerId;
	}

	public int getMovesLeft() {
		return movesLeft;
	}

	public int getVeteranLevel() {
		return veteranLevel;
	}

	public int getHitPoints() {
		return hitPoints;
	}
	
	@Override
	public String toString() {
		return String.format("%s (%d,%d,%d/%d) %d/%d", type.getName(), type.attackStrength, type.defenseStrength, movesLeft, type.moveRate, hitPoints, type.hitPoints);
	}

	public Bitmap getSprite() {
		return sprite;
	}
}
