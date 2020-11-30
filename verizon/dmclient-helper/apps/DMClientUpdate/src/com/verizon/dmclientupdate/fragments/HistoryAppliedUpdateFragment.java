/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate.fragments;

import android.app.Fragment;
import android.content.Intent;
import android.content.res.Resources;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.verizon.dmclientupdate.R;
import com.verizon.dmclientupdate.UpdateInfo;
import com.verizon.dmclientupdate.Utils;
import com.verizon.dmclientupdate.App;

public class HistoryAppliedUpdateFragment extends Fragment implements View.OnClickListener {

    private UpdateInfo updateInfo;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View v = inflater.inflate(R.layout.history_applied_update_fragment, null);
        Resources res = getResources();
        TextView softwareUpdateTo = (TextView)v.findViewById(R.id.softwere_update_to);
        TextView configurableText = (TextView)v.findViewById(R.id.configurable_text);
        TextView learnMoreOnlineButton = (TextView)v.findViewById(R.id.learn_more_online_button);
        TextView info = (TextView)v.findViewById(R.id.info);
        updateInfo = App.getUpdateInfo();
        if (learnMoreOnlineButton != null) {
            String learnMoreUrl = updateInfo.getmLastUpdateUrl();
            if ((learnMoreUrl != null) && (learnMoreUrl.length() > 0)) {
                learnMoreOnlineButton.setOnClickListener(this);
                learnMoreOnlineButton.setVisibility(View.VISIBLE);
            } else {
                learnMoreOnlineButton.setVisibility(View.GONE);
            }
        }
        configurableText.setText(updateInfo.getmLastUpdateText());
        String resultSoftwareVersion = String.format(res.getString(R.string.software_update_to), updateInfo.getmLastUpdateSwVersion());
        softwareUpdateTo.setText(resultSoftwareVersion);
        String result = String.format(res.getString(R.string.info), "", updateInfo.getmLastUpdateTime());
        info.setText(result);
        return v;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.learn_more_online_button:
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(updateInfo.getmLastUpdateUrl()));
                startActivity(browserIntent);
                break;
        }
    }
}
