/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;


import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.DialogInterface.OnShowListener;
import android.graphics.Typeface;
import android.util.TypedValue;
import android.view.ContextThemeWrapper;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.verizon.dmclientupdate.R;

public class OmadmControllerAlertDialog extends AlertDialog.Builder implements OnShowListener, OnClickListener {
    private Context mContext;
    public OmadmControllerAlertDialog(Context context, int title, int ui_type) {
        super(new ContextThemeWrapper(context, android.R.style.Theme_Holo_Light));
        mContext = context;
        setTitle(title);
        setIcon(android.R.drawable.ic_dialog_alert);
        switch (ui_type) {
            case OmadmControllerDialog.DMCLT_UI_TYPE_DISPLAY:
            case OmadmControllerDialog.DMCLT_UI_TYPE_DISPLAY_UIE:
                setPositiveButton(R.string.system_update_omadm_controller_dialog_ok_button, this);
                break;
            case OmadmControllerDialog.DMCLT_UI_TYPE_CONFIRM:
            case OmadmControllerDialog.DMCLT_UI_TYPE_USER_INPUT:
            case OmadmControllerDialog.DMCLT_UI_TYPE_USER_MULTICHOICE:
                setPositiveButton(R.string.system_update_omadm_controller_dialog_accept_button, this);
                setNegativeButton(R.string.system_update_omadm_controller_dialog_cancel_button, this);
                break;
            case OmadmControllerDialog.DMCLT_UI_TYPE_USER_CHOICE:
                setNegativeButton(R.string.system_update_omadm_controller_dialog_cancel_button, this);
                break;
            default:
                break;
        }
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            doPositiveClick();
        } else if ( (which == DialogInterface.BUTTON_NEGATIVE)){
            doNegativeClick();
        }
    }

    public void doPositiveClick() {
    }

    public void doNegativeClick() {
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onShow(DialogInterface dialog) {
        AlertDialog alertDialog = (AlertDialog) dialog;
        // Title and Divider style update
        int titleTextViewId = mContext.getResources().getIdentifier("android:id/alertTitle", null, null);
        TextView titleTextView = (TextView)alertDialog.findViewById(titleTextViewId);
        titleTextView.setTypeface(Typeface.DEFAULT, Typeface.BOLD | Typeface.ITALIC);
        titleTextView.setTextColor(mContext.getResources().getColor(android.R.color.holo_red_dark));
        int titleDividerId = alertDialog.getContext().getResources().getIdentifier("android:id/titleDivider", null, null);
        if (titleDividerId != 0) {
            View dialogDivider = alertDialog.findViewById(titleDividerId);
            dialogDivider.setBackgroundColor(mContext.getResources().getColor(android.R.color.holo_red_dark));
        }

        // Button style update
        Button positiveButton = alertDialog.getButton(AlertDialog.BUTTON_POSITIVE);
        Button negativeButton = alertDialog.getButton(AlertDialog.BUTTON_NEGATIVE);
        positiveButton.setTextColor(mContext.getResources().getColor(android.R.color.holo_red_dark));
        positiveButton.setTypeface(Typeface.DEFAULT, Typeface.BOLD | Typeface.ITALIC);
        positiveButton.setTextSize(TypedValue.COMPLEX_UNIT_SP, (float) 16);
        negativeButton.setTextColor(mContext.getResources().getColor(android.R.color.holo_red_dark));
        negativeButton.setTypeface(Typeface.DEFAULT, Typeface.BOLD | Typeface.ITALIC);
        negativeButton.setTextSize(TypedValue.COMPLEX_UNIT_SP, (float) 16);
    }
}
