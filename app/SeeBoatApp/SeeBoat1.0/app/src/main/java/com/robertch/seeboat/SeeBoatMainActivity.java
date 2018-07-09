
package com.robertch.seeboat;

import android.app.Dialog;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.MenuItemCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.ShareActionProvider;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import com.droidbyme.dialoglib.AnimUtils;
import com.droidbyme.dialoglib.DroidDialog;
import com.igalata.bubblepicker.BubblePickerListener;
import com.igalata.bubblepicker.model.BubbleGradient;
import com.igalata.bubblepicker.model.PickerItem;
import com.igalata.bubblepicker.rendering.BubblePicker;
import com.igalata.bubblepicker.rendering.BubbleShader;

import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;

import uk.co.chrisjenx.calligraphy.CalligraphyContextWrapper;

import static android.R.id.message;


/**
 * The main activity for the sample. This activity registers a local service and
 * perform discovery over Wi-Fi p2p network. It also hosts a couple of fragments
 * to manage chat operations. When the app is launched, the device publishes a
 * chat service and also tries to discover services published by other peers. On
 * selecting a peer published service, the app initiates a Wi-Fi P2P (Direct)
 * connection with the peer. On successful connection with a peer advertising
 * the same service, the app opens up sockets to initiate a chat.
 * {@code WiFiChatFragment} is then added to the the main activity which manages
 * the interface and messaging needs for a chat session.
 */
public class SeeBoatMainActivity extends AppCompatActivity  {
    public static final String TAG = "seeboat";


    private static SeeBoatMainActivity ins;

    private WiFiDirectServicesList servicesList;

    public final static String EXTRA_MESSAGE = "com.robertch.SeeBoat1.0.MESSAGE";

    private Context context;

    BubblePicker picker;


    @Override
    protected void attachBaseContext(Context newBase) {
        super.attachBaseContext(CalligraphyContextWrapper.wrap(newBase));  //Needed in order to have custom fonts.
    }

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        GlobalVariableClass.getInstance().setKillThreads(true);

        ins = this;
        context = this;

        NotificationManager notificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        notificationManager.cancelAll();

        setContentView(R.layout.main);

        final String[] titles = new String[]{"testa", "testb", "testc"};


        final BubbleGradient bubbleGradientRed = new BubbleGradient(getResources().getColor(R.color.custom_red), getResources().getColor(R.color.custom_red_transparent), BubbleGradient.VERTICAL);
        final BubbleGradient bubbleGradientGreen = new BubbleGradient(getResources().getColor(R.color.custom_green), getResources().getColor(R.color.custom_green_transparent), BubbleGradient.VERTICAL);
        final BubbleGradient bubbleGradientBlue = new BubbleGradient(getResources().getColor(R.color.custom_dark_blue), getResources().getColor(R.color.custom_dark_blue_transparent), BubbleGradient.VERTICAL);

        picker = (BubblePicker) findViewById(R.id.picker);
        picker.setCenterImmediately(true);
        picker.setBubbleSize(10);
        picker.setItems(new ArrayList<PickerItem>() {{

            add(new PickerItem("Boat 1", getResources().getDrawable(R.mipmap.boat_white), true,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    bubbleGradientRed,
                    0.1f,
                    Typeface.DEFAULT,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    40f,
                    null,
                    false));
            add(new PickerItem("Boat 2", getResources().getDrawable(R.mipmap.boat_white), true,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    bubbleGradientGreen,
                    0.1f,
                    Typeface.DEFAULT,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    40f,
                    null,
                    false));
            add(new PickerItem("Boat 3", getResources().getDrawable(R.mipmap.boat_white), true,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    bubbleGradientBlue,
                    0.1f,
                    Typeface.DEFAULT,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    40f,
                    null,
                    false));
            add(new PickerItem("Boat 4", getResources().getDrawable(R.mipmap.boat_white), true,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    bubbleGradientRed,
                    0.1f,
                    Typeface.DEFAULT,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    40f,
                    null,
                    false));
            add(new PickerItem("Boat 5", getResources().getDrawable(R.mipmap.boat_white), true,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    bubbleGradientGreen,
                    0.1f,
                    Typeface.DEFAULT,
                    ContextCompat.getColor(SeeBoatMainActivity.this, android.R.color.white),
                    40f,
                    null,
                    false));
        }});

