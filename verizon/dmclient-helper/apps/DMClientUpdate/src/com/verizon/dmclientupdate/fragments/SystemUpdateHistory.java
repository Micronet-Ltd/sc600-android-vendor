/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.content.res.Resources;
import android.os.Bundle;
import android.app.DialogFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.view.Window;
import android.content.DialogInterface;
import android.view.KeyEvent;

import java.io.IOException;

import org.json.JSONException;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.UpdateInfo;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.App;
import com.verizon.dmclientupdate.ParseJson;

public class SystemUpdateHistory extends DialogFragment implements View.OnClickListener{

    public static final String FRAGMENT_TAG =
            SystemUpdateHistory.class.getName();

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.system_update_history, null);
        TextView firstUpdate = (TextView)v.findViewById(R.id.first_update);
        TextView firstUpdateDate = (TextView)v.findViewById(R.id.first_update_date);
        TextView secondUpdate = (TextView)v.findViewById(R.id.second_update);
        TextView secondUpdateDate = (TextView)v.findViewById(R.id.second_update_date);
        Button okButton = (Button)v.findViewById(R.id.ok_button);
        okButton.setOnClickListener(this);
        Resources res = getResources();
        String defaultSwVersion = null;

        ParseJson parseJson = new ParseJson(Utils.getInstance(getActivity()));
        try {
            parseJson.getJSON();
            defaultSwVersion = parseJson.getDefaulSwVersion();
        } catch (IOException | JSONException e) {
            e.printStackTrace();
        }

        UpdateInfo updateInfo = App.getUpdateInfo();

        if(updateInfo.getmLastSuccessSwVersion() == null) {
            String resultFirstUpdate = String.format(res.getString(R.string.software_version), defaultSwVersion);
            firstUpdate.setText(resultFirstUpdate);
            firstUpdateDate.setVisibility(View.GONE);
            secondUpdate.setVisibility(View.GONE);
            secondUpdateDate.setVisibility(View.GONE);
        } else if(updateInfo.getmPreLastSuccessSwVersion() == null) {
            String resultFirstUpdate = String.format(res.getString(R.string.software_version), updateInfo.getmLastSuccessSwVersion());
            firstUpdate.setText(resultFirstUpdate);
            String firstUpdateTime = updateInfo.getmLastSuccessTime();
            if (null != firstUpdateTime) {
                firstUpdateDate.setText(firstUpdateTime);
                firstUpdateDate.setVisibility(View.VISIBLE);
            } else {
                firstUpdateDate.setVisibility(View.GONE);
            }
            secondUpdate.setVisibility(View.GONE);
            secondUpdateDate.setVisibility(View.GONE);
        } else {
            String resultFirstUpdate = String.format(res.getString(R.string.software_update_to), updateInfo.getmLastSuccessSwVersion());
            firstUpdate.setText(resultFirstUpdate);
            String resultFirstUpdateDate = updateInfo.getmLastSuccessTime();
            firstUpdateDate.setText(resultFirstUpdateDate);

            String resultSecondUpdate = String.format(res.getString(R.string.software_update_to), updateInfo.getmPreLastSuccessSwVersion());
            secondUpdate.setText(resultSecondUpdate);
            String resultSecondUpdateDate = updateInfo.getmPreLastSuccessTime();
            secondUpdateDate.setText(resultSecondUpdateDate);
        }

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return v;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.ok_button:
                dismiss();
                break;
        }
    }
}
