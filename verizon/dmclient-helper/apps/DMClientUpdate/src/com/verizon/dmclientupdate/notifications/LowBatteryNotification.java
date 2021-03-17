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
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class LowBatteryNotification {
    private NotificationManager notificationManager;
    private static LowBatteryNotification sLowBatteryNotification = null;

    public static LowBatteryNotification getInstance() {
        if (sLowBatteryNotification == null) {
            sLowBatteryNotification = new LowBatteryNotification();
        }
        return sLowBatteryNotification;
    }

    public void show(Context context){
        RemoteViews contentView = new RemoteViews(context.getPackageName(), R.layout.custom_notification_layout_low_battary);
        Intent intent = new Intent(Utils.ACTION_LOW_BATTERY);
        PendingIntent contentIntent = PendingIntent.getBroadcast(context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);
        Notification.Builder mNotifyBuilder = new Notification.Builder(context);

        Calendar c = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
        String strDate = sdf.format(c.getTime());
        contentView.setTextViewText(R.id.low_battary_time,strDate);

        Notification notification = mNotifyBuilder
                .setSmallIcon(R.drawable.mobile_ic)
                .setContentIntent(contentIntent)
                .setContent(contentView)
                .setAutoCancel(true)
                .build();
        notification.bigContentView = contentView;
        getNotificationManager(context).notify(Utils.NOTIFY_LOW_BATTARY, notification);
    }

    public NotificationManager getNotificationManager(Context context) {
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        return notificationManager;
    }

    public void remove(Context context) {
        getNotificationManager(context).cancel(Utils.NOTIFY_LOW_BATTARY);
    }
}
