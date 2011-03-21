package net.hackcasual.freeciv;

import java.util.Collection;
import java.util.LinkedList;

import android.content.Context;
import android.os.Environment;
import android.util.DisplayMetrics;

public class Settings {
	static final Settings defaultSettings = new Settings(3, 5, "Sparky","/data/data/net.hackcasual.freeciv", true);
	static Settings currentSettings = defaultSettings;
	
	static Collection<Runnable> updateListeners = new LinkedList<Runnable>();
	
	public static void addSettingsUpdateListener(Runnable listener) {
		updateListeners.add(listener);
	}
	
	public static void removeSettingsUpdateListener(Runnable listener) {
		updateListeners.remove(listener);
	}
	
	public static Settings getCurrentSettings() {
		return currentSettings;
	}
	
	private final int turnsBetweenAutoSaves;
	private final int maxNumberOfAutoSaves;
	private final String defaultUserName;
	private final String savedGameDir;
	private final boolean halveResolution;
	
	public static void updateSettings(Context c) {
		currentSettings = new Settings(currentSettings.turnsBetweenAutoSaves,
				currentSettings.maxNumberOfAutoSaves,
				currentSettings.defaultUserName,
				Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)?"/sdcard/Freeciv/saves/":currentSettings.savedGameDir,
				c.getResources().getDisplayMetrics().densityDpi > DisplayMetrics.DENSITY_MEDIUM);
		
		for (Runnable r: updateListeners) {
			r.run();
		}
	}
	
	public boolean isHalveResolution() {
		return halveResolution;
	}

	public int getTurnsBetweenAutoSaves() {
		return turnsBetweenAutoSaves;
	}
	
	public int getMaxNumberOfAutoSaves() {
		return maxNumberOfAutoSaves;
	}
	
	public String getDefaultUserName() {
		return defaultUserName;
	}
	
	public String getSavedGameDir() {
		return savedGameDir;
	}
	
	private Settings(int turnsBetweenAutoSaves, int maxNumberOfAutoSaves, String defaultUserName, String savedGameDir, boolean halveResolution) {
		this.turnsBetweenAutoSaves = turnsBetweenAutoSaves;
		this.maxNumberOfAutoSaves = maxNumberOfAutoSaves;
		this.defaultUserName = defaultUserName;
		this.savedGameDir = savedGameDir;
		this.halveResolution = halveResolution;
	}
}
