/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import com.verizon.dmclientupdate.R;

public class App extends android.app.Application {


    private static App instance;

    public App() {
        instance = this;
    }

    public static Context getContext() {
        return instance;
    }

    private final static String UPDATE_INFO_PREFS = "info_prefs";

    private final static String UPDATE_INFO_SW_VERSION = "info_sw_version";
    private final static String UPDATE_INFO_UPDATE_STATUS = "info_update_status";
    private final static String UPDATE_INFO_TEXT = "info_text";
    private final static String UPDATE_INFO_TIME = "info_time";
    private final static String UPDATE_INFO_URL = "info_url";
    private final static String UPDATE_INFO_ERROR = "info_error";

    private final static String UPDATE_INFO_SW_VERSION_LAST_SUCCESS = "info_sw_version_last_success";
    private final static String UPDATE_INFO_TIME_LAST_SUCCESS = "info_time_last_success";

    private final static String UPDATE_INFO_SW_VERSION_PRE_LAST_SUCCESS = "info_sw_version_pre_last_success";
    private final static String UPDATE_INFO_TIME_PRE_LAST_SUCCESS = "info_time_pre_last_success";

    private final static String GUI_MESSAGE_DESCRIPTOR_PREFS = "gui_prefs";

    private final static String GUI_MESSAGE_DESCRIPTOR_STATE = "gui_state";
    private final static String GUI_MESSAGE_DESCRIPTOR_TYPE = "gui_type";
    private final static String GUI_MESSAGE_DESCRIPTOR_MODE = "gui_mode";
    private final static String GUI_MESSAGE_DESCRIPTOR_ICON_TYPE = "gui_icon_type";
    private final static String GUI_MESSAGE_DESCRIPTOR_HEADER = "gui_header";
    private final static String GUI_MESSAGE_DESCRIPTOR_MESSAGE = "gui_message";
    private final static String GUI_MESSAGE_DESCRIPTOR_LINK_CAPTION = "gui_link_caption";
    private final static String GUI_MESSAGE_DESCRIPTOR_LINK = "gui_link";
    private final static String GUI_MESSAGE_DESCRIPTOR_PROGRESS = "gui_progress";
    private final static String GUI_MESSAGE_DESCRIPTOR_BUTTON_TYPE = "gui_button_type";
    private final static String GUI_MESSAGE_DESCRIPTOR_INSTALL_PARAM = "gui_install_param";
    private final static String GUI_MESSAGE_DESCRIPTOR_REQUIRED_SPACE_FOR_UPDATE = "gui_required_space_for_update";
    private final static String GUI_MESSAGE_DESCRIPTOR_REQUIRED_SPACE_FOR_DELETE = "gui_required_space_for_delete";
    private final static String GUI_MESSAGE_DESCRIPTOR_SEVERITY = "gui_severity";

    public static void saveUpdateInfo(UpdateInfo ui) {
        Context context = App.getContext();
        SharedPreferences prefs = context.getSharedPreferences(UPDATE_INFO_PREFS, 0);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putString(UPDATE_INFO_SW_VERSION, ui.mLastUpdateSwVersion);
        editor.putString(UPDATE_INFO_UPDATE_STATUS, ui.mLastUpdateStatus);
        editor.putString(UPDATE_INFO_TEXT, ui.mLastUpdateText);
        editor.putString(UPDATE_INFO_TIME, ui.mLastUpdateTime);
        editor.putString(UPDATE_INFO_URL, ui.mLastUpdateUrl);
        editor.putString(UPDATE_INFO_ERROR, ui.mLastUpdateFailureError);

        editor.putString(UPDATE_INFO_SW_VERSION_LAST_SUCCESS, ui.mLastSuccessSwVersion);
        editor.putString(UPDATE_INFO_TIME_LAST_SUCCESS, ui.mLastSuccessTime);

        editor.putString(UPDATE_INFO_SW_VERSION_PRE_LAST_SUCCESS, ui.mPreLastSuccessSwVersion);
        editor.putString(UPDATE_INFO_TIME_PRE_LAST_SUCCESS, ui.mPreLastSuccessTime);
        editor.commit();
    }

