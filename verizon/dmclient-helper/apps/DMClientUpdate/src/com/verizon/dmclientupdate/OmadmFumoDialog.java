/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.app.Activity;
import android.app.FragmentManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.view.MotionEvent;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;
import android.omadm.FumoGuiMessageDescriptor;
import android.omadm.OmadmManager;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.fragments.*;
import com.verizon.dmclientupdate.notifications.DownloadingNotification;
import com.verizon.dmclientupdate.notifications.InstallingNotification;
import com.verizon.dmclientupdate.notifications.LowBatteryNotification;
import com.verizon.dmclientupdate.notifications.SystemAvaliableNotification;
import com.verizon.dmclientupdate.notifications.SystemUpdateReadyNotification;

public class OmadmFumoDialog extends Activity {

    protected GuiMessageDescriptor guiMessageDescriptor;
    protected Utils mUtils;
    protected FragmentManager fm;
    protected static final String TAG = "OmadmFumoDialog";
    protected SystemUpdateUnsuccessfullNotice systemUpdateUnsuccessfullNotice;
    protected SystemUpdateCompleteWindow systemUpdateCompleteWindow;
    protected DownloadUnsuccessfulNotice downloadUnsuccessfulNotice;
    protected PostDownloadMessageWindow postDownloadMessageWindow;
    protected SystemUpdateReadyWindow systemUpdateReadyWindow;
    protected PleaseMakeRoomWindow pleaseMakeRoomWindow;
    protected PreDownloadMessageWindow preDownloadMessageWindow;
    protected DownloadingWindow downloadingWindow;
    protected DownloadOverWiFiWindow downloadOverWiFiWindow;
    protected NotConnectedWiFiNotice notConnectedWiFiNotice;
    protected InstallLaterAutoWindow installLaterAutoWindow;
    protected static Activity sActivity;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.omadm_fumo_dialog);
        fm = getFragmentManager();
        sActivity = this;
    }

    @Override
    public void onResume() {
        super.onResume();
        guiMessageDescriptor = App.getGuiMessageDescriptor();
        mUtils = Utils.getInstance(getApplicationContext());
        mUtils.registerListener();
        Log.d(TAG, "guiMessageDescriptor.getmState()" + guiMessageDescriptor.getmState());
        updateView(guiMessageDescriptor.getmState());
    }

    @Override
    protected void onStop() {
        super.onStop();
        mUtils.unregisterListener();
    }

    protected void updateView(int state) {
        switch (state) {
            case Utils.STATE_IDLE:
                break;
            case Utils.STATE_DOWNLOAD_FAILED:
                downloadUnsuccessfulNotice = new DownloadUnsuccessfulNotice();
                downloadUnsuccessfulNotice.show(fm, Utils.ACTION_DOWNLOAD_FAILED);
                break;
            case Utils.STATE_DOWNLOAD_PROGRESSING:
                downloadingWindow = new DownloadingWindow();
                downloadingWindow.show(fm, Utils.ACTION_DOWNLOADING_WINDOW);
                break;
            case Utils.STATE_DOWNLOAD_COMPLETE:
                postDownloadMessageWindow = new PostDownloadMessageWindow();
                postDownloadMessageWindow.show(fm, Utils.ACTION_DOWNLOAD_COMPLETE);
                break;
            case Utils.STATE_READY_TO_UPDATE:
                systemUpdateReadyWindow = new SystemUpdateReadyWindow();
                systemUpdateReadyWindow.show(fm, Utils.ACTION_READY_TO_UPDATE);
                break;
            case Utils.STATE_UPDATE_PROGRESSING:
                break;
            case Utils.STATE_UPDATE_FAILED:
                systemUpdateUnsuccessfullNotice = SystemUpdateUnsuccessfullNotice.getInstance();
                systemUpdateUnsuccessfullNotice.show(fm, Utils.ACTION_UPDATE_FAILED);
                break;
            case Utils.STATE_UPDATE_FAILED_NO_DATA:
                systemUpdateUnsuccessfullNotice = SystemUpdateUnsuccessfullNotice.getInstance();
                systemUpdateUnsuccessfullNotice.show(fm, Utils.ACTION_UPDATE_FAILED);
                break;
            case Utils.STATE_UPDATE_COMPLITE_DATA:
                systemUpdateCompleteWindow = new SystemUpdateCompleteWindow();
                systemUpdateCompleteWindow.show(fm, Utils.ACTION_UPDATE_COMPLITE);
                break;
            case Utils.STATE_UPDATE_COMPLETE_NO_DATA:
                systemUpdateCompleteWindow = new SystemUpdateCompleteWindow();
                systemUpdateCompleteWindow.show(fm, Utils.ACTION_UPDATE_COMPLITE);
                break;
            case Utils.STATE_MEMORY_NOT_ENOUGH:
                pleaseMakeRoomWindow = new PleaseMakeRoomWindow();
                pleaseMakeRoomWindow.show(fm, Utils.ACTION_MEMORY_NOT_ENOUGH);
                break;
            case Utils.STATE_UPDATE_AVALIABLE:
                preDownloadMessageWindow = new PreDownloadMessageWindow();
                preDownloadMessageWindow.show(fm, Utils.ACTION_UPDATE_AVLIABLE);
                break;
            case Utils.STATE_LOW_BATTERY:
                LowBatteryNotification.getInstance().show(App.getContext());
                break;
            case Utils.STATE_DOWNLOAD_OVER_WIFI:
                downloadOverWiFiWindow = new DownloadOverWiFiWindow();
                downloadOverWiFiWindow.show(fm, Utils.ACTION_DOWNLOAD_OVER_WIFI);
                break;
            case Utils.STATE_WIFI_NOT_CONNECTED:
                notConnectedWiFiNotice = new NotConnectedWiFiNotice();
                notConnectedWiFiNotice.show(fm, Utils.ACTION_NOT_CONNECTED_WIFI);
                break;
        }
    }
}
