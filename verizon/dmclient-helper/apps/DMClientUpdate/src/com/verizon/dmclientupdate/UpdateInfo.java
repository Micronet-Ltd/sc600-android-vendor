/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */
package com.verizon.dmclientupdate;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.util.Log;

import com.verizon.dmclientupdate.R;

public class UpdateInfo {

    private static final String TAG = UpdateInfo.class.getSimpleName();

    public UpdateInfo(){}

    private static SimpleDateFormat DATE_FMT_IN =
            new SimpleDateFormat("MM:dd:yyyy:HH:mm");

    static {
        DATE_FMT_IN.setTimeZone(TimeZone.getTimeZone("Etc/UTC"));
    }

    private static final long MSECS_IN_30_DAYS = 2592000000L;

    // for fragment: last system history update
    protected String mLastUpdateSwVersion;
    protected String mLastUpdateStatus;
    protected String mLastUpdateText;
    protected String mLastUpdateTime;
    protected String mLastUpdateUrl;
    protected String mLastUpdateFailureError;

    // for dialog: check for system update
    protected String mLastSuccessSwVersion;
    protected String mLastSuccessTime;
    protected String mPreLastSuccessSwVersion;
    protected String mPreLastSuccessTime;

    public UpdateInfo(JSONArray array) throws JSONException {
        JSONObject fupi = array.getJSONObject(array.length() - 1);
        this.mLastUpdateSwVersion = getStringFromJsonObject("Software version", fupi);
        this.mLastUpdateStatus = getStringFromJsonObject("Update status", fupi);
        this.mLastUpdateText = getStringFromJsonObject("Text", fupi);
        String time = getStringFromJsonObject("Time", fupi);
        String date = getStringFromJsonObject("Date", fupi);
        this.mLastUpdateTime = gmtString2LocalTimeString(time, date);
        if (isLearnMoreExpired(time, date)) {
            this.mLastUpdateUrl = "";
        } else {
            this.mLastUpdateUrl = getStringFromJsonObject("learn more URL", fupi);
        }
        this.mLastUpdateFailureError = getStringFromJsonObject("failure error", fupi);

        for(int i = array.length() - 1; i >= 0; i--) {
            JSONObject obj = array.getJSONObject(i);
            String status = getStringFromJsonObject("Update status", obj);
            if(status.equals(Utils.SUCCESS)) {
                String swVersion = getStringFromJsonObject("Software version", obj);
                time = getStringFromJsonObject("Time", obj);
                date = getStringFromJsonObject("Date", obj);
                if(this.mLastSuccessSwVersion == null) {
                    this.mLastSuccessSwVersion = swVersion;
                    this.mLastSuccessTime = gmtString2LocalTimeString(time, date);
                } else {
                    this.mPreLastSuccessSwVersion = swVersion;
                    this.mPreLastSuccessTime = gmtString2LocalTimeString(time, date);
                    break;
                }
            }
        }
    }

    private static Date gmt2localTime(Date gmt) {
        TimeZone tz = TimeZone.getDefault();
        int offsetFromUtc = tz.getOffset(gmt.getTime());
        Date date = new Date(offsetFromUtc + gmt.getTime());
        return date;
    }

    private static Date gmtString2LocalTime(String hhmm, String yyyymmdd) {
        String fullStringGmtDate = yyyymmdd + ":" + hhmm;
        Date date = null;
        try {
            date = DATE_FMT_IN.parse(fullStringGmtDate);
        } catch (ParseException e) {
            Log.w(TAG, "gmtString2LocalTime(" + fullStringGmtDate + "):err", e);
        }
        return date;
    }

    private static boolean isLearnMoreExpired(String hhmm, String yyyymmdd) {
        if(hhmm == null || yyyymmdd == null) {
            return false;
        }
        Date updateTime = gmtString2LocalTime(hhmm, yyyymmdd);
        if (updateTime != null) {
            Date expiredTime = new Date(updateTime.getTime() + MSECS_IN_30_DAYS);
            Date currentDate = new Date(System.currentTimeMillis());
            if (currentDate.after(expiredTime)) {
                return true;
            }
        }
        return false;
    }

    private static String gmtString2LocalTimeString(
            String hhmm, String yyyymmdd) {
        String localTimeString = null;
        if(hhmm == null || yyyymmdd == null) {
            return localTimeString;
        }
        Date gmTime = gmtString2LocalTime(hhmm, yyyymmdd);
        Context ctx = App.getContext();
        if (ctx != null) {
            String sdfPattern = ctx.getResources().
                    getString(R.string.update_info_date_fmt_out);
            SimpleDateFormat sdf = new SimpleDateFormat(sdfPattern);
            sdf.setTimeZone(TimeZone.getDefault());
            localTimeString = sdf.format(gmTime);
        }
        return localTimeString;
    }

    private String getStringFromJsonObject(String fieldName, JSONObject jsonObject) {
        String result = null;
        try {
            result = ((jsonObject.has(fieldName) && !jsonObject.isNull(fieldName))) ? jsonObject.getString(fieldName) : null;
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return result;
    }

    public String getmLastUpdateSwVersion() {
        return mLastUpdateSwVersion;
    }

    public void setmLastUpdateSwVersion(String mLastUpdateSwVersion) {
        this.mLastUpdateSwVersion = mLastUpdateSwVersion;
    }

    public String getmLastUpdateStatus() {
        return mLastUpdateStatus;
    }

    public void setmLastUpdateStatus(String mLastUpdateStatus) {
        this.mLastUpdateStatus = mLastUpdateStatus;
    }

    public String getmLastUpdateText() {
        return mLastUpdateText;
    }

    public void setmLastUpdateText(String mLastUpdateText) {
        this.mLastUpdateText = mLastUpdateText;
    }

    public String getmLastUpdateTime() {
        return mLastUpdateTime;
    }

    public void setmLastUpdateTime(String mLastUpdateTime) {
        this.mLastUpdateTime = mLastUpdateTime;
    }

    public String getmLastUpdateUrl() {
        return mLastUpdateUrl;
    }

    public void setmLastUpdateUrl(String mLastUpdateUrl) {
        this.mLastUpdateUrl = mLastUpdateUrl;
    }

    public String getmLastUpdateFailureError() {
        return mLastUpdateFailureError;
    }

    public void setmLastUpdateFailureError(String mLastUpdateFailureError) {
        this.mLastUpdateFailureError = mLastUpdateFailureError;
    }

    public String getmLastSuccessSwVersion() {
        return mLastSuccessSwVersion;
    }

    public void setmLastSuccessSwVersion(String mLastSuccessSwVersion) {
        this.mLastSuccessSwVersion = mLastSuccessSwVersion;
    }

    public String getmLastSuccessTime() {
        return mLastSuccessTime;
    }

    public void setmLastSuccessTime(String mLastSuccessTime) {
        this.mLastSuccessTime = mLastSuccessTime;
    }

    public String getmPreLastSuccessSwVersion() {
        return mPreLastSuccessSwVersion;
    }

    public void setmPreLastSuccessSwVersion(String mPreLastSuccessSwVersion) {
        this.mPreLastSuccessSwVersion = mPreLastSuccessSwVersion;
    }

    public String getmPreLastSuccessTime() {
        return mPreLastSuccessTime;
    }

    public void setmPreLastSuccessTime(String mPreLastSuccessTime) {
        this.mPreLastSuccessTime = mPreLastSuccessTime;
    }
}