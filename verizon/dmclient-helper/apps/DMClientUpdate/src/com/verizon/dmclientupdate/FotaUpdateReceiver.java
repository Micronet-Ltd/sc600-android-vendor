package com.verizon.dmclientupdate;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.RecoverySystem;
import android.util.Log;

import java.io.File;

public class FotaUpdateReceiver extends BroadcastReceiver {
    private static final boolean DEBUG = true;
    private static final String TAG = "FotaUpdateReceiver";
    private Context mContext;

    @Override
    public void onReceive(Context context, Intent intent) {
        this.mContext = context;
		String path  = intent.getStringExtra("update_path");
		if(path.isEmpty()){
		Log.d(TAG, "update update.zip path is null");
		}else{
		installUpdateImage(path);
		}
    }

    public int installUpdateImage(String path) {
        if (DEBUG) Log.d(TAG, "update update.zip " + path);
        File recoveryFile = new File(path);
        try {
            RecoverySystem.installPackage(mContext, recoveryFile);
        } catch (Exception e) {
            e.printStackTrace();
            if (DEBUG) Log.d(TAG, "install update.zip fail");
            return 0;
        } finally {
            if (DEBUG) Log.d(TAG, "install update.zip ing..");
        }
        return 1;
    }

}
