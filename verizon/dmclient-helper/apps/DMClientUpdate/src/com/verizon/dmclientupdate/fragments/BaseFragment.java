/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.DialogFragment;
import android.app.FragmentManager;
import android.view.View;

import com.verizon.dmclientupdate.UpdateInfo;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.GuiMessageDescriptor;
import com.verizon.dmclientupdate.App;

public abstract class BaseFragment extends DialogFragment implements View.OnClickListener {

    protected GuiMessageDescriptor guiMessageDescriptor = App.getGuiMessageDescriptor();
    protected UpdateInfo updateInfo = App.getUpdateInfo();
    protected Utils mUtils = Utils.getInstance(getActivity());
    protected boolean mCloseAppWithNotificationFlag = true;

    @Override
    public void onStop() {
        super.onStop();
        if (mCloseAppWithNotificationFlag) {
            ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
        }
    }

    public void transactions(FragmentManager fragmentManager, DialogFragment fragment) {
        fragment.show(fragmentManager, null);
    }
}
