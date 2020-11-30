/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */


package com.verizon.dmclientupdate;

import android.content.Context;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;

import android.os.Environment;
import android.os.ParcelFileDescriptor;


public class ParseJson {
    private static final String TAG = ParseJson.class.getSimpleName();
    String everything;
    JSONObject jsonObject;
    Utils mUtils;

    public ParseJson(Utils utils) {
        mUtils = utils;
    }

    private static final String JSON_PATH = Environment.getDataDirectory()+"/vendor/verizon/dmclient/data/" + "updateInfo.json";
    private static final String FIRMWARE_PATH = Environment.getDataDirectory()+"/vendor/verizon/dmclient/data/" + "last_update_firmware_version";

    public void getJSON() throws JSONException {

        try {
            BufferedReader br = null;
            File file = new File(JSON_PATH);
            if (file.canRead()) {
                br = new BufferedReader(new FileReader(file));
            }
            if (br == null) {
                ParcelFileDescriptor pfd = mUtils.inputStream(JSON_PATH);
                if (pfd != null) {
                    InputStream inputStream =
                            new ParcelFileDescriptor.AutoCloseInputStream(pfd);
                    br = new BufferedReader(new InputStreamReader(inputStream));
                }
            }
            if (br == null) {
                Log.w(TAG, "Could not read file :" + JSON_PATH);
                return;
            }
            StringBuilder sb = new StringBuilder();
            String line = br.readLine();
            while (line != null) {
                sb.append(line);
                sb.append(System.lineSeparator());
                line = br.readLine();
            }
            everything = sb.toString();
            Log.d(TAG, everything);
            if(everything.length() > 0) {
                jsonObject = new JSONObject(everything);
                JSONArray jsonArray = jsonObject.optJSONArray("updateInfo");
                App.saveUpdateInfo(new UpdateInfo(jsonArray));
            } else {
                Log.e(TAG, "File is empty!!!");
            }
        } catch (IOException | JSONException e1) {
            e1.printStackTrace();
        }
    }

    public void read() throws JSONException {
        getJSON();
    }

    public String getDefaulSwVersion() throws IOException {
        String firmware = null;
        BufferedReader br = null;
        try {
            if (br == null) {
                ParcelFileDescriptor pfd = mUtils.inputStream(FIRMWARE_PATH);
                if (pfd != null) {
                    InputStream inputStream =
                            new ParcelFileDescriptor.AutoCloseInputStream(pfd);
                    br = new BufferedReader(new InputStreamReader(inputStream));
                }
            }
            if (br == null) {
                Log.w(TAG, "Could not read file :" + FIRMWARE_PATH);
                return null;
            }
            firmware = br.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.i(TAG, "Use current firmware version :" + FIRMWARE_PATH);
        Log.i(TAG, "Current firmware = " + firmware);
        return firmware;
    }
}
