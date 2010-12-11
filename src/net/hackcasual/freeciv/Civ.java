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

package net.hackcasual.freeciv;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.hackcasual.freeciv.models.Advance;
import net.hackcasual.freeciv.models.AdvanceExpense;
import net.hackcasual.freeciv.models.Government;
import net.hackcasual.freeciv.models.Improvement;
import net.hackcasual.freeciv.models.UnitType;
import net.hackcasual.freeciv.views.NativeAwareActivity;

import net.hackcasual.freeciv.R;

import android.app.Application;
import android.util.Log;

public class Civ extends Application implements NativeEventListener {
	NativeHarness nativeHarness;

	private String lastTilesetProgress = "";

	private NativeEventListener nativeListener;

	private boolean lastConnectionStatus = false;
	
	static List<Improvement> gameImprovements;
	static List<UnitType> unitTypes;
	static List<Advance> advances;
	static List<Government> governments;
	
	static Map<Integer, Improvement> improvementsById;
	static Map<Integer, UnitType> unitTypesById;
	static Map<Integer, Advance> advancesById;
	static Map<Integer, Government> governmentsById;		
	
	@Override
	public void onCreate() {
		Log.i("Freeciv", "Creating the app.");
        DataSetup.doIt(this.getResources().openRawResource(R.raw.freeciv));
		nativeHarness = NativeHarness.getHarness();
		
		// Start the client and server threads
		nativeHarness.runServer();
		nativeHarness.runClient();
		nativeHarness.hookNativeEventListener(this);
	}
	
	public void addImprovements(Improvement[] native_improvements) {
		gameImprovements = new ArrayList<Improvement>();
		improvementsById = new HashMap<Integer, Improvement>();
		for (Improvement i: native_improvements) {
			if (i != null) {
				gameImprovements.add(i);
				improvementsById.put(i.getId(), i);
			}
		}
	}
	
	public void addUnitTypes(UnitType[] native_unitTypes) {
		unitTypes = new ArrayList<UnitType>();
		unitTypesById = new HashMap<Integer, UnitType>();
		for (UnitType i: native_unitTypes) {
			if (i != null) {
				unitTypes.add(i);
				unitTypesById.put(i.getId(), i);
			}
		}
	}
	
	public void addAdvances(Advance[] native_advances) {
		advances = new ArrayList<Advance>();
		advancesById = new HashMap<Integer, Advance>();
		for (Advance i: native_advances) {
			if (i != null) {
				advances.add(i);
				advancesById.put(i.getId(), i);
			}
		}
	}
	
	public void addGovernments(Government[] native_governments) {
		governments = new ArrayList<Government>();
		governmentsById = new HashMap<Integer, Government>();
		for (Government g: native_governments) {
			if (g != null) {
				governments.add(g);
				governmentsById.put(g.getId(), g);
				Log.i("Freeciv", "Loading government: " + g.getName() + " " + g.getHelpText());
			}
		}
	}	
	
	void addAdvanceDependencies() {

		for (Advance adv: advances) {
			if (adv.getRequire1() > 0) {
				advancesById.get(adv.getRequire1()).getEnabledAdvances().add(adv);
			}
			if (adv.getRequire2() > 0) {
				advancesById.get(adv.getRequire2()).getEnabledAdvances().add(adv);
			}
		}
		for (UnitType ut: unitTypes) {
			if (ut.getRequiredAdvance() > 0) {
				advancesById.get(ut.getRequiredAdvance()).getEnabledUnitTypes().add(ut);
			}
		}
		for (Improvement imp: gameImprovements) {
			if (imp.getRequiredAdvance() > 0) {
				advancesById.get(imp.getRequiredAdvance()).getEnabledImprovements().add(imp);
			}
		}
		for (Government gov: governments) {
			if (advancesById.containsKey(gov.getRequiredAdvance())) {
				advancesById.get(gov.getRequiredAdvance()).getEnabledGovernments().add(gov);
			}
		}
	}
	
	public List<Improvement> getImprovements() {
		return gameImprovements;
	}
	
	public NativeHarness getNativeHarness() {
		return nativeHarness;
	}

	public List<UnitType> getUnitTypes() {
		return unitTypes;
	}

	public List<Advance> getAdvances() {
		return advances;
	}
	
	public static UnitType getUnitTypeById(int id) {
		return unitTypesById.get(id);
	}
	
	public static Improvement getImprovementTypeById(int id) {
		return improvementsById.get(id);
	}

	public static Advance getAdvanceById(int id) {
		return advancesById.get(id);
	}
	
	public static Government getGovernmentById(int id) {
		return governmentsById.get(id);
	}
	
	public List<AdvanceExpense> getCurrentResearchStatus() {
		AdvanceExpense[] costs = nativeHarness.getAdvanceExpenses();
		List<AdvanceExpense> rval = new ArrayList<AdvanceExpense>();
		
		for (AdvanceExpense e: costs) {
			if (e != null) {
				rval.add(e);
			}
		}
		
		Collections.sort(rval, new Comparator<AdvanceExpense>() {

			@Override
			public int compare(AdvanceExpense object1, AdvanceExpense object2) {
				return object1.getBulbs() - object2.getBulbs();
			}
			
		});
		
		return rval;
	}

	@Override
	public void recieveTilesetUpdate(String info) {
		if (this.nativeListener != null)
			this.nativeListener.recieveTilesetUpdate(info);
		this.lastTilesetProgress = info;
	}

	@Override
	public void setConnectionStatus(boolean isConnected) {
		
		if (this.nativeListener != null)
			this.nativeListener.setConnectionStatus(isConnected);
		this.lastConnectionStatus = isConnected;
	

	}
	
	public void loadUniversals() {
		Improvement[] imps = nativeHarness.loadImprovements();
		UnitType[] unitTypes = nativeHarness.loadUnitTypes();
		Advance[] advances = nativeHarness.loadAdvances();
		Government[] governments = nativeHarness.loadGovernments();
		
		addImprovements(imps);
		addUnitTypes(unitTypes);
		addAdvances(advances);
		addGovernments(governments);
		
		addAdvanceDependencies();
	}
	
	public void updateTilesetProgress(String chunk) {
		Log.i("Freeciv", String.format("Got a tileset chunk: %s", chunk));
		lastTilesetProgress = chunk;
		if (this.nativeListener != null) {
			this.nativeListener.recieveTilesetUpdate(chunk);
		}
	}
	
	public void hookNativeEventListener(NativeEventListener nel) {
		this.nativeListener = nel;
		if (lastTilesetProgress != null)
			nativeListener.recieveTilesetUpdate(lastTilesetProgress);
		nativeListener.setConnectionStatus(lastConnectionStatus );
	}

	public void unhookNativeEventListener(
			NativeAwareActivity nativeAwareActivity) {
		// Don't let an Out-of-order situation trash the listener
		if (this.nativeListener == nativeAwareActivity)
			this.nativeListener = null;
	}	
}
