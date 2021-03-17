/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.util.Log;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.App;
import com.verizon.dmclientupdate.notifications.DownloadingNotification;

public class DownloadingWindow extends BaseFragment {

    protected ProgressBar progressBar;
    protected NotificationReceiver receiver;
    protected static final String TAG = "DownloadingWindow";
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.downloading_window, container);
        Button okButton = (Button)view.findViewById(R.id.ok_button);
        progressBar = (ProgressBar)view.findViewById(R.id.progress);
        final TextView configurableText = (TextView) view.findViewById(R.id.configurable_text);

        okButton.setOnClickListener(this);
        progressBar.setProgress(mUtils.getProgress());
        configurableText.setText(guiMessageDescriptor.getmMessageText());
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    public class NotificationReceiver extends BroadcastReceiver{
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            switch (action){
                case Utils.ACTION_UPDATE_PROGRESS:
                    progressBar.setProgress(mUtils.getProgress());
                    break;
                case Utils.ACTION_DOWNLOAD_DONE:
                    dismiss();
                    break;
            }
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        receiver = new NotificationReceiver();
        final IntentFilter notificationFilter = new IntentFilter();
        final IntentFilter downloadDoneFilter = new IntentFilter();
        notificationFilter.addAction(Utils.ACTION_UPDATE_PROGRESS);
        downloadDoneFilter.addAction(Utils.ACTION_DOWNLOAD_DONE);
        getActivity().registerReceiver(receiver, notificationFilter);
        getActivity().registerReceiver(receiver, downloadDoneFilter);
    }

    @Override
    public void onStop() {
        if(receiver != null){
            getActivity().unregisterReceiver(receiver);
            receiver = null;
        }
        super.onStop();
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
