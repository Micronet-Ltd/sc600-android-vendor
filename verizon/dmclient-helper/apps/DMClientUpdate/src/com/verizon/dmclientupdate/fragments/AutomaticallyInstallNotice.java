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
import com.verizon.dmclientupdate.OmadmFumoDialog;

public class AutomaticallyInstallNotice extends BaseFragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.automatically_update_notice, container);
        Button startAutomaticallyNoticeOkButton = (Button)view.findViewById(R.id.start_automatically_notice_ok_button);
        startAutomaticallyNoticeOkButton.setOnClickListener(this);
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        switch (v.getId()) {
            case R.id.start_automatically_notice_ok_button:
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
        }
    }
}
