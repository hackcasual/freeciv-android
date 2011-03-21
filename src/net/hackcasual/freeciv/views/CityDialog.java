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

package net.hackcasual.freeciv.views;

import java.util.List;

import net.hackcasual.freeciv.Civ;
import net.hackcasual.freeciv.NativeHarness;

import net.hackcasual.freeciv.R;
import net.hackcasual.freeciv.R.id;
import net.hackcasual.freeciv.R.layout;
import net.hackcasual.freeciv.models.Advance;
import net.hackcasual.freeciv.models.City;
import net.hackcasual.freeciv.models.CityView;
import net.hackcasual.freeciv.models.Unit;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class CityDialog extends Activity {
	TextView name, population, foodPerTurn, foodSurplus, productionName;
	ImageView cityMap, productionIcon;
	LinearLayout mapHolder;
	float dpFactor;
	float mapScale;
	MotionEvent lastMapMotion;
	boolean cancelClick;
	NativeHarness nh;
	int currentCity;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		dpFactor = metrics.density;	
        
        setContentView(R.layout.citydialog);

        name = (TextView)this.findViewById(R.id.city_name);
        population = (TextView)this.findViewById(R.id.city_population);
        foodPerTurn = (TextView)this.findViewById(R.id.city_food_per_turn);
        foodSurplus = (TextView)this.findViewById(R.id.city_food_surplus);
        productionName = (TextView)this.findViewById(R.id.current_production_name);
        cityMap = (ImageView)this.findViewById(R.id.city_map);
        productionIcon = (ImageView)this.findViewById(R.id.current_production_icon);
        mapHolder = (LinearLayout)this.findViewById(R.id.map_holder);
        
        
        nh = ((Civ)this.getApplication()).getNativeHarness();


        
        final int cityId = this.getIntent().getIntExtra("city-id", -1);
        currentCity = cityId;
        final City city = nh.getCity(cityId);
        final Activity curActivity = this;
        
        List<Unit> currentUnits = nh.getUnitsInCity(city);
        
        Log.i("Freeciv", "Embedded units: " + currentUnits);
        
        updateCityInfo(city);
        
        LinearLayout presentUnits = (LinearLayout)findViewById(R.id.present_units);
        
        /*ArrayAdapter<Unit> unitAdapter = new CityPresentUnitAdapter(this);
        presentUnits.setAdapter(unitAdapter);
        
        for (Unit u: currentUnits) {
        	unitAdapter.add(u);
        }*/
        
        for (final Unit u: currentUnits) {
        	/*LinearLayout impr = new LinearLayout(this);
            impr.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT));
            impr.setOrientation(LinearLayout.HORIZONTAL);

            ImageView iv = new ImageView(this);
            iv.setImageBitmap(u.getSprite());
            
            TextView name = new TextView(this);
            name.setText(u.getType().getName() + "\n" + String.format("(%d,%d,%d/%d) %d/%d", u.getType().getAttackStrength(), u.getType().getDefenseStrength(), u.getMovesLeft(), u.getType().getMoveRate(), u.getHitPoints(), u.getType().getHitPoints()));
            
            impr.addView(iv);
            impr.addView(name);
            presentUnits.addView(impr);*/
        	
    		LinearLayout curView = (LinearLayout) this.getLayoutInflater().inflate(R.layout.city_presentunit, null);
    		
    		
    		((ImageView)curView.findViewById(R.id.Unit_Icon)).setImageBitmap(u.getSprite());
    		((TextView)curView.findViewById(R.id.Unit_Info)).setText(u.getType().getName() + "\n" + String.format("(%d,%d,%d/%d) %d/%d", u.getType().getAttackStrength(), u.getType().getDefenseStrength(), u.getMovesLeft(), u.getType().getMoveRate(), u.getHitPoints(), u.getType().getHitPoints()));
    		
    		curView.setOnClickListener(new OnClickListener() {

				@Override
				public void onClick(View v) {
					nh.focusOnUnit(u.getUnitId());
					
				}});
    		
    		presentUnits.addView(curView);
        }

        ((Button)this.findViewById(R.id.change_production)).setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				Intent openProduction = new Intent(curActivity.getApplicationContext(), CityProductionView.class);
				openProduction.putExtra("cityId", cityId);
				//curActivity.startActivity(openProduction);
				curActivity.startActivityForResult(openProduction, 0);
			}
        	
        });

        
        mapHolder.setOnTouchListener(new View.OnTouchListener() {

			@Override
			public boolean onTouch(View arg0, MotionEvent arg1) {
				switch (arg1.getAction()) {
				case MotionEvent.ACTION_DOWN: lastMapMotion = arg1;	cancelClick = false; break;
				case MotionEvent.ACTION_UP: 
					if (!cancelClick && arg1.getEventTime() - arg1.getDownTime() < 500) {
						nh.clickCityMap(cityId, (int)(arg1.getX() / mapScale), (int)(arg1.getY()  / mapScale));
						updateCityInfo(nh.getCity(cityId));
					}
					break;
				case MotionEvent.ACTION_CANCEL: cancelClick = true; break;
				}
				return true;
			}
        	
        });
        
        Log.i("Freeciv", "CityDialog started");
    }
    
    @Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		updateCityInfo(nh.getCity(currentCity));
    }
    
    void updateCityInfo(City city) {
        name.setText(" " + city.getName());
        population.setText(" " + String.format("%,d", city.getPopulation() * 10000));
        foodPerTurn.setText(" " + String.valueOf(city.getFoodPerTurn()) + "  ");
        foodSurplus.setText(" " + String.valueOf(city.getSurplus()));       
        cityMap.setImageBitmap(city.getMap());

        float mapWidth = this.getWindowManager().getDefaultDisplay().getWidth() - (2 * 5 * dpFactor);
        mapScale = mapWidth / city.getMap().getWidth();
        Log.i("Freeciv", "Map width: " + mapWidth);
        
        
        
        cityMap.setLayoutParams(new LinearLayout.LayoutParams((int)(city.getMap().getWidth() * mapScale), 
        		(int)(city.getMap().getHeight() * mapScale)));
        productionIcon.setImageDrawable(city.getCurrentProduction().getIcon());
        productionName.setText(city.getCurrentProduction().getName() + " in " + city.getProductionTurns() + " turns. (" + city.getProductionStock() + ")" );

    }
}
