/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.DialogFragment;
import android.app.FragmentManager;

import com.verizon.dmclientupdate.UpdateInfo;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.GuiMessageDescriptor;
import com.verizon.dmclientupdate.App;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Spinner;
import android.content.DialogInterface;
import android.view.KeyEvent;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.OmadmFumoDialog;
import com.verizon.dmclientupdate.notifications.*;

import java.lang.Exception;
import java.lang.String;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.TimeZone;

public abstract class BaseLaterFragment extends BaseFragment implements View.OnClickListener {

    protected static final int SIMPLE_DROPDOWN_ITEM = android.R.layout.simple_dropdown_item_1line;
    private static final int HOURS = 12;
    private static final int MINUTES = 60;
    protected static final String PART_AM = "AM";
    protected static final String PART_PM = "PM";
    private static final String MINUTES_FORMAT = "%02d";
    protected ArrayAdapter<String> hourOfTheDayAdapter;
    protected ArrayAdapter<String> minutesAdapter;
    protected ArrayAdapter<String> amPmAdapter;
    protected Spinner hourOfTheDaySpinner;
    protected Spinner minutesSpinner;
    protected Spinner amPmSpinner;
    protected boolean mCloseAppWithNotificationFlag = true;

    @Override
    public void onStop() {
        super.onStop();
        if (mCloseAppWithNotificationFlag) {
            SystemAvaliableNotification.getInstance().show(getActivity());
            ((OmadmFumoDialog) getActivity()).finishAndRemoveTask();
        }
    }

    protected void initSpinners(View view, String defaultHours, String defaultMinutes, String defaultDayPart) {

        hourOfTheDaySpinner = (Spinner) view.findViewById(R.id.spinner_day);
        String[] items = new String[HOURS];
        for (int i = 1; i < HOURS + 1; ++i) {
            items[i - 1] = String.valueOf(i);
        }
        hourOfTheDayAdapter = new ArrayAdapter<>(getActivity(), SIMPLE_DROPDOWN_ITEM, items);
        hourOfTheDaySpinner.setAdapter(hourOfTheDayAdapter);
        setSpinnerSelection(hourOfTheDaySpinner, hourOfTheDayAdapter, defaultHours);

        minutesSpinner = (Spinner) view.findViewById(R.id.spinner_time);
        String[] items2 = new String[MINUTES];
        for (int i = 0; i < MINUTES; i++) {
            items2[i] = String.format(MINUTES_FORMAT, i) ;
        }
        minutesAdapter = new ArrayAdapter<>(getActivity(), SIMPLE_DROPDOWN_ITEM, items2);
        minutesSpinner.setAdapter(minutesAdapter);
        setSpinnerSelection(minutesSpinner, minutesAdapter, defaultMinutes);


        amPmSpinner = (Spinner) view.findViewById(R.id.spinner_part_of_day);
        String[] items3 = new String[]{PART_AM, PART_PM};
        amPmAdapter = new ArrayAdapter<>(getActivity(), SIMPLE_DROPDOWN_ITEM, items3);
        amPmSpinner.setAdapter(amPmAdapter);
        setSpinnerSelection(amPmSpinner, amPmAdapter, defaultDayPart);
    }

    void setSpinnerSelection(Spinner spinner, ArrayAdapter<String> arrayAdapter, String itemValue) {
        int itemPosition = arrayAdapter.getPosition(itemValue);
        spinner.setSelection(itemPosition);
    }

    public long getSeconds(){
        int  positionOfHour = hourOfTheDaySpinner.getSelectedItemPosition();
        String hourOfTheDay = hourOfTheDayAdapter.getItem(positionOfHour);
        int  positionOfminutes = minutesSpinner.getSelectedItemPosition();
        String minutes = minutesAdapter.getItem(positionOfminutes);
        int  positionAmPm = amPmSpinner.getSelectedItemPosition();
        String amPm = amPmAdapter.getItem(positionAmPm);
        String deferedUpdateTime = hourOfTheDay + ":" + minutes;

        Date date = new Date();
        DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd hh:mm a");
        try {
            date = sdf.parse(dateFormat.format(date) +" "+ deferedUpdateTime +" "+ amPm);
            Log.d("in milliseconds: ", String.valueOf(date.getTime()));
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return date.getTime();
    }
}
