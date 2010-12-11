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

public class AdvanceExpense {
	private final int steps;
	private final int bulbs;
	private final Advance target;
	
	public AdvanceExpense(int advanceId, int steps, int bulbs) {
		this.target = Civ.getAdvanceById(advanceId);
		this.steps = steps;
		this.bulbs = bulbs;
	}

	public int getSteps() {
		return steps;
	}

	public int getBulbs() {
		return bulbs;
	}

	public Advance getTarget() {
		return target;
	}
}
