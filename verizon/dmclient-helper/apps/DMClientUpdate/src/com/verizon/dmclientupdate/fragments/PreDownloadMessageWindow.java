/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;
import android.content.ComponentName;
import android.app.FragmentManager;
import android.content.DialogInterface;
import android.view.KeyEvent;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.notifications.*;

public class PreDownloadMessageWindow extends BaseFragment {

    private static final String TAG = "PreDownloadMessageWindow";
    DownloadingNotification downloadingNotification;
    private static PreDownloadMessageWindow sPreDownloadMessageWindow = null;
    public static PreDownloadMessageWindow getInstance() {
        if (sPreDownloadMessageWindow == null) {
            sPreDownloadMessageWindow = new PreDownloadMessageWindow();
        }
        return sPreDownloadMessageWindow;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sPreDownloadMessageWindow.show(fragmentManager, null);
    }

    public void removeDialog() {
        sPreDownloadMessageWindow.dismiss();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.pre_download_message_window, container);

        Button yesButton = (Button)view.findViewById(R.id.yes_button);
        Button noButton = (Button)view.findViewById(R.id.no_button);
        Button leterButton = (Button)view.findViewById(R.id.later_button);
        TextView learnMoreOnlineButton = (TextView)view.findViewById(R.id.learn_more_online_button);
        TextView configurableText = (TextView)view.findViewById(R.id.configurable_text);

        if(guiMessageDescriptor.getmHyperLink().length() <= Utils.URL_MIN_LENGTH) {
            learnMoreOnlineButton.setVisibility(View.GONE);
        } else {
            learnMoreOnlineButton.setVisibility(View.VISIBLE);
            learnMoreOnlineButton.setOnClickListener(this);
        }

        configurableText.setText(guiMessageDescriptor.getmMessageText());

        yesButton.setOnClickListener(this);
        noButton.setOnClickListener(this);
        leterButton.setOnClickListener(this);

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onStop() {
        if (mCloseAppWithNotificationFlag) {
            SystemAvaliableNotification.getInstance().show(getActivity());
        }
        super.onStop();
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        SystemAvaliableNotification.getInstance().remove(getActivity());
        switch (v.getId()){
            case R.id.yes_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_OK);
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
            case R.id.no_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_NO);
                dismiss();
                transactions(getFragmentManager(), new SystemUpdateCanceledWindow());
                break;
            case R.id.later_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_LATER);
                dismiss();
                transactions(getFragmentManager(), new DownloadLaterWindow());
                break;
            case R.id.learn_more_online_button:
                Intent intent = new Intent();
                intent.setComponent(new ComponentName(Utils.BROWSER_PACKAGE, Utils.BROWSER));
                intent.setData(Uri.parse(guiMessageDescriptor.getmHyperLink()));
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                startActivity(intent);
                dismiss();
                break;
        }
    }
}
