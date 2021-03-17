/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

import android.omadm.FumoGuiMessageDescriptor;
import android.omadm.OmadmManager;

public class GuiMessageDescriptor {

    public GuiMessageDescriptor(){}

    public GuiMessageDescriptor(FumoGuiMessageDescriptor fgmd) {
        this.mState = fgmd.mState;
        this.mMessageType = fgmd.mMessageType;
        this.mMessageMode = fgmd.mMessageMode;
        this.mIconType = fgmd.mIconType;
        this.mHeaderText = fgmd.mHeaderText;
        this.mMessageText = fgmd.mMessageText;
        this.mHyperLinkCaption = fgmd.mHyperLinkCaption;
        this.mHyperLink = fgmd.mHyperLink;
        this.mProgressBarCaption = fgmd.mProgressBarCaption;
        this.mInstallParam = fgmd.mInstallParam;
        this.mButtonType = fgmd.mButtonType;
        this.mRequiredSpaceForUpdate = fgmd.mRequiredSpaceForUpdate;
        this.mRequiredSpaceForDelete = fgmd.mRequiredSpaceForDelete;
        this.mSeverity = fgmd.mSeverity;
    }

    public int mState;
    public int mMessageType;
    public int mMessageMode;
    public int mIconType;
    public String mHeaderText;
    public String mMessageText;
    public String mHyperLinkCaption;
    public String mHyperLink;
    public String mProgressBarCaption;
    public String mInstallParam;
    public int mButtonType;
    public long mRequiredSpaceForUpdate;
    public long mRequiredSpaceForDelete;
    public String mSeverity;

    public String getmSeverity() {
        return mSeverity;
    }

    public void setmSeverity(String mSeverity) {
        this.mSeverity = mSeverity;
    }

    public String getmInstallParam() {
        return mInstallParam;
    }

    public void setmInstallParam(String mInstallParam) {
        this.mInstallParam = mInstallParam;
    }

    public int getmState() {
        return mState;
    }

    public void setmState(int mState) {
        this.mState = mState;
    }

    public int getmMessageType() {
        return mMessageType;
    }

    public void setmMessageType(int mMessageType) {
        this.mMessageType = mMessageType;
    }

    public int getmMessageMode() {
        return mMessageMode;
    }

    public void setmMessageMode(int mMessageMode) {
        this.mMessageMode = mMessageMode;
    }

    public int getmIconType() {
        return mIconType;
    }

    public void setmIconType(int mIconType) {
        this.mIconType = mIconType;
    }

    public String getmHeaderText() {
        return mHeaderText;
    }

    public void setmHeaderText(String mHeaderText) {
        this.mHeaderText = mHeaderText;
    }

    public String getmMessageText() {
        return mMessageText;
    }

    public void setmMessageText(String mMessageText) {
        this.mMessageText = mMessageText;
    }

    public String getmHyperLinkCaption() {
        return mHyperLinkCaption;
    }

    public void setmHyperLinkCaption(String mHyperLinkCaption) {
        this.mHyperLinkCaption = mHyperLinkCaption;
    }

    public String getmHyperLink() {
        return mHyperLink;
    }

    public void setmHyperLink(String mHyperLink) {
        this.mHyperLink = mHyperLink;
    }

    public String getmProgressBarCaption() {
        return mProgressBarCaption;
    }

    public void setmProgressBarCaption(String mProgressBarCaption) {
        this.mProgressBarCaption = mProgressBarCaption;
    }

    public int getmButtonType() {
        return mButtonType;
    }

    public void setmButtonType(int mButtonType) {
        this.mButtonType = mButtonType;
    }

    public long getRequiredSpaceForUpdate() {
        return mRequiredSpaceForUpdate;
    }

    public void setRequiredSpaceForUpdate(long required_space_for_update) {
        this.mRequiredSpaceForUpdate = required_space_for_update;
    }

    public long getRequiredSpaceForDelete() {
        return mRequiredSpaceForDelete;
    }

    public void setRequiredSpaceForDelete(long required_space_for_delete) {
        this.mRequiredSpaceForDelete = required_space_for_delete;
    }
}
