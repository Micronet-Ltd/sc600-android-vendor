/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.widget.RemoteViews;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;

public class SystemUpdateFailNotification {
    private NotificationManager notificationManager;
    private static SystemUpdateFailNotification sSystemUpdateReadyNotification = null;

    public static SystemUpdateFailNotification getInstance() {
        if (sSystemUpdateReadyNotification == null) {
            sSystemUpdateReadyNotification = new SystemUpdateFailNotification();
        }
        return sSystemUpdateReadyNotification;
    }
    public void show(Context context){
        RemoteViews contentView = new RemoteViews(context.getPackageName(), R.layout.custom_notification_layout_system_update_fail);
        Notification.Builder mNotifyBuilder = new Notification.Builder(context);

        Calendar c = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
        String strDate = sdf.format(c.getTime());
        contentView.setTextViewText(R.id.ready_time,strDate);
        contentView.setTextViewTextSize(R.id.ready_time, 2, 11);

        Intent intent = new Intent(Utils.ACTION_OPEN_ACTIVITY);
        PendingIntent contentIntent = PendingIntent.getBroadcast(context, 0, intent, 0);
        Notification notification = mNotifyBuilder
                .setSmallIcon(R.drawable.mobile_ic)
                .setContentIntent(contentIntent)
                .setOngoing(true)
                .build();
        notification.bigContentView = contentView;
        getNotificationManager(context).notify(Utils.NOTIFY_RESTART_PHONE, notification);
    }
    public NotificationManager getNotificationManager(Context context) {
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        return notificationManager;
    }
    public void remove(Context context) {
        getNotificationManager(context).cancel(Utils.NOTIFY_RESTART_PHONE);
    }
}

