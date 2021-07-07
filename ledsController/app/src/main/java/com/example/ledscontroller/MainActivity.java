package com.example.ledscontroller;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.graphics.Color;
import android.os.Bundle;
import android.renderscript.ScriptGroup;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SearchView;
import android.widget.Spinner;
import android.widget.Toast;


import com.google.android.material.slider.Slider;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Arrays;

import yuku.ambilwarna.AmbilWarnaDialog;


public class MainActivity extends AppCompatActivity {
    int color1;
    int color2;
    int color3;
    Button button1;
    Button button2;
    Button button3;
    int brightness=254;
    int delay;
    BluetoothService bluetooth;
    ArrayList<String> modes = new ArrayList<String>(
            Arrays.asList("Static","Fade","Alternate","Rainbow","Palettes Cycle","Palettes")
    );
    ArrayList<String> palettes = new ArrayList<String>(
            Arrays.asList("Rainbow","Rainbow Stripe","Rainbow Stripe linear","Purple and Green","Random","Black and white",
                    "Black and white linear", "Cloud", "Party", "USA", "USA linear","Lava","Ocean","Heat","Forest","Españita","Custom")
    );





    /**
     * Method that is executed on activity creation.
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Filling the modes list with the prompts defined in the strings values
        Spinner mySpinner = (Spinner) findViewById(R.id.modeSelect);
        ArrayAdapter<String> myAdapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_list_item_1,getResources().getStringArray(R.array.MODES));
        myAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mySpinner.setAdapter(myAdapter);

        Spinner palettesSpinner = (Spinner) findViewById(R.id.palettes);
        ArrayAdapter<String> palettesAdapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_list_item_1,getResources().getStringArray(R.array.PALETTES));
        palettesAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        palettesSpinner.setAdapter(palettesAdapter);

        // Buttons to select colors
        setColorButtons();

        mySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parentView, View selectedItemView, int position, long id) {
                View palettesText = findViewById(R.id.palettesText);
                View palettesSpinner = findViewById(R.id.palettes);
                View button1 = findViewById(R.id.button1);
                View button2 = findViewById(R.id.button2);
                View button3 = findViewById(R.id.button3);

                // Palettes mode
                if (position >= 4){
                    palettesText.setVisibility(View.VISIBLE);
                    palettesSpinner.setVisibility(View.VISIBLE);

                }else{
                    palettesText.setVisibility(View.GONE);
                    palettesSpinner.setVisibility(View.GONE);
                }

                // alternate
                if (position == 2){
                    button2.setVisibility(View.VISIBLE);
                    button3.setVisibility(View.VISIBLE);

                }else{
                    button2.setVisibility(View.GONE);
                    button3.setVisibility(View.GONE);
                }

                if (position >2){
                    button1.setVisibility(View.GONE);
                }else{
                    button1.setVisibility(View.VISIBLE);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }


        });


        bluetooth = new BluetoothService();
        View buttonView = findViewById(R.id.fetch);
        fetchDevices(buttonView);


        //slider
        Slider slider = findViewById(R.id.bright);
        slider.addOnChangeListener(new Slider.OnChangeListener() {
            @Override
            public void onValueChange( Slider slider, float value, boolean fromUser) {
                brightness =  (int) value;
            }
        });

        Slider delaySlider = findViewById(R.id.delay);
        delaySlider.addOnChangeListener(new Slider.OnChangeListener() {
            @Override
            public void onValueChange( Slider delaySlider, float value, boolean fromUser) {
                delay =  (int) value;
            }
        });



    }

    private void setColorButtons() {
        button1 = (Button) findViewById(R.id.button1);
        button2 = (Button) findViewById(R.id.button2);
        button3 = (Button) findViewById(R.id.button3);
        button1.setOnClickListener(new View.OnClickListener(){
                                    @Override
                                    public void onClick(View v){
                                        openColorPicker(1);
                                    }
        }
        );
        button2.setOnClickListener(new View.OnClickListener(){
                                       @Override
                                       public void onClick(View v){
                                           openColorPicker(2);
                                       }
                                   }
        );
        button3.setOnClickListener(new View.OnClickListener(){
                                       @Override
                                       public void onClick(View v){
                                           openColorPicker(3);
                                       }
                                   }
        );
    }

    public void fetchDevices(View v){
        String [] devices = bluetooth.fetchPairedDevices();
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_list_item_1,devices);

        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        Spinner sItems = (Spinner) findViewById(R.id.bluetoothDevices);
        sItems.setAdapter(adapter);
        Toast.makeText(v.getContext(),
                "Available devices fetched", Toast.LENGTH_SHORT).show();
    }

    public void connectDevice(View v){
        Spinner mySpinner = (Spinner) findViewById(R.id.bluetoothDevices);
        String name = mySpinner.getSelectedItem().toString();
        bluetooth.connect(name);
        Toast.makeText(v.getContext(),
                "Connected to device", Toast.LENGTH_SHORT).show();
    }

    public void openColorPicker(int button) {
        if (button == 1){
            AmbilWarnaDialog dialog1 = new AmbilWarnaDialog(this, color1, new AmbilWarnaDialog.OnAmbilWarnaListener() {
                @Override
                public void onOk(AmbilWarnaDialog dialog, int color) {
                    // color is the color selected by the user.
                    color1 = color;
                    Log.i("Colors",String.valueOf(color1));
                    int red = Color.red(color1);
                    int green = Color.green(color1);
                    int blue = Color.blue(color1);
                    Log.i("Red",String.valueOf(red));
                    Log.i("Green",String.valueOf(green));
                    Log.i("Blue",String.valueOf(blue));
                }

                @Override
                public void onCancel(AmbilWarnaDialog dialog) {
                    // cancel was selected by the user

                }
            }

            );
            dialog1.show();

        }
        else if(button == 2){
            AmbilWarnaDialog dialog2 = new AmbilWarnaDialog(this, color2, new AmbilWarnaDialog.OnAmbilWarnaListener() {
                @Override
                public void onOk(AmbilWarnaDialog dialog, int color) {
                    // color is the color selected by the user.
                    color2 = color;
                    Log.i("Colors",String.valueOf(color2));
                }

                @Override
                public void onCancel(AmbilWarnaDialog dialog) {
                    // cancel was selected by the user

                }
            }

            );
            dialog2.show();

        }
        else{
            AmbilWarnaDialog dialog3 = new AmbilWarnaDialog(this, color3, new AmbilWarnaDialog.OnAmbilWarnaListener() {
                @Override
                public void onOk(AmbilWarnaDialog dialog, int color) {
                    // color is the color selected by the user.
                    color3 = color;
                    Log.i("Colors",String.valueOf(color3));
                }

                @Override
                public void onCancel(AmbilWarnaDialog dialog) {
                    // cancel was selected by the user

                }
            }

            );
            dialog3.show();

        }


    }

    public void sendData(View v) throws IOException, JSONException {
        JSONObject obj = new JSONObject();

        Spinner spinner = (Spinner)findViewById(R.id.modeSelect);
        String mode = spinner.getSelectedItem().toString();
        int indexMode = modes.indexOf(mode);

        Spinner paletteSpinner = (Spinner)findViewById(R.id.palettes);
        String palette = paletteSpinner.getSelectedItem().toString();
        int indexPalette = palettes.indexOf(palette);

        obj.put("mode",indexMode);
        obj.put("br",brightness);
        obj.put("de",delay);
        obj.put("c1r",Color.red(color1));
        obj.put("c1g",Color.green(color1));
        obj.put("c1b",Color.blue(color1));
        obj.put("pa",indexPalette);

/*
        JSONObject obj2 = new JSONObject();

        obj2.put("c2r",Color.red(color2));
        obj2.put("c2g",Color.green(color2));
        obj2.put("c2b",Color.blue(color2));

        obj2.put("c3r",Color.red(color3));
        obj2.put("c3g",Color.green(color3));
        obj2.put("c3b",Color.blue(color3));
*/

        String jsonText = obj.toString();
        bluetooth.write(jsonText);
/*
        String jsonText2 = obj2.toString();
        bluetooth.write(jsonText2);

 */
        Toast.makeText(v.getContext(),
                "Data sent", Toast.LENGTH_SHORT).show();
    }

}


