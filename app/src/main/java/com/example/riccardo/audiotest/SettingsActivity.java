package com.example.riccardo.audiotest;

import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.support.v7.preference.PreferenceFragmentCompat;


public class SettingsActivity extends PreferenceActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        //poniamo la possibilità sulla action bar del tasto indietro
        if(getActionBar()!=null)
            getActionBar().setDisplayHomeAsUpEnabled(true);

        //ci creiamo il nostro fragment
        //frag = new SettingsFragment(); //classe creata più giù

        //getFragmentManager().beginTransaction()
        //		.replace(android.R.id.content, frag).commit();
    }

    public static class SettingsFragment extends PreferenceFragmentCompat
    {
        @Override
        public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
            addPreferencesFromResource(R.xml.prefs);
            setRetainInstance(true);
        }
    }

}
