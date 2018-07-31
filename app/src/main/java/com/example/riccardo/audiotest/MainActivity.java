package com.example.riccardo.audiotest;

import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.Timer;

public class MainActivity extends AppCompatActivity {

    private boolean mEngineCreated = false;

    private Timer mLatencyUpdater;
    private static final long UPDATE_LATENCY_EVERY_MILLIS = 1000;

    private Button button1;
    private Button button2;
    private Button buttonMP3;

    private boolean isButton1Interacting=false;
    private boolean isButton2Interacting=false;

    TextView freqLabel1;
    TextView freqLabel2;

    private double toneFreq1;
    private double toneFreq2;

    private final double VOLUME = 0.25;

    static AssetManager assetManager;

    protected Context context;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.context = MainActivity.this;

        mEngineCreated = AudioEngine.create();
        AudioEngine.setBufferSizeInBursts(0);//this will set the audio latency through the number of burst

        // set a change listener on the SeekBar
        final SeekBar seekBar1 = findViewById(R.id.seekBar1);
        seekBar1.setProgress((int) AudioEngine.getToneFrequency(1), true);
        seekBar1.setOnSeekBarChangeListener(seekBarChangeListener1);

        final SeekBar seekBar2 = findViewById(R.id.seekBar2);
        seekBar1.setProgress((int) AudioEngine.getToneFrequency(2), true);
        seekBar2.setOnSeekBarChangeListener(seekBarChangeListener2);

        freqLabel1 = findViewById(R.id.textView1);
        freqLabel1.setText("Frequency: " + seekBar1.getProgress());

        freqLabel2 = findViewById(R.id.textView2);
        freqLabel2.setText("Frequency: " + seekBar2.getProgress());


        button1 = findViewById(R.id.button1);
        button1.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        isButton1Interacting = true;
                        AudioEngine.setToneOn(1, true);
                        break;
                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_CANCEL:
                        isButton1Interacting = false;
                        AudioEngine.setToneOn(1, false);
                        break;
                    case MotionEvent.ACTION_MOVE:
                        double newFreq = toneFreq1 + (motionEvent.getX(0) - 500) / 10;
                        freqLabel1.setText("Frequency: " + (int) newFreq);
                        seekBar1.setProgress((int) newFreq, true);
                        AudioEngine.resetTone(1, newFreq, VOLUME);
                        break;
                }
                return false;
            }
        });


        button2 = findViewById(R.id.button2);
        button2.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        isButton2Interacting = true;
                        AudioEngine.setToneOn(2, true);
                        break;
                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_CANCEL:
                        isButton2Interacting = false;
                        AudioEngine.setToneOn(2, false);
                        break;
                    case MotionEvent.ACTION_MOVE:
                        Log.d("DEBUG", "2: " + motionEvent.getX(motionEvent.getPointerCount() - 1));
                        double newFreq = toneFreq2 + (motionEvent.getX(motionEvent.getPointerCount() - 1) - 500) / 10;
                        freqLabel2.setText("Frequency: " + (int) newFreq);
                        seekBar2.setProgress((int) newFreq, true);
                        AudioEngine.resetTone(2, newFreq, VOLUME);
                        break;
                }
                return false;
            }
        });

        assetManager = getAssets();

        buttonMP3 = findViewById(R.id.button3);
        buttonMP3.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        boolean created = false;
                        if (!created) {
                            //int CDECL hip_decode( hip_t           gfp
                            //AudioEngine.createAssetAudioPlayer(assetManager, "track39CBR320.mp3");
                            AudioEngine.createAssetAudioPlayer(assetManager, "song.mp3");
                        }
                        if (created) {
                            //isPlayingAsset = !isPlayingAsset;
                            //setPlayingAssetAudioPlayer(isPlayingAsset);
                        }
                        break;
                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_CANCEL:
                        break;
                    case MotionEvent.ACTION_MOVE:
                        break;
                }
                return false;
            }
        });

        Button buttonTest = findViewById(R.id.buttonTest);
        buttonTest.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                switch (motionEvent.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        context.startActivity(new Intent(context, SettingsActivity.class));
                        break;
                }
                return false;
            }
        });
    }

    SeekBar.OnSeekBarChangeListener seekBarChangeListener1 = new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            // updated continuously as the user slides the thumb
            if (!isButton1Interacting) {
                freqLabel1.setText("Frequency: " + progress);
                toneFreq1 = progress;
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // called when the user first touches the SeekBar
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // called after the user finishes moving the SeekBar
            AudioEngine.resetTone(1, seekBar.getProgress(), VOLUME);
        }
    };

    SeekBar.OnSeekBarChangeListener seekBarChangeListener2 = new SeekBar.OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            // updated continuously as the user slides the thumb
            if (!isButton2Interacting) {
                freqLabel2.setText("Frequency: " + progress);
                toneFreq2 = progress;
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // called when the user first touches the SeekBar
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // called after the user finishes moving the SeekBar
            AudioEngine.resetTone(2, seekBar.getProgress(), VOLUME);
        }
    };


    /*
     * Hook to user control to start / stop audio playback:
     *    touch-down: start, and keeps on playing
     *    touch-up: stop.
     * simply pass the events to native side.
     */
    /*@Override
    public boolean onTouchEvent(MotionEvent event) {
        int action =event.getAction();// MotionEventCompat.getActionMasked(event);
        switch(action) {
            case (MotionEvent.ACTION_DOWN) :
                if (mEngineCreated)
                    AudioEngine.setToneOn(true);
                break;
            case (MotionEvent.ACTION_UP) :
                if (mEngineCreated)
                    AudioEngine.setToneOn(false);
                break;
        }
        return super.onTouchEvent(event);
    }*/

    @Override
    protected void onDestroy() {

        AudioEngine.delete();
        super.onDestroy();
    }
}