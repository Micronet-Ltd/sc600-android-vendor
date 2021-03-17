/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.content.res.Resources;
import android.widget.RemoteViews;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.App;
import com.verizon.dmclientupdate.Utils;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class InstallingNotification {
    private NotificationManager notificationManager;
    private static InstallingNotification sInstallingNotification = null;
    protected Utils mUtils = Utils.getInstance(App.getContext());
    public static InstallingNotification getInstance() {
        if (sInstallingNotification == null) {
            sInstallingNotification = new InstallingNotification();
        }
        return sInstallingNotification;
    }

    public void show(Context context){
        Resources resources = context.getResources();
        RemoteViews contentView = new RemoteViews(context.getPackageName(), R.layout.custom_notification_layout_installing);
        contentView.setProgressBar(R.id.installing_notifprogress, 100, mUtils.getProgress(), false);
        Calendar c = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
        String strDate = sdf.format(c.getTime());
        contentView.setTextViewText(R.id.installing_time,strDate);
        contentView.setTextViewTextSize(R.id.installing_time, 2, 11);
        Notification.Builder mNotifyBuilder = new Notification.Builder(context);
        Notification notification = mNotifyBuilder
                .setSmallIcon(R.drawable.mobile_ic)
                .setContentTitle(resources.getString(R.string.system_update_info))
                .setContentText(resources.getString(R.string.installing_system_update))
                .setOngoing(true)
                .build();
        notification.bigContentView = contentView;
        getNotificationManager(context).notify(Utils.NOTIFY_INSTALLING, notification);
    }

    public NotificationManager getNotificationManager(Context context) {
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        return notificationManager;
    }

    public void remove(Context context) {
        getNotificationManager(context).cancel(Utils.NOTIFY_INSTALLING);
    }
}