    public static UpdateInfo getUpdateInfo() {
        Context context = App.getContext();
        SharedPreferences prefs = context.getSharedPreferences(UPDATE_INFO_PREFS, 0);
        UpdateInfo ui = new UpdateInfo();
        ui.mLastUpdateSwVersion = prefs.getString(UPDATE_INFO_SW_VERSION, null);
        ui.mLastUpdateStatus = prefs.getString(UPDATE_INFO_UPDATE_STATUS, null);
        ui.mLastUpdateText = prefs.getString(UPDATE_INFO_TEXT, null);
        ui.mLastUpdateTime = prefs.getString(UPDATE_INFO_TIME, null);
        ui.mLastUpdateUrl = prefs.getString(UPDATE_INFO_URL, null);
        ui.mLastUpdateFailureError = prefs.getString(UPDATE_INFO_ERROR, null);

        ui.mLastSuccessSwVersion = prefs.getString(UPDATE_INFO_SW_VERSION_LAST_SUCCESS, null);
        ui.mLastSuccessTime = prefs.getString(UPDATE_INFO_TIME_LAST_SUCCESS, null);

        ui.mPreLastSuccessSwVersion = prefs.getString(UPDATE_INFO_SW_VERSION_PRE_LAST_SUCCESS, null);
        ui.mPreLastSuccessTime = prefs.getString(UPDATE_INFO_TIME_PRE_LAST_SUCCESS, null);
        return ui;
    }

    public static void saveGuiMessageDescriptor(GuiMessageDescriptor gmd) {
        Context context = App.getContext();
        SharedPreferences prefs = context.getSharedPreferences(GUI_MESSAGE_DESCRIPTOR_PREFS, 0);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putInt(GUI_MESSAGE_DESCRIPTOR_STATE, gmd.mState);
        editor.putInt(GUI_MESSAGE_DESCRIPTOR_TYPE, gmd.mMessageType);
        editor.putInt(GUI_MESSAGE_DESCRIPTOR_MODE, gmd.mMessageMode);
        editor.putInt(GUI_MESSAGE_DESCRIPTOR_ICON_TYPE, gmd.mIconType);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_HEADER, gmd.mHeaderText);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_MESSAGE, gmd.mMessageText);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_LINK_CAPTION, gmd.mHyperLinkCaption);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_LINK, gmd.mHyperLink);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_INSTALL_PARAM, gmd.mInstallParam);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_PROGRESS, gmd.mProgressBarCaption);
        editor.putInt(GUI_MESSAGE_DESCRIPTOR_BUTTON_TYPE, gmd.mButtonType);
        editor.putLong(GUI_MESSAGE_DESCRIPTOR_REQUIRED_SPACE_FOR_UPDATE, gmd.mRequiredSpaceForUpdate);
        editor.putLong(GUI_MESSAGE_DESCRIPTOR_REQUIRED_SPACE_FOR_DELETE, gmd.mRequiredSpaceForDelete);
        editor.putString(GUI_MESSAGE_DESCRIPTOR_SEVERITY, gmd.mSeverity);
        editor.commit();
    }

    public static GuiMessageDescriptor getGuiMessageDescriptor() {
        Context context = App.getContext();
        SharedPreferences prefs = context.getSharedPreferences(GUI_MESSAGE_DESCRIPTOR_PREFS, 0);
        GuiMessageDescriptor md = new GuiMessageDescriptor();
        md.mState = prefs.getInt(GUI_MESSAGE_DESCRIPTOR_STATE, 10);
        md.mMessageType = prefs.getInt(GUI_MESSAGE_DESCRIPTOR_TYPE, 10);
        md.mMessageMode = prefs.getInt(GUI_MESSAGE_DESCRIPTOR_MODE, 10);
        md.mIconType = prefs.getInt(GUI_MESSAGE_DESCRIPTOR_ICON_TYPE, 10);
        md.mHeaderText = prefs.getString(GUI_MESSAGE_DESCRIPTOR_HEADER, "");
        md.mMessageText = prefs.getString(GUI_MESSAGE_DESCRIPTOR_MESSAGE, "");
        md.mHyperLinkCaption = prefs.getString(GUI_MESSAGE_DESCRIPTOR_LINK_CAPTION, "");
        md.mHyperLink = prefs.getString(GUI_MESSAGE_DESCRIPTOR_LINK, "");
        md.mInstallParam = prefs.getString(GUI_MESSAGE_DESCRIPTOR_INSTALL_PARAM, "");
        md.mProgressBarCaption = prefs.getString(GUI_MESSAGE_DESCRIPTOR_PROGRESS, "");
        md.mButtonType = prefs.getInt(GUI_MESSAGE_DESCRIPTOR_BUTTON_TYPE, 10);
        md.mRequiredSpaceForUpdate = prefs.getLong(GUI_MESSAGE_DESCRIPTOR_REQUIRED_SPACE_FOR_UPDATE, 0);
        md.mRequiredSpaceForDelete = prefs.getLong(GUI_MESSAGE_DESCRIPTOR_REQUIRED_SPACE_FOR_DELETE, 0);
        md.mSeverity = prefs.getString(GUI_MESSAGE_DESCRIPTOR_SEVERITY, "3");
        return md;
    }

    public static void cleanGuiMessageDescriptor() {
        Context context = App.getContext();
        SharedPreferences prefs = context.getSharedPreferences(GUI_MESSAGE_DESCRIPTOR_PREFS, 0);
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.commit();
    }
}
