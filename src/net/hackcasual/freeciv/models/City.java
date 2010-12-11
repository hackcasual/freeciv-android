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
import android.os.Parcel;
import android.os.Parcelable;

public final class City {
	private final String name;
	
	private final int cityId;
	private final int population;
	private final int foodPerTurn;
	private final int surplus;
	private final int production;
	private final int support;
	private final int trade;
	private final int corruption;
	private final int gold;
	private final int upkeep;
	private final int science;
	private final int luxury;
	private final Bitmap cityImage;
	private final Buildable building;
	private final int productionTurns;
	private final int productionStock;
	private final int productionCost;
	
	
	public City(String name,
			int cityId,
			int map_w,
			int map_h,
			int population, 
			int food_per_turn, 
			int surplus, 
			int production,
			int support,
			int trade,
			int corruption,
			int gold,
			int upkeep,
			int science,
			int luxury,
			int buildType,
			int buildId,
			int productionTurns,
			int productionStock,
			int productionCost) {
		
		this.name = name;
		this.cityId = cityId;
		this.population = population;
		this.foodPerTurn = food_per_turn;
		this.surplus = surplus;
		this.production = production;
		this.support = support;
		this.trade = trade;
		this.corruption = corruption;
		this.gold = gold;
		this.upkeep = upkeep;
		this.science = science;
		this.luxury = luxury;
		this.productionTurns = productionTurns;
		this.productionStock = productionStock;
		this.productionCost = productionCost;
		
		this.cityImage = Bitmap.createBitmap(map_w, map_h, Bitmap.Config.RGB_565);
		this.cityImage.copyPixelsFromBuffer(NativeHarness.getHarness().getIncomingBuffer());
		
		switch (buildType) {
		case 0: building = Civ.getImprovementTypeById(buildId); break;
		case 1: building = Civ.getUnitTypeById(buildId); break;
		default: building = null;
		}
	}
	
	public String getName() {
		return name;
	}

	public int getCityId() {
		return cityId;
	}
	
	public int getPopulation() {
		return population;
	}

	public int getFoodPerTurn() {
		return foodPerTurn;
	}

	public int getSurplus() {
		return surplus;
	}

	public int getProduction() {
		return production;
	}

	public int getSupport() {
		return support;
	}

	public int getTrade() {
		return trade;
	}

	public int getCorruption() {
		return corruption;
	}

	public int getGold() {
		return gold;
	}

	public int getUpkeep() {
		return upkeep;
	}

	public int getScience() {
		return science;
	}

	public int getLuxury() {
		return luxury;
	}
	
	public Bitmap getMap() {
		return cityImage;
	}
	
	public Universal getCurrentProduction() {
		return building;
	}

	public Bitmap getCityImage() {
		return cityImage;
	}

	public Buildable getBuilding() {
		return building;
	}

	public int getProductionTurns() {
		return productionTurns;
	}

	public int getProductionStock() {
		return productionStock;
	}

	public int getProductionCost() {
		return productionCost;
	}	
}
