package com.robertch.seeboat;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;

public class SplashActivity extends AppCompatActivity {



    private final int SPLASH_DISPLAY_LENGTH = 1500;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        new Handler().postDelayed(new Runnable(){
            @Override
            public void run() {
                /* Create an Intent that will start the Menu-Activity. */
                Intent mainIntent = new Intent(SplashActivity.this, SeeBoatMainActivity.class);
                SplashActivity.this.startActivity(mainIntent);
                SplashActivity.this.finish();
            }
        }, SPLASH_DISPLAY_LENGTH);
    }
}