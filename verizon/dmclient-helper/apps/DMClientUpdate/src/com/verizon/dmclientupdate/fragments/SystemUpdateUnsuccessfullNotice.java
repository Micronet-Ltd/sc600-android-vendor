/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.FragmentManager;
import android.content.res.Resources;
import android.os.Build;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.KeyEvent;
import android.content.DialogInterface;
import android.widget.Button;
import android.widget.TextView;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import android.util.Log;
import java.io.File;
import java.io.FileInputStream;
import com.verizon.dmclientupdate.notifications.SystemUpdateFailNotification;
public class SystemUpdateUnsuccessfullNotice extends BaseFragment {
	static String update_fail_flag="/data/vendor/verizon/dmclient/data/update_fail_flag_count";
	private static int fail_count = 0;

    private static SystemUpdateUnsuccessfullNotice sSystemUpdateUnsuccessfullNotice = null;
    public static SystemUpdateUnsuccessfullNotice getInstance() {
        if (sSystemUpdateUnsuccessfullNotice == null) {
			fail_count = getFailCount();
			Log.e("SystemUpdateUnsuccessfullNotice", "getInstance");
        }
        sSystemUpdateUnsuccessfullNotice = new SystemUpdateUnsuccessfullNotice();
        return sSystemUpdateUnsuccessfullNotice;
    }

    public void createDialog(FragmentManager fragmentManager) {
        sSystemUpdateUnsuccessfullNotice.show(fragmentManager, null);
    }

    public void removeDialog() {
        sSystemUpdateUnsuccessfullNotice.dismiss();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.system_update_unsuccessful_notice, container);
        Resources res = getResources();
        Button systemUpdateUnsuccessfulOkButton = (Button)view.findViewById(R.id.system_update_unsuccessful_ok_button);
        TextView configurableText = (TextView)view.findViewById(R.id.system_update_unsuccessful_msg);
        systemUpdateUnsuccessfulOkButton.setOnClickListener(this);
        String updateString ;
        if(fail_count > 0){
			updateString= String.format(res.getString(R.string.system_update_unsuccessful_msg2),
        	Build.MANUFACTURER, Build.MODEL, Build.MANUFACTURER, Build.MODEL, Build.MANUFACTURER, Build.MODEL);
        }else {
        	updateString = String.format(res.getString(R.string.system_update_unsuccessful_msg), Build.MANUFACTURER, Build.MODEL);
		}
         
        configurableText.setText(updateString);
        getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        return view;
    }

    @Override
    public void onStop() {
        if (mCloseAppWithNotificationFlag) {
            SystemUpdateFailNotification.getInstance().show(getActivity());
        }
        super.onStop();
    }
    
    @Override
    public void onClick(View view) {
        mCloseAppWithNotificationFlag = false;
        SystemUpdateFailNotification.getInstance().remove(getActivity());
        switch (view.getId()) {
            case R.id.system_update_unsuccessful_ok_button:
                dismiss();
                ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
                break;
        }
    }

    public static int getFailCount() {
    	try {
        	File file=new File(update_fail_flag);
			if (!file.exists())
			{
				file.createNewFile();
				return 0;
			}

			Log.e("unsuccess", "unsuccess full getFailCount");
		} catch (Exception e) {
			e.printStackTrace();
			// TODO: handle exception
		}
		return 1;
	}
}

