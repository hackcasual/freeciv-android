package net.hackcasual.freeciv.game;

import java.io.File;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

public class SaveGameDB {


    private static final String SAVE_GAME_TABLE_NAME = "saved_games";
    private static final String DATABASE_NAME = "freeciv";
    private static final int DATABASE_VERSION = 2;
	
    private final String SAVE_FILE = "saveFileName";
    private final String PLAYER_NAME = "playerName";
    private final String SNAP_SHOT_FILE = "homeCityImageFileName";
    private final String OVERLAY_FILE = "overlayImageFileName";
    private final String SCORE = "score";
    private final String LAST_PLAYED = "lastPlayed";
    private final String TURN_COUNT = "turnCount";
    private final String CIV_NAME = "civName";
    private final String CIV_LEADER = "civLeader";
    private final String YEAR = "year";
    private final String POPULATION = "population";
    private final String FLAG_ID = "flagString";
    private final String DELETE_MARK = "deleted";

    private final SaveGameDBOpenHelper mDatabaseOpenHelper;
    
    public SaveGameDB(Context context) {
    	mDatabaseOpenHelper = new SaveGameDBOpenHelper(context);
    }
    
    private ContentValues gameToValues(Game game) {
        ContentValues initialValues = new ContentValues();
        
        initialValues.put(SAVE_FILE, game.savedGame.getAbsolutePath());            
        initialValues.put(PLAYER_NAME, game.playerName);
        initialValues.put(SNAP_SHOT_FILE, game.snapShot.getAbsolutePath());            
        initialValues.put(OVERLAY_FILE, game.overlayView.getAbsolutePath());
        initialValues.put(SCORE, game.score);            
        initialValues.put(LAST_PLAYED, game.lastPlayed.getTime());
        initialValues.put(TURN_COUNT, game.turn);            
        initialValues.put(CIV_NAME, game.civName);
        initialValues.put(CIV_LEADER, game.civLeader);            
        initialValues.put(YEAR, game.formattedYear);
        initialValues.put(POPULATION, game.population);            
        initialValues.put(FLAG_ID, game.flagResource);
        
        return initialValues;
    }
    
    /**
     * Add a word to the dictionary.
     * @return rowId or -1 if failed
     */
    public long storeGame(Game game) {
        
        return mDatabaseOpenHelper.getWritableDatabase().insert(SAVE_GAME_TABLE_NAME, null, gameToValues(game));
    }
    
    /**
     * Add a word to the dictionary.
     * @return rowId or -1 if failed
     */
    public List<Game> listGames() {
    	LinkedList<Game> allGames = new LinkedList<Game>();

    	Cursor gameCursor = mDatabaseOpenHelper.getReadableDatabase().query(SAVE_GAME_TABLE_NAME, new String[]{SAVE_FILE,
    			PLAYER_NAME, SNAP_SHOT_FILE, OVERLAY_FILE, SCORE, LAST_PLAYED, TURN_COUNT, CIV_NAME,
    			CIV_LEADER, YEAR, POPULATION, FLAG_ID}, null, null, null, null, LAST_PLAYED + " DESC", null);

    	while (gameCursor.moveToNext()) {
    		allGames.add(new Game(gameCursor.getString(gameCursor.getColumnIndexOrThrow(PLAYER_NAME)),
    				new File(gameCursor.getString(gameCursor.getColumnIndexOrThrow(SAVE_FILE))),
    				new File(gameCursor.getString(gameCursor.getColumnIndexOrThrow(SNAP_SHOT_FILE))),
    				new File(gameCursor.getString(gameCursor.getColumnIndexOrThrow(OVERLAY_FILE))),
    				new Date(gameCursor.getLong(gameCursor.getColumnIndexOrThrow(LAST_PLAYED))),
    				gameCursor.getInt(gameCursor.getColumnIndexOrThrow(SCORE)),
    				gameCursor.getInt(gameCursor.getColumnIndexOrThrow(TURN_COUNT)),
    				gameCursor.getString(gameCursor.getColumnIndexOrThrow(CIV_NAME)),
    				gameCursor.getString(gameCursor.getColumnIndexOrThrow(CIV_LEADER)),
    				gameCursor.getString(gameCursor.getColumnIndexOrThrow(YEAR)),
    				gameCursor.getInt(gameCursor.getColumnIndexOrThrow(POPULATION)),
    				gameCursor.getString(gameCursor.getColumnIndexOrThrow(FLAG_ID))
    				));
    	}
    	
		gameCursor.close();
    	
        return allGames;
    }
	
    public void markForDelete(Game game) {
    	ContentValues updateVals = gameToValues(game);
    	updateVals.put(DELETE_MARK, true);
    	mDatabaseOpenHelper.getReadableDatabase().update(SAVE_GAME_TABLE_NAME, updateVals, SAVE_FILE + "=?", new String[] {game.savedGame.getAbsolutePath()});
    }
    
    public void delete(Game game) {
    	mDatabaseOpenHelper.getReadableDatabase().delete(SAVE_GAME_TABLE_NAME, SAVE_FILE + "=?", new String[] {game.savedGame.getAbsolutePath()});
    }

    public void close() {
    	mDatabaseOpenHelper.close();
    }
    
    /**
     * This creates/opens the database.
     */
    private static class SaveGameDBOpenHelper extends SQLiteOpenHelper {

        private final Context mHelperContext;
        private SQLiteDatabase mDatabase;
              
        /* Note that FTS3 does not support column constraints and thus, you cannot
         * declare a primary key. However, "rowid" is automatically used as a unique
         * identifier, so when making requests, we will use "_id" as an alias for "rowid"
         */
        private static final String SAVE_TABLE_CREATE =
                    "CREATE TABLE " + SAVE_GAME_TABLE_NAME +
                    " (saveFileName TEXT, playerName TEXT, homeCityImageFileName TEXT, overlayImageFileName TEXT, score INTEGER," +
                    "lastPlayed INTEGER, turnCount INTEGER, civName TEXT, civLeader TEXT, year TEXT, population INTEGER, flagString TEXT, deleted INTEGER);";

        SaveGameDBOpenHelper(Context context) {
            super(context, DATABASE_NAME, null, DATABASE_VERSION);
            mHelperContext = context;
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            mDatabase = db;
            mDatabase.execSQL(SAVE_TABLE_CREATE);
        }

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            Log.w("FreecivDB", "Upgrading database from version " + oldVersion + " to "
                    + newVersion + ", which will destroy all old data");
            db.execSQL("DROP TABLE IF EXISTS " + SAVE_GAME_TABLE_NAME);
            onCreate(db);
        }
        
        
    }

}