        picker.setListener(new BubblePickerListener() {
            @Override
            public void onBubbleSelected(@NotNull PickerItem item) {
                String boatName = item.getTitle();
                openDialog(boatName);

            }

            @Override
            public void onBubbleDeselected(@NotNull PickerItem item) {

            }
        });


        GlobalVariableClass.getInstance().setKillThreads(false);

        servicesList = new WiFiDirectServicesList();


        final ImageButton scan_bubble_button = (ImageButton) findViewById(R.id.button_pop_scan);
        scan_bubble_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                final Animation myAnim = AnimationUtils.loadAnimation(getApplicationContext(), R.anim.bounce);

                // Use bounce interpolator with amplitude 0.2 and frequency 20
                MyBounceInterpolator interpolator = new MyBounceInterpolator(0.1, 30);
                myAnim.setInterpolator(interpolator);

                scan_bubble_button.startAnimation(myAnim);
            }
        });


    }

    public void openDialog(final String boatName) {// Opens the dialog that provides details once a given boat is selected.
        new DroidDialog.Builder(context)
                .icon(R.mipmap.boat_white)
                .title(boatName)
                .content("This boat can display data on temperature, turbidity, and color. Last update: 22 minutes ago.")
                .cancelable(true, true)
                .positiveButton("See Data", new DroidDialog.onPositiveListener() {
                    @Override
                    public void onPositive(Dialog droidDialog) {
                        droidDialog.dismiss();
                        Toast.makeText(context, "See Data", Toast.LENGTH_SHORT).show();

                        Intent intent = new Intent(SeeBoatMainActivity.getInstanceMain(), MultiGraphDisplay.class);
                        intent.putExtra(EXTRA_MESSAGE, boatName);  //Fixme: 10 ie boat number
                        startActivity(intent);
                    }
                })
                .negativeButton("Cancel", new DroidDialog.onNegativeListener() {
                    @Override
                    public void onNegative(Dialog droidDialog) {
                        droidDialog.dismiss();
                        Toast.makeText(context, "Cancel", Toast.LENGTH_SHORT).show();
                    }
                })
                .typeface("Lato-Regular.ttf")
                .animation(AnimUtils.AnimZoomInOut)
                .color(ContextCompat.getColor(context, R.color.custom_light_blue), ContextCompat.getColor(context, R.color.custom_white),
                        ContextCompat.getColor(context, R.color.custom_white))
                .divider(true, ContextCompat.getColor(context, R.color.custom_green))
                .show();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate our menu from the resources by using the menu inflater.
        getMenuInflater().inflate(R.menu.main, menu);

        MenuItem shareItem = menu.findItem(R.id.action_share);
        ShareActionProvider mShareActionProvider = (ShareActionProvider) MenuItemCompat.getActionProvider(shareItem);

        Intent sendIntent = new Intent();
        sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_TEXT, "www.seeboat.com");
        sendIntent.setType("text/plain");
        // Get the ViewPager's current item position and set its ShareIntent.
        mShareActionProvider.setShareIntent(sendIntent);

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem menuItem) {
        switch (menuItem.getItemId()) {
            case R.id.action_settings:

                Intent intentSettings = new Intent(this, SettingsActivity.class);
                startActivity(intentSettings);

                return true;

            case R.id.action_share: //This is complete
                return true;

            default:
                // If we got here, the user's action was not recognized.
                // Invoke the superclass to handle it.
                return super.onOptionsItemSelected(menuItem);
        }
    }


    @Override
    protected void onStart() {
        super.onStart();
    }


    @Override
    public void onResume() {
        super.onResume();
        picker.onResume();  //A requirement for the bubblePicker library

    }


    @Override
    public void onPause() {
        super.onPause();
        picker.onPause();  //A requirement for the bubblePicker library
    }


    @Override
    protected void onStop() {
        super.onStop();
    }


    @Override
    protected void onRestart() {
        super.onRestart();
    }


    @Override
    public void onDestroy() {

        super.onDestroy();

    }



    public static SeeBoatMainActivity getInstanceMain() {
        return ins;
    }


}

