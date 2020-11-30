/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.DialogFragment;
import android.content.res.Resources;
import android.os.Build;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.view.Window;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.App;

public class CheckUpdateFragment extends DialogFragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.check_update_fragment, container);
        Resources res = App.getContext().getResources();
        TextView textCheckForUpdate = (TextView)view.findViewById(R.id.text_check_for_update);
        String resultCheckForUpdate = String.format(res.getString(R.string.checking_toast), Build.MANUFACTURER, Build.MODEL);
        textCheckForUpdate.setText(resultCheckForUpdate);
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }
}
