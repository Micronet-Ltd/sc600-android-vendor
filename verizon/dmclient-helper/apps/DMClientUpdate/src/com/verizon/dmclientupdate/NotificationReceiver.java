/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import com.verizon.dmclientupdate.notifications.*;

public class NotificationReceiver  extends BroadcastReceiver {

    protected GuiMessageDescriptor guiMessageDescriptor;
    protected static final String TAG = "NotificationReceiver";
    protected Utils mUtils;
    public NotificationReceiver() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        guiMessageDescriptor = App.getGuiMessageDescriptor();
        mUtils = Utils.getInstance(App.getContext());
        Log.d(TAG, String.valueOf(guiMessageDescriptor.getmState()) + " " + action);
        switch (action) {
            case Utils.ACTION_OPEN_ACTIVITY:
                Intent dialog_intent = new Intent(context, OmadmFumoDialog.class);
                dialog_intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(dialog_intent);
                break;
            case Utils.ACTION_UPDATE_AVALIABLE_NOTIFICATION:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_NOTIFICATION);
                break;
            case Utils.ACTION_LOW_BATTERY:
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_OK);
                break;
        }
    }
}
