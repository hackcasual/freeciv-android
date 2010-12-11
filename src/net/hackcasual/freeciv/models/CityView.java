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

import android.os.Parcel;
import android.os.Parcelable;

public class CityView implements Parcelable {
	private final String name;
	
	private final int cityPtr;
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
	private final int imageRef;
	
	public final static Parcelable.Creator<CityView> CREATOR = new Parcelable.Creator<CityView>() {

		@Override
		public CityView createFromParcel(Parcel source) {
			return new CityView(source);
		}

		@Override
		public CityView[] newArray(int size) {
			return new CityView[size];
		}

	};
	
	public CityView(String name,
			int cityPtr,
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
			int imageRef) {
		
		this.name = name;
		this.cityPtr = cityPtr;
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
		this.imageRef = imageRef;
	}
	
	public CityView(Parcel in) {
		this.name = in.readString();
		this.cityPtr = in.readInt();
		this.population = in.readInt();
		this.foodPerTurn = in.readInt();
		this.surplus = in.readInt();
		this.production = in.readInt();
		this.support = in.readInt();
		this.trade = in.readInt();
		this.corruption = in.readInt();
		this.gold = in.readInt();
		this.upkeep = in.readInt();
		this.science = in.readInt();
		this.luxury = in.readInt();	
		this.imageRef = in.readInt();
	}

	public String getName() {
		return name;
	}

	public int getCityPtr() {
		return cityPtr;
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

	public int getImageRef() {
		return imageRef;
	}
	
	@Override
	public int describeContents() {
		return 0;
	}

	@Override
	public void writeToParcel(Parcel out, int arg1) {
		out.writeString(name);
		out.writeInt(cityPtr);
		out.writeInt(population);
		out.writeInt(foodPerTurn);
		out.writeInt(surplus);
		out.writeInt(production);
		out.writeInt(support);
		out.writeInt(trade);
		out.writeInt(corruption);
		out.writeInt(gold);
		out.writeInt(upkeep);
		out.writeInt(science);
		out.writeInt(luxury);
		out.writeInt(imageRef);
	}


}
