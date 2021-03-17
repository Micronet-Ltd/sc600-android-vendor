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
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.content.DialogInterface;
import android.view.KeyEvent;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.notifications.*;

public class DownloadLaterWindow extends BaseLaterFragment
implements OnCheckedChangeListener{
    private CheckBox whenWiFiIsAvaliableCheckBox;
    private ViewGroup mSchedulerBox;
    private static final String DEFAULT_HOURS = "1";
    private static final String DEFAULT_MINUTES = "00";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.download_later_window, container);

        whenWiFiIsAvaliableCheckBox = (CheckBox)view.findViewById(R.id.download_later_checkbox);
        whenWiFiIsAvaliableCheckBox.setOnCheckedChangeListener(this);
        Button downloadLaterOkButton = (Button)view.findViewById(R.id.download_later_ok_button);
        Button downloadLaterCancelButton = (Button)view.findViewById(R.id.download_later_cancel_button);

        downloadLaterOkButton.setOnClickListener(this);
        downloadLaterCancelButton.setOnClickListener(this);

        mSchedulerBox = (ViewGroup)(view.findViewById(R.id.scheduler_box));

        initSpinners(view, DEFAULT_HOURS, DEFAULT_MINUTES, PART_AM);

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        SystemAvaliableNotification.getInstance().remove(getActivity());
        switch (v.getId()){
            case R.id.download_later_ok_button:
                if(whenWiFiIsAvaliableCheckBox.isChecked()){
                    mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), getSeconds(), true, false, Utils.EBT_OK);
                    dismiss();
                    transactions(getFragmentManager(), new AutomaticallyDownloadNotice());
                }else {
                    mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), getSeconds(), false, false, Utils.EBT_OK);
                    dismiss();
                    ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                }
                break;
            case R.id.download_later_cancel_button:
                dismiss();
                transactions(getFragmentManager(), new PreDownloadMessageWindow());
                break;
        }
    }

    private void resetScheduler() {
        hourOfTheDaySpinner.setSelection(0);
        minutesSpinner.setSelection(0);
        amPmSpinner.setSelection(0);
    }

    private void enableScheduler(boolean enabled) {
        Utils.setEnabledR(mSchedulerBox, enabled);
        if (!enabled) {
            resetScheduler();
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView,
            boolean checked) {
        int resId = buttonView.getId();
        switch (resId) {
            case R.id.download_later_checkbox: {
                enableScheduler(!checked);
                break;
            }

            default:
              break;
        }

    }
}
