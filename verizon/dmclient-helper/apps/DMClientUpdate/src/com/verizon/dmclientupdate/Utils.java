/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.omadm.OmadmStateListener;
import android.os.Parcel;
import android.os.ParcelFileDescriptor;
import android.os.UserHandle;
import android.provider.Telephony;
import android.app.NotificationManager;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

import com.android.internal.telephony.WspTypeDecoder;

import android.omadm.OmadmManager;
import android.omadm.FumoGuiSystemUpdateInfo;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.App;
import com.verizon.dmclientupdate.UpdateInfo;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.notifications.*;

public class Utils extends Activity{

    private final static String TAG = "Utils";
    public static boolean DEBUG = true;

    private static Utils sUtils = null;
    private final Context mContext;

    private final OmadmManager mOmadmManager;
    final OmadmStateListener mOmadmStateListener;
    private final ConnectivityManager mConnectivityManager;
    protected GuiMessageDescriptor guiMessageDescriptor;
    private final static Object LOCK = new Object();
    private OmadmFumoDialog omadmFumoDialog;

    public static final int STATE_IDLE = 10;
    public static final int STATE_DOWNLOAD_FAILED = 20;
    public static final int STATE_DOWNLOAD_PROGRESSING = 30;
    public static final int STATE_DOWNLOAD_COMPLETE = 40;
    public static final int STATE_READY_TO_UPDATE = 50;
    public static final int STATE_UPDATE_PROGRESSING = 60;
    public static final int STATE_UPDATE_FAILED = 70;
    public static final int STATE_UPDATE_FAILED_NO_DATA = 80;
    public static final int STATE_UPDATE_COMPLITE_DATA = 90;
    public static final int STATE_UPDATE_COMPLETE_NO_DATA = 100;
    public static final int STATE_MEMORY_NOT_ENOUGH = 110;
    public static final int STATE_UPDATE_AVALIABLE = 120;
    public static final int STATE_LOW_BATTERY = 130;
    public static final int STATE_DOWNLOAD_OVER_WIFI = 140;
    public static final int STATE_WIFI_NOT_CONNECTED = 150;

    public static final String ACTION_IDLE = "ACTION_IDLE";
    public static final String ACTION_CLEAR = "ACTION_CLEAR";
    public static final String ACTION_MEMORY_NOT_ENOUGH = "ACTION_MEMORY_NOT_ENOUGH";
    public static final String ACTION_UPDATE_AVLIABLE = "ACTION_UPDATE_AVLIABLE";
    public static final String ACTION_UPDATE_AVALIABLE_NOTIFICATION = "ACTION_UPDATE_AVALIABLE_NOTIFICATION";
    public static final String ACTION_DOWNLOAD_FAILED = "ACTION_DOWNLOAD_FAILED";
    public static final String ACTION_DOWNLOAD_PROGRESSING = "ACTION_DOWNLOAD_PROGRESSING";
    public static final String ACTION_DOWNLOAD_COMPLETE = "ACTION_DOWNLOAD_COMPLETE";
    public static final String ACTION_READY_TO_UPDATE = "ACTION_READY_TO_UPDATE";
    public static final String ACTION_RESTART_PHONE = "ACTION_RESTART_PHONE";
    public static final String ACTION_UPDATE_PROGRESSING = "ACTION_UPDATE_PROGRESSING";
    public static final String ACTION_UPDATE_FAILED = "ACTION_UPDATE_FAILED";
    public static final String ACTION_UPDATE_COMPLITE = "ACTION_UPDATE_COMPLITE";
    public static final String ACTION_DOWNLOAD_OVER_WIFI = "ACTION_DOWNLOAD_OVER_WIFI";
    public static final String ACTION_LOW_BATTERY = "ACTION_LOW_BATTERY";
    public static final String ACTION_UPDATE_PROGRESS = "ACTION_UPDATE_PROGRESS";
    public static final String ACTION_DOWNLOAD_DONE = "ACTION_DOWNLOAD_DONE";
    public static final String ACTION_DOWNLOADING_WINDOW = "ACTION_DOWNLOADING_WINDOW";
    public static final String ACTION_OPEN_URL = "ACTION_OPEN_URL";
    public static final String ACTION_INSTALL_LATER = "ACTION_INSTALL_LATER";
    public static final String ACTION_NOT_CONNECTED_WIFI = "ACTION_NOT_CONNECTED_WIFI";
    public static final String REMOVE_DIALOG = "REMOVE_DIALOG";
    public static final String CREATE_DIALOG = "CREATE_DIALOG";
    public static final String STATE_CHANGED = "com.verizon.dmclientupdate.STATE_CHANGED";
    public static final String ACTION_OPEN_ACTIVITY = "ACTION_OPEN_ACTIVITY";
    public static final String EXTRAS = "EXTRAS";

