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

import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import net.hackcasual.freeciv.game.Game;
import net.hackcasual.freeciv.game.SaveGameDB;
import net.hackcasual.freeciv.models.Advance;
import net.hackcasual.freeciv.models.AdvanceExpense;
import net.hackcasual.freeciv.models.Government;
import net.hackcasual.freeciv.models.Improvement;
import net.hackcasual.freeciv.models.Player;
import net.hackcasual.freeciv.models.UnitType;
import net.hackcasual.freeciv.views.Freeciv;
import net.hackcasual.freeciv.views.NativeAwareActivity;

import net.hackcasual.freeciv.R;

import android.app.Application;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
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
	
	static SaveGameDB savedGameDB;
	
	static Context context;
	
	static int lastTurn = -1;
	static int startTurn = 0;
	
	public enum SaveType {
		AUTO, // Triggered by a new turn, configured by the user
		MANUAL, // Triggered by the user
		FORCED // Triggered by leaving the app, only one save permitted
	}
	
	@Override
	public void onCreate() {
		Log.i("Freeciv", "Creating the app.");
		
		context = this.getApplicationContext();
		
		savedGameDB = new SaveGameDB(this);
		
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
	
	public static void checkForNewTurn() {
		Player p = NativeHarness.getPlayerInfo();
		
		if (lastTurn < 0) {
			lastTurn = p.getTurns();
			startTurn = p.getTurns();
		} else if (p.getTurns() != lastTurn) {
			lastTurn = p.getTurns();
			
			if ((lastTurn - startTurn) % Settings.getCurrentSettings().getTurnsBetweenAutoSaves() == 0) {
				saveGame(SaveType.AUTO);
			}
		}
		Log.d("FreeCiv", lastTurn + " " + p.getTurns() + " " + startTurn);

	}
	
	public static void saveGame(SaveType type) {

		Player p = NativeHarness.getPlayerInfo();		
		String baseGameName = String.format("%s_Y%s_T%d-%d", p.getName(), p.getYear(), p.getTurns(), System.currentTimeMillis() / 1000)
			.replaceAll("\\s", "_").replaceAll("\\.", "");

		switch (type) {
		case AUTO: {		
			File savesDirectory = new File(Settings.getCurrentSettings().getSavedGameDir() + "/auto");
			saveGameHelper(savesDirectory, baseGameName, p);
			clearSaveGames();
		} break;
		
		case FORCED: {
			File savesDirectory = new File(Settings.getCurrentSettings().getSavedGameDir() + "/forced");
			saveGameHelper(savesDirectory, baseGameName, p);		
		} break;
		case MANUAL: {
			File savesDirectory = new File(Settings.getCurrentSettings().getSavedGameDir());
			saveGameHelper(savesDirectory, baseGameName, p);
		} break;
		}
	}

	private static void clearSaveGames() {
		List<Game> savedGames = savedGameDB.listGames();
		
		TreeSet<Game> autoGames = new TreeSet<Game>(new Comparator<Game>(){
			@Override
			public int compare(Game arg0, Game arg1) {
				return arg1.getLastPlayed().compareTo(arg0.getLastPlayed());
			}
		});
		
		Set<Game> forcedSaves = new HashSet<Game>();
		
		for (Game g: savedGames) {
			if (!g.saveExists()) {
				cleanSaveFS(g); // Clean up anyway, we might have missed the screen shots
				savedGameDB.delete(g);
			}
			else if (g.getSavedGame().getParent().endsWith("auto"))
				autoGames.add(g);
			else if (g.getSavedGame().getParent().endsWith("forced"))
				forcedSaves.add(g);
		}
		
		int gameCount = 0;
		
		for (Game g: autoGames) {
			if (gameCount > Settings.getCurrentSettings().getMaxNumberOfAutoSaves()) {
				savedGameDB.markForDelete(g);
				cleanSaveFS(g);
				savedGameDB.delete(g);
			}
			gameCount++;
		}

		for (Game g: forcedSaves) {
			savedGameDB.markForDelete(g);
			cleanSaveFS(g);
			savedGameDB.delete(g);
		}
		
		savedGameDB.close();
	}
	
	private static void cleanSaveFS(Game g) {
		g.getSavedGame().delete();
		g.getOverlayView().delete();
		g.getSnapShot().delete();
	}

	public static void deleteGame(Game g) {
		savedGameDB.markForDelete(g);
		cleanSaveFS(g);
		savedGameDB.delete(g);
		savedGameDB.close();
	}
	
	//TODO: Leverage grayscale JPEG for smaller size, use the alpha channel
	private static Bitmap convertScreenShot(Bitmap mapView) {
		int dimension = Math.min(mapView.getHeight(), mapView.getWidth());
   
		Bitmap screenShot = Bitmap.createBitmap(dimension, dimension,
				   Bitmap.Config.RGB_565);
		
		Canvas c = new Canvas(screenShot);
		
		Paint paint = new Paint();
		ColorMatrix cm = new ColorMatrix();
		cm.setSaturation(0);
		ColorMatrixColorFilter f = new ColorMatrixColorFilter(cm);
		paint.setColorFilter(f);
		
		int xo = (mapView.getWidth() - dimension) / 2;
		int yo = (mapView.getHeight() - dimension) / 2;
		
		c.drawBitmap(mapView, -xo, -yo, paint);
		
		return screenShot;
	}
	
	private static void saveGameHelper(File destinationDirectory, String tag, Player p) {
		if (!destinationDirectory.exists())
			destinationDirectory.mkdirs();
		
		File savedGameFile = new File(destinationDirectory, tag + ".sav.gz");
		File snapShotFile = new File(destinationDirectory, tag + ".ss.jpg");
		File overlayFile = new File(destinationDirectory, tag + ".ol.jpg");
		
		NativeHarness.save(savedGameFile.getAbsolutePath());
					
		Game game = new Game(p.getUserName(), savedGameFile, snapShotFile, overlayFile, new Date(), p.getScore(),
				p.getTurns(), p.getPeopleName(), String.format("%s %s",p.getRulerName(), p.getName()), p.getYear(), p.getPopulation(), p.getFlagName());
		
		savedGameDB.storeGame(game);
		savedGameDB.close();
		try {
		       FileOutputStream out = new FileOutputStream(snapShotFile);
		       Bitmap screenShot = convertScreenShot(Freeciv.getMapView());		       
		       screenShot.compress(Bitmap.CompressFormat.JPEG, 85, out);
		       screenShot.recycle();
		       out = new FileOutputStream(overlayFile);
		       NativeHarness.getOverview().compress(Bitmap.CompressFormat.JPEG, 85, out);
		} catch (Exception e) {
		       e.printStackTrace();
		}
	}
	
	public static SaveType getSaveType(Game g) {
		if (g.getSavedGame().getParent().endsWith("auto"))
			return SaveType.AUTO;
		else if (g.getSavedGame().getParent().endsWith("forced"))
			return SaveType.FORCED;
		else
			return SaveType.MANUAL;
	}
	
	public static Bitmap getBigFlag(String flagName) {
		int flagResourceId = context.getResources().getIdentifier("net.hackcasual.freeciv:drawable/" + flagName + "_big", null, null);
		
		if (0 == flagResourceId)
			flagResourceId = R.drawable.unknown_big;
		
		return BitmapFactory.decodeResource(context.getResources(), flagResourceId);
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
