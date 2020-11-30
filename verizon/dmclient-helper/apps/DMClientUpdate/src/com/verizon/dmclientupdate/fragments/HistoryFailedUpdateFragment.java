/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.Fragment;
import android.content.res.Resources;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.UpdateInfo;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.App;

public class HistoryFailedUpdateFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.history_failed_update_fragment, null);
        Resources res = getResources();
        TextView softwereUpdateTo = (TextView)v.findViewById(R.id.softwere_update_to);
        TextView failedUpdateTime = (TextView)v.findViewById(R.id.info);
        TextView errorCode = (TextView)v.findViewById(R.id.error_code);
        UpdateInfo updateInfo = App.getUpdateInfo();
        String resultSoftwareVersion = String.format(res.getString(R.string.software_not_update_to), updateInfo.getmLastUpdateSwVersion());
        softwereUpdateTo.setText(resultSoftwareVersion);

        String result = String.format(res.getString(R.string.info), "",
                updateInfo.getmLastUpdateTime());
        failedUpdateTime.setText(result);
        String resultErrorCode = String.format(res.getString(R.string.error_code), updateInfo.getmLastUpdateFailureError());
        errorCode.setText(resultErrorCode);
        return v;
    }
}
