package net.hackcasual.freeciv.models;

public class DiplomacyClause {
	public enum Type {
		ADVANCE, 
		GOLD, 
		MAP,
		SEAMAP, 
		CITY, 
		CEASEFIRE, 
		PEACE, 
		ALLIANCE,
		VISION, 
		EMBASSY
	}
	
	private final int value;
	private final Type type;
	private final int playerFrom;
	private final String name;
	
	public DiplomacyClause(int value, int type, int playerNumber, String name) {
		this.value = value;
		this.type = Type.values()[type];
		this.playerFrom = playerNumber;
		this.name = name;
	}

	public int getValue() {
		return value;
	}

	public Type getType() {
		return type;
	}

	public int getPlayerFrom() {
		return playerFrom;
	}

	public String getName() {
		return name;
	}
	
}
