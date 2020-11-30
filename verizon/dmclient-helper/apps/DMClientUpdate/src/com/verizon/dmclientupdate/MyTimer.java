/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.app.DialogFragment;
import android.app.FragmentManager;
import android.os.Handler;

import com.verizon.dmclientupdate.fragments.CheckUpdateFragment;
import com.verizon.dmclientupdate.fragments.UpdateNotAvaliableFragment;

import java.util.Timer;
import java.util.TimerTask;

public class MyTimer extends DialogFragment{
    public static final int UPDATE_AVAILABLE_TIMEOUT = 15000;
    private static MyTimer sMyTimer = null;
    public static MyTimer getInstance() {
        if (sMyTimer == null) {
            sMyTimer = new MyTimer();
        }
        return sMyTimer;
    }

    public void transactions(FragmentManager fragmentManager, DialogFragment fragment) {
        fragment.show(fragmentManager, null);
    }

    protected Timer timer;
    protected TimerTask timerTask;
    protected final Handler handler = new Handler();
    protected DialogFragment dialogFragment;
    protected FragmentManager manager;
    protected int i;

    public void startChecking(FragmentManager manager, DialogFragment dialogFragment) {
        this.dialogFragment = dialogFragment;
        this.manager = manager;
        timer = new Timer();
        initializeTimerTask();
        transactions(manager, dialogFragment);
        timer.schedule(timerTask, UPDATE_AVAILABLE_TIMEOUT);
    }

    public Timer getTimer() {
        return timer;
    }

    public void stopTimerTask() {
        if (timer != null) {
            timerTask.cancel();
            timerTask = null;
            timer.cancel();
            timer = null;
            dialogFragment.dismiss();
            sMyTimer = null;
        }
    }

    public void initializeTimerTask() {

        timerTask = new TimerTask() {
            public void run() {
                handler.post(new Runnable() {
                    public void run() {
                        transactions(manager, UpdateNotAvaliableFragment.getInstance());
                        stopTimerTask();
                    }
                });
            }
        };
    }
}
