/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.app.Activity;
import android.app.DialogFragment;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import org.json.JSONException;
import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.fragments.*;
import com.verizon.dmclientupdate.notifications.*;

public class MainActivity extends Activity implements View.OnClickListener {

    protected FragmentManager fm;
    protected NotificationReceiver mNotificationReceiver;
    protected GuiMessageDescriptor guiMessageDescriptor;
    protected Utils mUtils;
    protected TextView noUpdatesApplies;
    protected static final String TAG = "MainActivity";
    protected UpdateInfo updateInfo;
    MyTimer timer;
    protected HomeWatcher mHomeWatcher;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);
        mUtils = Utils.getInstance(getApplicationContext());
        Button checkForUpdatesButton = (Button) findViewById(R.id.check_for_updates_button);
        Button historyUpdate = (Button) findViewById(R.id.history_update);
        noUpdatesApplies = (TextView)findViewById(R.id.no_updates_applies);
        checkForUpdatesButton.setOnClickListener(this);
        historyUpdate.setOnClickListener(this);
        mHomeWatcher = new HomeWatcher(this);
        guiMessageDescriptor = App.getGuiMessageDescriptor();
    }

    public void transactions(FragmentManager fragmentManager, Fragment fragment){
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.replace(R.id.info_frame, fragment).commit();
    }

    @Override
    public void onResume() {
        super.onResume();
        mHomeWatcher.setOnHomePressedListener(new HomeWatcher.OnHomePressedListener() {
            @Override
            public void onHomePressed() {
                timer = MyTimer.getInstance();
                if(timer.getTimer() != null) {
                    timer.stopTimerTask();
                }
            }
        });
        mHomeWatcher.startWatch();
        getUpdateHistory();
        try {
            if (updateInfo.mLastUpdateStatus !=null) {
                if(updateInfo.mLastUpdateStatus.equals(Utils.SUCCESS)) {
                    Fragment appliedFragment = new HistoryAppliedUpdateFragment();
                    transactions(getFragmentManager(), appliedFragment);
                } else {
                    Fragment failedFragment = new HistoryFailedUpdateFragment();
                    transactions(getFragmentManager(), failedFragment);
                }
                noUpdatesApplies.setVisibility(View.GONE);
            } else {
                noUpdatesApplies.setVisibility(View.VISIBLE);
                noUpdatesApplies.setText(R.string.no_updates_applies);
                Log.e(TAG, "updateInfo.mUpdateStatus is null");
            }
        } catch(NullPointerException e) {
            e.printStackTrace();
        }
    }

    private void showHistoryDialog() {
        FragmentTransaction ft =
                getFragmentManager().beginTransaction();
        Fragment prev = getFragmentManager().findFragmentByTag(
                SystemUpdateHistory.FRAGMENT_TAG);
        if (prev != null) {
            ft.remove(prev);
        }
        ft.addToBackStack(null);

        DialogFragment appliedFragment = new SystemUpdateHistory();
        appliedFragment.show(ft,
                SystemUpdateHistory.FRAGMENT_TAG);
    }

    private void getUpdateHistory() {
        ParseJson parseJson = new ParseJson(Utils.getInstance(this));
        try {
            parseJson.read();
        } catch (JSONException e) {
            e.printStackTrace();
        }
        updateInfo = App.getUpdateInfo();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.check_for_updates_button:
                mUtils.checkForUpdates();
                mUtils.sendUserReplyToOmadmFumoPlugin(guiMessageDescriptor.getmState(), 0, false, false, Utils.EBT_CHECK_FOR_UPDATE);
                timer = MyTimer.getInstance();
                timer.startChecking(getFragmentManager(), new CheckUpdateFragment());
                break;
            case R.id.history_update:
                showHistoryDialog();
                break;
        }
    }
}