    public static final String SOFTWARE_VERSION = "softwareVersion";
    public static final String UPDATE_STATUS = "updateStatus";
    public static final String DATA = "data";
    public static final String TIME = "time";
    public static final String URL = "url";
    public static final String TEXT = "text";
    public static final String FAILURE_ERROR = "failureError";
    public static final String SUCCESS = "Success";

    public static final int PERSENT_100 = 100;
    public static final int NOTIFY_DOWNLOADING = 101;
    public static final int NOTIFY_INSTALLING = 102;
    public static final int NOTIFY_LOW_BATTARY = 103;
    public static final int NOTIFY_MAKE_ROOM = 104;
    public static final int NOTIFY_RESTART_PHONE = 105;
    public static final String NULL = "null";
    public static final int URL_MIN_LENGTH = 1;

    public static final int EBT_OK = 0x0004;
    public static final int EBT_CANCEL = 0x0002;
    public static final int EBT_NO = 0x0008;
    public static final int EBT_IGNORE = 0x0010;
    public static final int EBT_LATER = 0x0020;
    public static final int EBT_CHECK_FOR_UPDATE = 0x0040;
    public static final int EBT_BACK = 0x0100;
    public static final int EBT_HOME = 0x0200;
    public static final int EBT_CHOOSE_FILES_TO_DELETE = 0x0040;
    public static final int EBT_CLOSE_WINDOW = 0x0080;
    public static final int EBT_NOTIFICATION = 0x2000;
    public static final int EBT_USER_DEFINED2 = 0x4000;
    public static final int EBT_USER_DEFINED3 = 0x8000;

    private static int sState = STATE_UPDATE_AVALIABLE;

    public static final String BROWSER = "com.verizon.dmbrowser.Browser";
    public static final String BROWSER_PACKAGE = "com.verizon.dmbrowser";

    private WapPushPackage0Builder mWapPushPackage0Builder = null;

    protected static int mPercent = 0;

