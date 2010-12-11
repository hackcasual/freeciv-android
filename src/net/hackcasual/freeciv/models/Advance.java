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

import java.util.ArrayList;
import java.util.List;

public class Advance extends Universal {

	final int require1, require2;
	List<Advance> enabledAdvances;
	List<UnitType> enabledUnitTypes;
	List<Improvement> enabledImprovements;
	List<Government> enabledGovernments;
	
	
	public Advance(String name, int id, String helpText, int icon_w, int icon_h, int require1, int require2, int requireRoot) {
		super(name, id, helpText, icon_w, icon_h);
		this.require1 = require1;
		this.require2 = require2;
		
		this.enabledAdvances = new ArrayList<Advance>();
		this.enabledUnitTypes = new ArrayList<UnitType>();
		this.enabledImprovements = new ArrayList<Improvement>();
		this.enabledGovernments = new ArrayList<Government>();
	}

	public int getRequire1() {
		return require1;
	}

	public int getRequire2() {
		return require2;
	}

	public List<Advance> getEnabledAdvances() {
		return enabledAdvances;
	}

	public List<UnitType> getEnabledUnitTypes() {
		return enabledUnitTypes;
	}

	public List<Improvement> getEnabledImprovements() {
		return enabledImprovements;
	}

	public List<Government> getEnabledGovernments() {
		return enabledGovernments;
	}
	
}
