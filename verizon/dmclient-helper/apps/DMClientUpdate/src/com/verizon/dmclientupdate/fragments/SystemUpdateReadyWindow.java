/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.FragmentManager;
import android.content.res.Resources;
import android.os.Bundle;
import android.view.KeyEvent;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;
import android.util.Log;

import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.notifications.*;

import java.util.Arrays;

public class SystemUpdateReadyWindow extends BaseFragment {

    private String resultUpdateReadyText;
    private static final String TAG = "SystemUpdateReadyWindow";
    final int one_minute_second = 60;
    final int one_minute = 1;
    final int first_element = 0;
    final int second_element = 1;
    private static SystemUpdateReadyWindow sSystemUpdateReadyWindow = null;
    public static SystemUpdateReadyWindow getInstance() {
        if (sSystemUpdateReadyWindow == null) {
            sSystemUpdateReadyWindow = new SystemUpdateReadyWindow();
        }
        return sSystemUpdateReadyWindow;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sSystemUpdateReadyWindow.show(fragmentManager, null);
    }

    public void removeDialog() {
        sSystemUpdateReadyWindow.dismiss();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.system_update_ready_window, container);
        Button systemUpdateReadyInstallNowButton = (Button)view.findViewById(R.id.system_update_ready_install_now_button);
        TextView updateReadyText = (TextView) view.findViewById(R.id.update_ready_text);
        systemUpdateReadyInstallNowButton.setOnClickListener(this);
        Resources res = getResources();
        try {
            if(guiMessageDescriptor.getmInstallParam() != null && !guiMessageDescriptor.getmInstallParam().isEmpty()) {
                String replaceString = guiMessageDescriptor.getmInstallParam().replaceAll("[^0-9]+", " ");
                int secondNumber = Integer.parseInt(Arrays.asList(replaceString.trim().split(" ")).get(second_element));
                int value = secondNumber/one_minute_second;
                if (secondNumber % one_minute_second > 30)
                {
                    value++;
                }
                resultUpdateReadyText = String.format(res.getString(R.string.system_update_ready_msg2), value);
                updateReadyText.setText(resultUpdateReadyText);
            } else {
                resultUpdateReadyText = String.format(res.getString(R.string.system_update_ready_msg2), one_minute);
                updateReadyText.setText(resultUpdateReadyText);
                Log.e(TAG, "cannot parse installParam!!!");
            }
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onStop() {
        if (mCloseAppWithNotificationFlag) {
            SystemUpdateReadyNotification.getInstance().show(getActivity());
        }
        super.onStop();
    }

    @Override
    public void onClick(View v) {
        mCloseAppWithNotificationFlag = false;
        SystemUpdateReadyNotification.getInstance().remove(getActivity());
        switch (v.getId()){
            case R.id.system_update_ready_install_now_button:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_OK);
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
        }
    }
}
