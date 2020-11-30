/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.support.v4.app.NotificationManagerCompat;
import android.widget.RemoteViews;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class SystemAvaliableNotification {
    private static SystemAvaliableNotification sSystemAvaliableNotification = null;
    public static SystemAvaliableNotification getInstance() {
        if (sSystemAvaliableNotification == null) {
            sSystemAvaliableNotification = new SystemAvaliableNotification();
        }
        return sSystemAvaliableNotification;
    }
    public void show(Context context){
        RemoteViews contentView = new RemoteViews(context.getPackageName(), R.layout.custom_notification_system_update_avaliable);
        Notification.Builder mNotifyBuilder = new Notification.Builder(context);
        Intent intent = new Intent(Utils.ACTION_UPDATE_AVALIABLE_NOTIFICATION);
        PendingIntent contentIntent = PendingIntent.getBroadcast(context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        Calendar c = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
        String strDate = sdf.format(c.getTime());
        contentView.setTextViewText(R.id.avaliable_time,strDate);
        contentView.setTextViewTextSize(R.id.avaliable_time, 2, 11);

        Notification notification = mNotifyBuilder
                .setSmallIcon(R.drawable.mobile_ic)
                .setContentIntent(contentIntent)
                .setOngoing(true)
                .setAutoCancel(true)
                .build();
        notification.bigContentView = contentView;
        getNotificationManager(context).notify(Utils.NOTIFY_MAKE_ROOM, notification);
    }
    public NotificationManager getNotificationManager(Context context) {
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        return notificationManager;
    }
    public void remove(Context context) {
        getNotificationManager(context).cancel(Utils.NOTIFY_MAKE_ROOM);
    }
}

