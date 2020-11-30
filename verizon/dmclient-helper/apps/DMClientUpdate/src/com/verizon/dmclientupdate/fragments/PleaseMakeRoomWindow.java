/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.view.LayoutInflater;
import android.view.View;
import android.util.Log;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;
import android.app.FragmentManager;
import android.content.DialogInterface;
import android.view.KeyEvent;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.*;
import com.verizon.dmclientupdate.notifications.*;

public class PleaseMakeRoomWindow extends BaseFragment {
    private final long mGb = 1024*1024*1024;
    protected SystemAvaliableNotification systemAvaliableNotification;
    private long memory;
    private static final String TAG = "PleaseMakeRoomWindow";
    private static PleaseMakeRoomWindow sPleaseMakeRoomWindow = null;
    public static PleaseMakeRoomWindow getInstance() {
        if (sPleaseMakeRoomWindow == null) {
            sPleaseMakeRoomWindow = new PleaseMakeRoomWindow();
        }
        return sPleaseMakeRoomWindow;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sPleaseMakeRoomWindow.show(fragmentManager, null);
    }

    public void removeDialog() {
        sPleaseMakeRoomWindow.dismiss();
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.please_make_room_window, container);
        Button memoryNotNowButton = (Button)view.findViewById(R.id.memory_not_now_button);
        Button chooseFilesToDeleteButton = (Button)view.findViewById(R.id.choose_files_to_delete_button);
        TextView memoryConfigurableText = (TextView)view.findViewById(R.id.memory_configurable_text);
        TextView memoryConfigurableText2 = (TextView)view.findViewById(R.id.memory_configurable_text2);
        float required_space_for_update = (float)guiMessageDescriptor.getRequiredSpaceForUpdate()/mGb;
        float required_space_for_delete = (float)guiMessageDescriptor.getRequiredSpaceForDelete()/mGb;
        String configurableText = String.format(getString(R.string.memory_msg), required_space_for_update);
        memoryConfigurableText.setText(configurableText);
        String configurableText2 = String.format(getString(R.string.memory_msg2), required_space_for_delete);
        memoryConfigurableText2.setText(configurableText2);
        memoryNotNowButton.setOnClickListener(this);
        chooseFilesToDeleteButton.setOnClickListener(this);
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
        switch (v.getId()) {
            case R.id.memory_not_now_button:
                SystemAvaliableNotification.getInstance().show(getActivity());
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_NO);
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
            case R.id.choose_files_to_delete_button:
                startActivity(new Intent(Settings.ACTION_MEMORY_CARD_SETTINGS));
                dismiss();
                break;
        }
    }
}
