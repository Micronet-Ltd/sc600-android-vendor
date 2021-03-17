/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.widget.RemoteViews;
import android.util.Log;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.App;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class DownloadingNotification {
    protected NotificationManager notificationManager;
    protected static final String TAG = "DownloadingNotification";

    protected static DownloadingNotification sDownloadingNotification = null;
    protected Utils mUtils = Utils.getInstance(App.getContext());

    public static DownloadingNotification getInstance() {
        if (sDownloadingNotification == null) {
            sDownloadingNotification = new DownloadingNotification();
        }
        return sDownloadingNotification;
    }

    public void show(Context context){
        Resources resources = context.getResources();
        RemoteViews contentView = new RemoteViews(context.getPackageName(), R.layout.custom_notification_layout_downloading);
        contentView.setProgressBar(R.id.downloading_notifprogress, 100, mUtils.getProgress(), false);

        Calendar c = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
        String strDate = sdf.format(c.getTime());
        contentView.setTextViewText(R.id.downloading_time,strDate);
        contentView.setTextViewTextSize(R.id.downloading_time, 2, 11);

        Intent notificationIntent = new Intent(context, OmadmFumoDialog.class);
        PendingIntent contentIntent = PendingIntent.getActivity(context, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        Notification.Builder mNotifyBuilder = new Notification.Builder(context);
        Notification notification = mNotifyBuilder
                .setSmallIcon(R.drawable.mobile_ic)
                .setContentTitle(resources.getString(R.string.system_update_info))
                .setContentText(resources.getString(R.string.downloading_system_update))
                .setContentIntent(contentIntent)
                .setOngoing(true)
                .build();
        notification.bigContentView = contentView;
        getNotificationManager(context).notify(Utils.NOTIFY_DOWNLOADING, notification);
    }

    public NotificationManager getNotificationManager(Context context) {
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        return notificationManager;
    }

    public void remove(Context context) {
        getNotificationManager(context).cancel(Utils.NOTIFY_DOWNLOADING);
    }
}
