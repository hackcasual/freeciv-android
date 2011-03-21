package net.hackcasual.freeciv.models;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class TileInfoMini {
	private final int cityId;
	private final List<Integer> unitIds;
	
	public TileInfoMini(int cityId, int[] unitIds) {
		this.cityId = cityId;
		List<Integer> dummy = new ArrayList<Integer>(unitIds.length);
		
		for (int i: unitIds)
			dummy.add(i);
		
		this.unitIds = Collections.unmodifiableList(dummy);
	}

	public int getCityId() {
		return cityId;
	}

	public List<Integer> getUnitIds() {
		return unitIds;
	}
	
	public boolean hasCity() {
		return cityId != -1;
	}
}
