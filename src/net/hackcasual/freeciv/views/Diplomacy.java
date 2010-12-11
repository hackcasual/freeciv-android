package net.hackcasual.freeciv.views;

import net.hackcasual.freeciv.R;
import android.app.TabActivity;
import android.os.Bundle;
import android.widget.TabHost;

public class Diplomacy extends TabActivity {
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.diplomacy_tabs);

	    TabHost mTabHost = getTabHost();
	    
	    mTabHost.addTab(mTabHost.newTabSpec("tab_test1").setIndicator("TAB 1").setContent(R.id.textview1));
	    mTabHost.addTab(mTabHost.newTabSpec("tab_test2").setIndicator("TAB 2").setContent(R.id.textview2));
	    mTabHost.addTab(mTabHost.newTabSpec("tab_test3").setIndicator("TAB 3").setContent(R.id.textview3));
	    
	    mTabHost.setCurrentTab(0);
	}
}
