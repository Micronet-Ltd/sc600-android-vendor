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
import com.verizon.dmclientupdate.notifications.*;
import com.verizon.dmclientupdate.OmadmFumoDialog;

public class DownloadOverWiFiWindow extends BaseFragment {

    private static final String TAG = "DownloadOverWiFiWindow";
    private static DownloadOverWiFiWindow sDownloadOverWiFiWindow = null;
    public static DownloadOverWiFiWindow getInstance() {
        if (sDownloadOverWiFiWindow == null) {
            sDownloadOverWiFiWindow = new DownloadOverWiFiWindow();
        }
        return sDownloadOverWiFiWindow;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sDownloadOverWiFiWindow.show(fragmentManager, null);
    }

    public void removeDialog() {
        sDownloadOverWiFiWindow.dismiss();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.download_over_wifi_window, container);

        Button downloadOverWifiNoThanksButton = (Button) view.findViewById(R.id.download_over_wifi_no_thanks_button);
        Button downloadOverWifiOkButton = (Button) view.findViewById(R.id.download_over_wifi_ok_button);
        TextView learnMoreOnlineButton = (TextView) view.findViewById(R.id.learn_more_online_button);

         TextView configurableText = (TextView) view.findViewById(R.id.configurable_text);

        downloadOverWifiNoThanksButton.setOnClickListener(this);
        downloadOverWifiOkButton.setOnClickListener(this);


        configurableText.setText(guiMessageDescriptor.getmMessageText());

        if(guiMessageDescriptor.getmHyperLink().length() <= Utils.URL_MIN_LENGTH) {
            learnMoreOnlineButton.setVisibility(View.GONE);
        } else {
            learnMoreOnlineButton.setVisibility(View.VISIBLE);
            learnMoreOnlineButton.setOnClickListener(this);
        }

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
    public void onClick(View view) {
        mCloseAppWithNotificationFlag = false;
        SystemAvaliableNotification.getInstance().remove(getActivity());
        switch (view.getId()) {
            case R.id.download_over_wifi_no_thanks_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_CANCEL);
                dismiss();
                transactions(getFragmentManager(), new SystemUpdateCanceledWindow());
                break;

            case R.id.download_over_wifi_ok_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_OK);
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
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
