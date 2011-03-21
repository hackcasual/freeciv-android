package net.hackcasual.freeciv.game;

import java.io.File;
import java.text.DateFormat;
import java.util.Date;

/*
 * Player name
 * Saved game file
 * home city image file
 * Overlay map file
 * score
 * Last played
 * Current turns
 * Civilization name
 * Civilization leader name
 * Year
 * Population
 * Civilization flag ID
 *
 *
 * (saveFileName TEXT, playerName TEXT, homeCityImageFileName TEXT, overlayImageFileName TEXT, score INTEGER,
 * lastPlayed INTEGER, turnCount INTEGER, civName TEXT, civLeader TEXT, year TEXT, population INTEGER, flagString TEXT)  
 *
 */

public class Game {
	final String playerName;
	final File savedGame;
	final File snapShot;
	final File overlayView;
	final Date lastPlayed;
	final int score;
	final int turn;
	final String civName;
	final String civLeader;
	final String formattedYear;
	final int population;
	final String flagResource;
	
	public Game(String playerName, File savedGame, File snapShot, File overlayView, Date lastPlayed, int score,
			int turn, String civName, String civLeader, String formattedYear, int population,
			String flagResource)
	{
		this.playerName = playerName;
		this.savedGame = savedGame;
		this.snapShot = snapShot;
		this.overlayView = overlayView;
		this.lastPlayed = lastPlayed;
		this.score = score;
		this.turn = turn;
		this.civName = civName;
		this.civLeader = civLeader;
		this.formattedYear = formattedYear;
		this.population = population;
		this.flagResource = flagResource;
	}
	
	@Override
	public String toString()
	{
		return String.format("Player: %s Saved Game: %s Snapshot File: %s Overlay: %s Last Played on: %s Score: %d " + 
				"Turn: %d Civilization Name: %s Civilization Leader: %s Year: %s Population: %d Flag: %s", 
				playerName, savedGame, snapShot.getAbsolutePath(), overlayView.getAbsolutePath(), DateFormat.getDateInstance().format(lastPlayed),
				score, turn, civName, civLeader, formattedYear, population, flagResource);
	}

	public String getPlayerName() {
		return playerName;
	}

	public File getSavedGame() {
		return savedGame;
	}

	public File getSnapShot() {
		return snapShot;
	}

	public File getOverlayView() {
		return overlayView;
	}

	public Date getLastPlayed() {
		return lastPlayed;
	}

	public int getScore() {
		return score;
	}

	public int getTurn() {
		return turn;
	}

	public String getCivName() {
		return civName;
	}

	public String getCivLeader() {
		return civLeader;
	}

	public String getFormattedYear() {
		return formattedYear;
	}

	public int getPopulation() {
		return population;
	}

	public String getFlagResource() {
		return flagResource;
	}
	
	public boolean saveExists() {
		return savedGame.exists();
	}
	
}
