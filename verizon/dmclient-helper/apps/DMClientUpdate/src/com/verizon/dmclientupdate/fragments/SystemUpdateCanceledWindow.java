/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;
import android.view.KeyEvent;
import android.content.DialogInterface;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.Utils;

public class SystemUpdateCanceledWindow extends BaseFragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.system_update_canceled_notice, container);
        TextView systemUpdateCanceledMsg = (TextView)view.findViewById(R.id.system_update_canceled_msg);

        Button okButton = (Button)view.findViewById(R.id.ok_button);
        okButton.setOnClickListener(this);

        if(guiMessageDescriptor.getmState() == Utils.STATE_DOWNLOAD_COMPLETE) {
            systemUpdateCanceledMsg.setText(R.string.system_update_canceled_install_msg);
        }

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        switch (v.getId()) {
            case R.id.ok_button:
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
        }
    }
}
