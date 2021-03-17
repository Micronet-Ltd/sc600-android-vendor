/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.KeyEvent;
import android.content.DialogInterface;
import android.widget.Button;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;

public class NotConnectedWiFiNotice extends BaseFragment implements View.OnClickListener {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.not_connected_wifi_notice, container);

        Button  notConnectedWifiOkButton = (Button)view.findViewById(R.id.not_connected_wifi_ok_button);
        notConnectedWifiOkButton.setOnClickListener(this);

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onClick(View view) {
        mCloseAppWithNotificationFlag = false;
        switch (view.getId()) {
            case R.id.not_connected_wifi_ok_button:
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
        }
    }
}
