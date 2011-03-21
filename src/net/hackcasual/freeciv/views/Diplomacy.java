package net.hackcasual.freeciv.views;

import net.hackcasual.freeciv.AvailableClauseAdapter;
import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.models.DiplomacyClause;
import android.app.Activity;
import android.app.TabActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ScrollView;
import android.widget.TabHost;
import android.widget.Toast;

public class Diplomacy extends TabActivity {
	ScrollView yours, theirs;
	
	AvailableClauseAdapter availableClauses;
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.diplomacy_tabs);

	    TabHost mTabHost = getTabHost();
	    
	    mTabHost.addTab(mTabHost.newTabSpec("tab_test1").setIndicator("Your Pacts").setContent(R.id.your_pacts));	    
	    mTabHost.addTab(mTabHost.newTabSpec("tab_test2").setIndicator("Their Pacts").setContent(R.id.their_pacts));
	    mTabHost.addTab(mTabHost.newTabSpec("tab_test3").setIndicator("TAB 3").setContent(R.id.textview3));
	    
	    yours = (ScrollView) this.findViewById(R.id.your_pacts);
	    
	    LinearLayout yourGeneralClauses = (LinearLayout) yours.findViewById(R.id.general_clauses);
	    LinearLayout yourConditions = (LinearLayout) yours.findViewById(R.id.conditions_list);
	    LinearLayout yourAdvanceClauses = (LinearLayout) yours.findViewById(R.id.advance_clauses);
	    LinearLayout yourCityClauses = (LinearLayout) yours.findViewById(R.id.city_clauses);
	    final Activity ctx = this;
	    
		availableClauses = new AvailableClauseAdapter(this, new AvailableClauseAdapter.clickCallback() {

			@Override
			public void onItemClicked(int index) {
				Toast toast = Toast.makeText(ctx, String.format("You clicked %d: %s", index, availableClauses.getItem(index).getName()), Toast.LENGTH_SHORT);
				toast.show();
			}
			
		});
		//yourGeneralClauses.setAdapter(availableClauses);
		
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 1"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 2"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 3"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 4"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 5"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 6"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 7"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 8"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 9"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 10"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 11"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 12"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 13"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 14"));
		availableClauses.add(new DiplomacyClause(0,0,0,"Test 15"));
		
		
		for (int pos = 0; pos < availableClauses.getCount(); pos++) {
			yourGeneralClauses.addView(availableClauses.getView(pos, null, null));
		}
		
		for (int pos = 0; pos < availableClauses.getCount(); pos++) {
			yourCityClauses.addView(availableClauses.getView(pos, null, null));
		}
		for (int pos = 0; pos < availableClauses.getCount(); pos++) {
			yourAdvanceClauses.addView(availableClauses.getView(pos, null, null));
		}
		
	    mTabHost.setCurrentTab(0);
	}
}
