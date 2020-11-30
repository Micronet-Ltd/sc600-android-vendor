/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmbrowser;

import android.app.Activity;
import android.app.KeyguardManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.view.KeyEvent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.content.ComponentName;
import android.webkit.WebView;

import com.verizon.dmbrowser.R;

public class Browser extends Activity{

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.browser);
        WebView webView = (WebView) findViewById(R.id.webView);
        Uri data = getIntent().getData();
        if(webView != null) {
            webView.loadUrl(data.toString());
        } else {
            Log.e("Browser", "webView is null");
        }
    }

    @Override
    public void onBackPressed()
    {
        super.onBackPressed();
    }

    public void toTakeAppToForeground() {
        Intent notificationIntent = new Intent();
        notificationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_REORDER_TO_FRONT | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        notificationIntent.setComponent(new ComponentName("com.verizon.dmclientupdate", "com.verizon.dmclientupdate.OmadmFumoDialog"));
        PendingIntent pendingIntent = PendingIntent.getActivity(Browser.this, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        try {
            pendingIntent.send();
        }
        catch (PendingIntent.CanceledException e) {
            e.printStackTrace();
            Log.e("Browser", "CanceledException");
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        toTakeAppToForeground();
    }

    @Override
    public void onStop() {
        super.onStop();
        finishAndRemoveTask();
    }
}