    protected Utils(Context context) {
        mContext = context;
        mOmadmManager =
                (OmadmManager) mContext.getSystemService(Context.OMADM_SERVICE);
        mOmadmStateListener = new SystemUpdatesOmadmStateListener();
        mConnectivityManager =
                (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        mWapPushPackage0Builder = new WapPushPackage0Builder();
    }

    public static Utils getInstance(Context context) {
        synchronized (LOCK) {
            if (sUtils == null) {
                sUtils = new Utils(context);
                // TBD
            }
            return sUtils;
        }
    }

    public void stateChanged() {
        Intent intent = new Intent(STATE_CHANGED);
        mContext.sendBroadcast(intent);
    }

    public boolean isNetworkConnected() {
        NetworkInfo netInfo = mConnectivityManager.getActiveNetworkInfo();
        return netInfo != null && netInfo.isConnected();
    }

    public int getState() {
        synchronized (LOCK) {
            return sState;
        }
    }

    private void setState(int state) {
        synchronized (LOCK) {
            if (sState != state) {
                sState = state;
                stateChanged();
            }
        }
    }

    public int getProgress() {
        return mPercent;
    }
    public void setmProgress(int persent) {
        mPercent = persent;
    }

    public String getLastCheckForUpdatesTime() {
        // TBD
        return null;
    }

    public void omadmCheckForUpdates() {
        mOmadmManager.omadmCheckForUpdate();
    }

    /**
     * Start listening for omadm client activities.
     */
    public void registerListener() {
        mOmadmManager.addOmadmChangedListener(mOmadmStateListener);
    }

    /**
     * Stop listening for omadm client activities.
     */
    public void unregisterListener() {
        mOmadmManager.removeOmadmChangedListener(mOmadmStateListener);
    }

    class SystemUpdatesOmadmStateListener extends OmadmStateListener {
        public SystemUpdatesOmadmStateListener() {
            super();
        }

        @Override
        public void onOmadmFumoPluginSystemUpdateInfo(FumoGuiSystemUpdateInfo fupi) {
            if (DEBUG) Log.d(TAG, "onOmadmFumoPluginSystemUpdateInfo");
        }

        @Override
        public void onOmadmControllerRemoveUI(int type) {}
    }

    public void onOmadmFumoPluginDownloadProgressChanged(int percent) {
        if (DEBUG) Log.d(TAG, "onOmadmFumoPluginDownloadProgressChanged");
        guiMessageDescriptor = App.getGuiMessageDescriptor();
        mPercent = percent;
        if (percent < PERSENT_100) {
            Intent intent = new Intent(ACTION_UPDATE_PROGRESS);
            App.getContext().sendBroadcast(intent);
            switch (guiMessageDescriptor.getmState()) {
                case Utils.STATE_DOWNLOAD_PROGRESSING:
                    DownloadingNotification.getInstance().show(App.getContext());
                    Log.d(TAG, String.valueOf(percent) + " download percent");
                    break;
                case Utils.STATE_UPDATE_PROGRESSING:
                    InstallingNotification.getInstance().show(App.getContext());
                    Log.d(TAG, String.valueOf(percent) + " install percent");
                    break;
            }
        }
    }

    public void onOmadmFumoPluginRemoveUI(int state) {
        if (DEBUG) Log.d(TAG, "onOmadmFumoPluginRemoveUI");
        guiMessageDescriptor = App.getGuiMessageDescriptor();
        NotificationManager notificationManager = (NotificationManager) App.getContext().getSystemService(Context.NOTIFICATION_SERVICE);

        if(guiMessageDescriptor.mState == Utils.STATE_DOWNLOAD_PROGRESSING){
            notificationManager.cancel(Utils.NOTIFY_DOWNLOADING);
            Log.d(TAG, "download notification is deleted");
        } else if (guiMessageDescriptor.mState == Utils.STATE_UPDATE_PROGRESSING){
            notificationManager.cancel(Utils.NOTIFY_INSTALLING);
            Log.d(TAG, "install notification is deleted");
        } else if (guiMessageDescriptor.mState == Utils.STATE_LOW_BATTERY) {
            notificationManager.cancel(Utils.NOTIFY_LOW_BATTARY);
            Log.d(TAG, "low battary notification is deleted");
        } else if (guiMessageDescriptor.mState == Utils.STATE_UPDATE_AVALIABLE) {
            notificationManager.cancel(Utils.NOTIFY_MAKE_ROOM);
            Log.d(TAG, "update avaliable notification is deleted");
        } else if (guiMessageDescriptor.mState == Utils.STATE_MEMORY_NOT_ENOUGH) {
            notificationManager.cancel(Utils.NOTIFY_MAKE_ROOM);
            Log.d(TAG, "memory not enough notification is deleted");
        } else if (guiMessageDescriptor.mState == Utils.STATE_READY_TO_UPDATE) {
            notificationManager.cancel(Utils.NOTIFY_RESTART_PHONE);
            Log.d(TAG, "restart phone notification is deleted");
        }

        mPercent = 0;
        if (OmadmFumoDialog.sActivity != null) {
            OmadmFumoDialog.sActivity.finishAndRemoveTask();
        }
    }

    public void sendUserReplyToOmadmController(int type, String input_text, int selected_choices_count,
        String selected_choices, int button_id) {
        mOmadmManager.omadmControllerDispachUserReply(type, input_text, selected_choices_count,selected_choices, button_id);
    }

    public void checkForUpdates() {
        mOmadmManager.omadmCheckForUpdate();
    }

    public void sendUserReplyToOmadmFumoPlugin(
        int state, long defered_update_time, boolean wifi_requred,
        boolean automatic_update_enable, int button_id) {
        if (DEBUG) Log.d(TAG, "sendUserReplyToOmadmFumoPlugin");
        mOmadmManager.omadmFumoPluginDispachUserReply(
            state, defered_update_time, wifi_requred,
            automatic_update_enable, button_id);
    }

    public static void setEnabledR(View view, boolean enabled) {
        if (null != view) {
            view.setEnabled(enabled);
            if (view instanceof ViewGroup) {
                int childCount = ((ViewGroup)view).getChildCount();
                for (int idx = 0; idx < childCount; idx ++) {
                    setEnabledR(((ViewGroup)view).getChildAt(idx), enabled);
                }
            }
        }
    }

    public ParcelFileDescriptor inputStream(String path) {
        if (mOmadmManager != null) {
            return mOmadmManager.inputStream(path);
        }
        return null;
    }
}
