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
import android.view.KeyEvent;
import android.content.DialogInterface;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.notifications.DownloadFailNotification;
import com.verizon.dmclientupdate.OmadmFumoDialog;

public class DownloadUnsuccessfulNotice extends BaseFragment {

    static DownloadUnsuccessfulNotice sDownloadUnsuccessfulNotice = null;

    public static DownloadUnsuccessfulNotice getInstance() {
        if (sDownloadUnsuccessfulNotice == null) {
            sDownloadUnsuccessfulNotice = new DownloadUnsuccessfulNotice();
        }
        return sDownloadUnsuccessfulNotice;
    }

    public void createDialog() {
        sDownloadUnsuccessfulNotice.show(getInstance().getFragmentManager(), Utils.ACTION_DOWNLOAD_FAILED);
    }

    public void removeDialog() {
        sDownloadUnsuccessfulNotice.dismiss();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
        Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.download_unsuccessful_notice, container);

        Button downloadUnsuccessfulOkButton = (Button)view.findViewById(R.id.download_unsuccessful_ok_button);
        downloadUnsuccessfulOkButton.setOnClickListener(this);

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }
    @Override
    public void onStop() {
        if (mCloseAppWithNotificationFlag) {
            DownloadFailNotification.getInstance().show(getActivity());
        }
        super.onStop();
    }

    @Override
    public void onClick(View view) {
        mCloseAppWithNotificationFlag = false;
        DownloadFailNotification.getInstance().remove(getActivity());
        switch (view.getId()) {
            case R.id.download_unsuccessful_ok_button:
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
        }
    }
}
