package com.robertch.seeboat;

import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import uk.co.chrisjenx.calligraphy.CalligraphyContextWrapper;


//See http://www.android-graphview.org/    for more details
public class MultiGraphDisplay extends AppCompatActivity {

    @Override
    protected void attachBaseContext(Context newBase) {
        super.attachBaseContext(CalligraphyContextWrapper.wrap(newBase));  //Needed in order to have custom fonts.
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_multiple_graph_display);

        Intent intent = getIntent();
        String message = intent.getStringExtra(SeeBoatMainActivity.EXTRA_MESSAGE);//0 is default


        TextView textView = (TextView) findViewById(R.id.textview);
        textView.setText("ID: " + message);



        GraphView graph = (GraphView) findViewById(R.id.graph);
        LineGraphSeries<DataPoint> series = new LineGraphSeries<>(new DataPoint[] {
                new DataPoint(0, 1),
                new DataPoint(1, 5),
                new DataPoint(2, 3),
                new DataPoint(3, 2),
                new DataPoint(4, 6)
        });
        graph.addSeries(series);
    }
}
