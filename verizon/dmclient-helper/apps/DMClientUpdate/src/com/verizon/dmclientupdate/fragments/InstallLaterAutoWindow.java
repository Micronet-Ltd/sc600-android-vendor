/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.content.DialogInterface;
import android.widget.Spinner;
import android.view.KeyEvent;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.notifications.*;

public class InstallLaterAutoWindow extends BaseLaterFragment {

    private CheckBox startInstallAutomaticallyCheckbox;
    private static final String DEFAULT_HOURS = "5";
    private static final String DEFAULT_MINUTES = "00";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.install_later_auto_window, container);

        startInstallAutomaticallyCheckbox = (CheckBox)view.findViewById(R.id.install_later_checkbox);
        Button installLaterOkButton = (Button) view.findViewById(R.id.install_later_ok_button);
        Button installLaterCancelButton = (Button) view.findViewById(R.id.install_later_cancel_button);
        installLaterOkButton.setOnClickListener(this);
        installLaterCancelButton.setOnClickListener(this);


        initSpinners(view, DEFAULT_HOURS, DEFAULT_MINUTES, PART_PM);

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        SystemAvaliableNotification.getInstance().remove(getActivity());
        switch (v.getId()) {
            case R.id.install_later_ok_button:
                if(startInstallAutomaticallyCheckbox.isChecked()) {
                    mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), getSeconds(), false, true, Utils.EBT_OK);
                    dismiss();
                    transactions(getFragmentManager(), new AutomaticallyInstallNotice());
                }else{
                    mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), getSeconds(), false, false, Utils.EBT_OK);
                    dismiss();
                    ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                }
                break;
            case R.id.install_later_cancel_button:
                dismiss();
                transactions(getFragmentManager(), new PostDownloadMessageWindow());
                break;
        }
    }
}
