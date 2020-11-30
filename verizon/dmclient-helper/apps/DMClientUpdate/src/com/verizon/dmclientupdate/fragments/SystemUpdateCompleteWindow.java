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
import android.app.FragmentManager;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.view.KeyEvent;
import android.os.Build;
import java.io.File;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.notifications.SystemUpdateCompleteNotification;

public class SystemUpdateCompleteWindow extends BaseFragment {

    private static final String TAG = "SystemUpdateCompleteWindow";
	String flag = "/cache/update_flag_complete_ui";
	String update_fail_flag="/data/vendor/verizon/dmclient/data/update_fail_flag_count";
    private static SystemUpdateCompleteWindow sSystemUpdateCompleteWindow = null;
    public static SystemUpdateCompleteWindow getInstance() {
        if (sSystemUpdateCompleteWindow == null) {
            sSystemUpdateCompleteWindow = new SystemUpdateCompleteWindow();
        }
        return sSystemUpdateCompleteWindow;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sSystemUpdateCompleteWindow.show(fragmentManager, null);
    }

    public void removeDialog() {
        sSystemUpdateCompleteWindow.dismiss();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.system_update_complete_window, container);
        Button systemUpdateCompleteOkButton = (Button)view.findViewById(R.id.system_update_complete_ok_button);
        TextView learnMoreOnlineButton = (TextView)view.findViewById(R.id.learn_more_online_button);
        TextView configurableText = (TextView)view.findViewById(R.id.configurable_text);

        systemUpdateCompleteOkButton.setOnClickListener(this);

        if(guiMessageDescriptor.getmHyperLink().length() <= Utils.URL_MIN_LENGTH) {
            learnMoreOnlineButton.setVisibility(View.GONE);
        } else {
            learnMoreOnlineButton.setVisibility(View.VISIBLE);
            learnMoreOnlineButton.setOnClickListener(this);
        }

        //String resultConfigurableText = String.format("%s\n\n%s %s", guiMessageDescriptor.getmMessageText(), Build.MANUFACTURER, Build.MODEL);
        String resultConfigurableText = String.format("%s", guiMessageDescriptor.getmMessageText());
        configurableText.setText(resultConfigurableText);
        File file=new File(flag);
        File update_fail_file = new File(update_fail_flag);
        try {
            boolean exist = file.exists();
            if (!exist)
            	file.createNewFile();
            exist = update_fail_file.exists();
            if (exist)
                update_fail_file.delete();
						
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}

        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onStop() {
        if (mCloseAppWithNotificationFlag) {
            SystemUpdateCompleteNotification.getInstance().show(getActivity());
        }
        super.onStop();
    }

    @Override
    public void onClick(View view) {
        mCloseAppWithNotificationFlag = false;
        SystemUpdateCompleteNotification.getInstance().remove(getActivity());
        switch (view.getId()) {
            case R.id.system_update_complete_ok_button:
                File file=new File(flag);
                file.delete();
                android.util.Log.i("test","remove flag");
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
