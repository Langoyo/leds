package com.example.ledscontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelUuid;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Set;

public class BluetoothService {
    OutputStream outputStream;
    InputStream inStream;
    BluetoothAdapter blueAdapter;
    public BluetoothService(){
        blueAdapter = BluetoothAdapter.getDefaultAdapter();

    }
    public String[] fetchPairedDevices(){

        if (blueAdapter.isEnabled()) {
            Set<BluetoothDevice> bondedDevices = blueAdapter.getBondedDevices();
            ArrayList<BluetoothDevice> devices = new ArrayList<BluetoothDevice>();
            if(bondedDevices.size() > 0) {
                for (Object device:bondedDevices){
                    devices.add( (BluetoothDevice) device );
                }
                String[] names = new String[devices.size()];
                for (int i = 0; i < devices.size(); i++) {
                    names[i] = devices.get(i).getName();

                }
                return names;
            }else{
                Log.e("error", "No appropriate paired devices.");
            }
        } else {
            Log.e("error", "Bluetooth is disabled.");
        }
        String [] names = {"No devices found"};
        return names;


    }

    public void connect(String name){
        Set<BluetoothDevice> bondedDevices = blueAdapter.getBondedDevices();
        ArrayList<BluetoothDevice> devices = new ArrayList<BluetoothDevice>();
        if(bondedDevices.size() > 0) {
            for (Object device:bondedDevices){
                devices.add( (BluetoothDevice) device );
            }
            String[] names = new String[devices.size()];
            for (int i = 0; i < bondedDevices.size(); i++) {
                devices.get(i).getName();
                names[i] = devices.get(i).getName();
            if(names[i].equals(name)) {
                BluetoothDevice device = (BluetoothDevice) devices.get(i);
                ParcelUuid[] uuids = device.getUuids();
                BluetoothSocket socket = null;
                try {
                    socket = device.createRfcommSocketToServiceRecord(uuids[0].getUuid());
                    socket.connect();
                    outputStream = socket.getOutputStream();
                    inStream = socket.getInputStream();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                break;
            }



            }

        }




    }

    private void mñe() throws IOException {
        blueAdapter = BluetoothAdapter.getDefaultAdapter();
        if (blueAdapter != null) {
            if (blueAdapter.isEnabled()) {
                Set<BluetoothDevice> bondedDevices = blueAdapter.getBondedDevices();

                if(bondedDevices.size() > 0) {
                    BluetoothDevice[] devices = (BluetoothDevice []) bondedDevices.toArray();
                    BluetoothDevice device = (BluetoothDevice) devices[0];
                    ParcelUuid[] uuids = device.getUuids();
                    BluetoothSocket socket = device.createRfcommSocketToServiceRecord(uuids[0].getUuid());
                    socket.connect();
                    outputStream = socket.getOutputStream();
                    inStream = socket.getInputStream();
                }

                Log.e("error", "No appropriate paired devices.");
            } else {
                Log.e("error", "Bluetooth is disabled.");
            }
        }
    }

    public void write(String s) throws IOException {
        outputStream.write(s.getBytes());
    }

    public void run() {
        final int BUFFER_SIZE = 1024;
        byte[] buffer = new byte[BUFFER_SIZE];
        int bytes = 0;
        int b = BUFFER_SIZE;

        while (true) {
            try {
                bytes = inStream.read(buffer, bytes, BUFFER_SIZE - bytes);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}
