/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.app.NotificationManager;
import android.os.SystemClock;
import android.content.IntentFilter;
import android.omadm.FumoGuiMessageDescriptor;
import android.omadm.OmadmManager;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.fragments.*;
import com.verizon.dmclientupdate.notifications.*;

public class OmadmNotificationReceiver extends BroadcastReceiver {
    private static final String TAG = "OmadmNotificationReceiver";
    private static final boolean DEBUG = true;
    private Context mContext;
    protected GuiMessageDescriptor guiMessageDescriptor;
    protected DownloadingWindow downloadingWindow;
    protected Utils mUtils = Utils.getInstance(App.getContext());

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        String action = intent.getAction();
        MyTimer.getInstance().stopTimerTask();
        if (DEBUG) Log.d(TAG, "Received action Intent: " + intent);

        if (OmadmManager.ACTION_OMADM_CONTROLLER_SHOW_UI.equals(action)) {
            showOmadmControllerUI(intent);
        } else if (OmadmManager.ACTION_OMADM_FUMO_SHOW_UI.equals(action)) {
            showOmadmFumoUI(intent);
        } else if (OmadmManager.ACTION_OMADM_FUMO_SHOW_PROGRESS.equals(action)) {
            updateOmadmFumoProgress(intent);
        } else if (OmadmManager.ACTION_OMADM_FUMO_REMOVE_UI.equals(action)) {
            removeOmadmFumoUI(intent);
        }
    }

    private void showOmadmControllerUI(Intent intent) {
        Bundle extras = intent.getExtras();
        if (extras == null) {
            Log.e(TAG, "received ACTION_OMADM_CONTROLLER_SHOW_UI with no extras");
            return;
        }
        Intent dialog_intent = new Intent(mContext, OmadmControllerDialog.class);
        dialog_intent.putExtras(extras);
        dialog_intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT
            | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(dialog_intent);
    }

    private void showOmadmFumoUI(Intent intent) {
        Bundle extras = intent.getExtras();
        if (extras == null) {
            Log.e(TAG, "received ACTION_OMADM_FUMO_SHOW_UI with no extras");
            return;
        }
        FumoGuiMessageDescriptor fgmd = (FumoGuiMessageDescriptor) extras.get(
            OmadmManager.FUMO_MESSAGE_DESCRIPTOR);
        Log.d(TAG, String.valueOf(fgmd.mState) + " mState");
        Log.d(TAG, String.valueOf(fgmd.mInstallParam) + " mInstallParam");
        if (fgmd == null) {
            Log.e(TAG, "received ACTION_OMADM_FUMO_SHOW_UI with no message descriptor extra");
            return;
        }

        if(UpdateNotAvaliableFragment.getInstance().isVisible()) {
            UpdateNotAvaliableFragment.getInstance().dismiss();
        }

        if (SystemUpdateCompleteNotification.getInstance() != null)
        {
            SystemUpdateCompleteNotification.getInstance().remove(App.getContext());
        }
        App.cleanGuiMessageDescriptor();

        NotificationManager notificationManager = (NotificationManager) App.getContext().getSystemService(Context.NOTIFICATION_SERVICE);
        App.saveGuiMessageDescriptor(new GuiMessageDescriptor(fgmd));
        Intent dialog_intent = new Intent(mContext, OmadmFumoDialog.class);
        dialog_intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
        if(fgmd.mState == Utils.STATE_DOWNLOAD_PROGRESSING){
            DownloadingNotification.getInstance().show(App.getContext());
        }else if(fgmd.mState == Utils.STATE_UPDATE_PROGRESSING){
            InstallingNotification.getInstance().show(App.getContext());
        }else if(fgmd.mState == Utils.STATE_LOW_BATTERY){
            LowBatteryNotification.getInstance().show(App.getContext());
        }else if(fgmd.mState == Utils.STATE_READY_TO_UPDATE){
            InstallingNotification.getInstance().remove(App.getContext());
            mContext.startActivity(dialog_intent);
        }else if(fgmd.mState == Utils.STATE_DOWNLOAD_FAILED || fgmd.mState == Utils.STATE_DOWNLOAD_COMPLETE){
            notificationManager.cancel(Utils.NOTIFY_DOWNLOADING);
            mContext.startActivity(dialog_intent);
        }else if(fgmd.mState == Utils.STATE_UPDATE_FAILED || fgmd.mState == Utils.STATE_UPDATE_FAILED_NO_DATA
            || fgmd.mState == Utils.STATE_UPDATE_COMPLITE_DATA || fgmd.mState == Utils.STATE_UPDATE_COMPLETE_NO_DATA){
            notificationManager.cancel(Utils.NOTIFY_INSTALLING);
            mContext.startActivity(dialog_intent);
        }else{
            mContext.startActivity(dialog_intent);
        }
    }

    private void updateOmadmFumoProgress(Intent intent) {
        Bundle extras = intent.getExtras();
        if (extras == null) {
            Log.e(TAG, "received ACTION_OMADM_FUMO_SHOW_PROGRESS with no extras");
            return;
        }
        mUtils.onOmadmFumoPluginDownloadProgressChanged((int) extras.get(OmadmManager.FUMO_PROGRESS_PERCENT));
    }

    private void removeOmadmFumoUI(Intent intent) {
        Bundle extras = intent.getExtras();
        if (extras == null) {
            Log.e(TAG, "received ACTION_OMADM_FUMO_REMOVE_UI with no extras");
            return;
        }
        mUtils.onOmadmFumoPluginRemoveUI((int) extras.get(OmadmManager.FUMO_REMOVE_UI_STATE));
    }
}
