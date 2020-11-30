/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.app.FragmentManager;
import android.view.View;
import android.view.ViewGroup;
import android.util.Log;
import android.view.Window;
import android.content.ComponentName;
import android.widget.Button;
import android.widget.TextView;
import java.util.Arrays;
import android.content.res.Resources;
import android.content.DialogInterface;
import android.view.KeyEvent;
import android.os.Build;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.notifications.*;

public class PostDownloadMessageWindow extends BaseFragment {

    private static final String TAG = "PostDownloadMessageWindow";
    final int one_minute_second = 60;
    final int one_minute = 1;
    final int first_element = 0;
    final int second_element = 1;

    private static PostDownloadMessageWindow sPostDownloadMessageWindow = null;
    public static PostDownloadMessageWindow getInstance() {
        if (sPostDownloadMessageWindow == null) {
            sPostDownloadMessageWindow = new PostDownloadMessageWindow();
        }
        return sPostDownloadMessageWindow;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sPostDownloadMessageWindow.show(fragmentManager, Utils.ACTION_DOWNLOAD_COMPLETE);
    }

    public void removeDialog() {
        sPostDownloadMessageWindow.dismiss();
        ((OmadmFumoDialog) getActivity()).finish();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.post_download_message_window, container);
        Button postDownloadInstallNowButton = (Button)view.findViewById(R.id.post_download_install_now_button);
        Button postDownloadInstallLaterButton = (Button)view.findViewById(R.id.post_download_install_later_button);
        Button postDownloadInstallNoButton = (Button)view.findViewById(R.id.post_download_install_no_button);
        TextView learnMoreOnlineButton = (TextView)view.findViewById(R.id.learn_more_online_button);
        TextView configurableText = (TextView)view.findViewById(R.id.configurable_text);
        TextView postDownloadWarning = (TextView) view.findViewById(R.id.post_download_warning);
        configurableText.setText(guiMessageDescriptor.getmMessageText());
        postDownloadInstallNowButton.setOnClickListener(this);
        postDownloadInstallLaterButton.setOnClickListener(this);
        postDownloadInstallNoButton.setOnClickListener(this);

        if(guiMessageDescriptor.getmHyperLink().length() <= Utils.URL_MIN_LENGTH) {
            learnMoreOnlineButton.setVisibility(View.GONE);
            Log.i(TAG, "Link is empty");
        } else {
            Log.i(TAG, "Link is not empty");
            learnMoreOnlineButton.setVisibility(View.VISIBLE);
            learnMoreOnlineButton.setOnClickListener(this);
        }

        try{
        //    if(Integer.parseInt(guiMessageDescriptor.getmSeverity()) == 2) {
            postDownloadInstallNoButton.setVisibility(View.GONE);
			postDownloadWarning.setVisibility(View.GONE);
        //    Log.i(TAG, "Severity is mandatory");
        //    }
        }catch(NumberFormatException e) {
            e.printStackTrace();
            Log.e(TAG,"parseInt(guiMessageDescriptor.getmSeverity()) is null");
        }

        //postDownloadWarning.setText(String.format(getActivity().getResources().getString(R.string.post_background_download_warning), Build.MODEL));
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        getDialog().setCanceledOnTouchOutside(true);
        return view;
    }

    @Override
    public void onStop() {
        if (mCloseAppWithNotificationFlag) {
            //SystemAvaliableNotification.getInstance().show(getActivity());
            SystemUpdateReadyNotification.getInstance().show(getActivity());  //after post download, it show install screen, click install,device will reboot to install
        }
        super.onStop();
    }

    String numberOfMinutes() {
        Resources res = getResources();
        String resultPostDownloadWarning = null;
        try {
            if(guiMessageDescriptor.getmInstallParam() != null && !guiMessageDescriptor.getmInstallParam().isEmpty()) {
                String replaceString = guiMessageDescriptor.getmInstallParam().replaceAll("[^0-9]+", " ");
                int firstNumber = Integer.parseInt(Arrays.asList(replaceString.trim().split(" ")).get(first_element));
                int value = firstNumber/one_minute_second;
                if (firstNumber % one_minute_second > 30)
                {
                    value++;
                }
                resultPostDownloadWarning = String.format(res.getString(R.string.post_download_warning), value);
            } else {
                resultPostDownloadWarning = "0";
            }
        }catch(NumberFormatException e){e.printStackTrace();}
        return resultPostDownloadWarning;
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        SystemUpdateReadyNotification.getInstance().remove(getActivity());
        switch (v.getId()) {
            case R.id.post_download_install_now_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_OK);
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
            case R.id.post_download_install_later_button:
                dismiss();
                transactions(getFragmentManager(), new InstallLaterAutoWindow());
                break;
             case R.id.post_download_install_no_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_CANCEL);
                dismiss();
                transactions(getFragmentManager(), new SystemUpdateCanceledWindow());
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
