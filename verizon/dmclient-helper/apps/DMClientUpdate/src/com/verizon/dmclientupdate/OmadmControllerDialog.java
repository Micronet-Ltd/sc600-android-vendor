/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;


import android.app.Activity;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AutoCompleteTextView;

import java.util.Arrays;

import android.omadm.OmadmControllerGuiMessageDescriptor;
import android.omadm.OmadmManager;
import com.verizon.dmclientupdate.R;

public class OmadmControllerDialog extends Activity {

    public final static String TAG = "OmadmControllerDialog";

    public final static String DMCLT_UI_TYPE_DISPLAY_TAG            = "dmclt_display_dialog";
    public final static String DMCLT_UI_TYPE_CONFIRM_TAG            = "dmclt_confirm_dialog";
    public final static String DMCLT_UI_TYPE_USER_INPUT_TAG         = "dmclt_user_input_dialog";
    public final static String DMCLT_UI_TYPE_USER_CHOICE_TAG        = "dmclt_user_choice_dialog";
    public final static String DMCLT_UI_TYPE_USER_MUILTI_CHOICE_TAG = "dmclt_user_multichoice_dialog";
    public final static String DMCLT_UI_TYPE_DISPLAY_UIE_TAG        = "dmclt_display_uie_dialog";

    public final static String DIALOG_TITLE   = "dialog_title";
    public final static String DIALOG_MESSAGE = "dialog_message";
    public final static String DIALOG_CHOICES = "dialog_choices";

    /**
     * User reaction.
     */
    public final static int OMADM_USER_CONFIRM       = 1;
    public final static int OMADM_USER_DECLINE       = 0;

    /**
     * The definitions for Omadm Controller user dialog Type
     * Should be in sync with Omadm dmclt_ui_type_t enum.
     */
    public final static int DMCLT_UI_TYPE_DISPLAY          = 1;
    public final static int DMCLT_UI_TYPE_CONFIRM          = 2;
    public final static int DMCLT_UI_TYPE_USER_INPUT       = 3;
    public final static int DMCLT_UI_TYPE_USER_CHOICE      = 4;
    public final static int DMCLT_UI_TYPE_USER_MULTICHOICE = 5;

    /**
     * This is definitions for special Omadm Controller user display dialogs Type
     * with predefined text resources for dialog title and body.
     */
    // User Initiated Update Error Condition display type dialog
    public final static int DMCLT_UI_TYPE_DISPLAY_UIE      = 6;

    /**
     * The definitions for Omadm Controller user input type
     * Should be in sync with Omadm dmclt_ui_input_t enum.
     */
    public final static int DMCLT_UI_INPUT_ALPHA = 1;
    public final static int DMCLT_UI_INPUT_NUM   = 2;
    public final static int DMCLT_UI_INPUT_DATE  = 3;
    public final static int DMCLT_UI_INPUT_TIME  = 4;
    public final static int DMCLT_UI_INPUT_PHONE = 5;
    public final static int DMCLT_UI_INPUT_IP    = 6;

    /**
     * The definitions for Omadm Controller user echo type
     * Should be in sync with Omadm dmclt_ui_echo_t enum.
     */
    public final static int DMCLT_UI_ECHO_TEXT   = 1;
    public final static int DMCLT_UI_ECHO_PASSWD = 2;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();

        handleIntent(intent);
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);

        handleIntent(intent);
    }

    private void handleIntent(Intent intent) {
        Bundle extras = intent.getExtras();

        OmadmControllerGuiMessageDescriptor ocgMessageDecriptior =
            (OmadmControllerGuiMessageDescriptor) extras.get(OmadmManager.OMADM_CONTROLLER_MESSAGE_DESCRIPTOR);

        if (ocgMessageDecriptior == null) {
            Log.e(TAG, "received ACTION_OMADM_CONTROLLER_SHOW_UI with no message descriptor extra");
            finish();
        }

        showOmadmControllerDialog(ocgMessageDecriptior);
    }

    // Implementation of DMCLT_UI_TYPE_DISPLAY dialog type
    public static class OmadmControllerDisplayDialog extends DialogFragment {
        private static OmadmControllerGuiMessageDescriptor mOcgmd;
        public static OmadmControllerDisplayDialog newInstance(OmadmControllerGuiMessageDescriptor ocgmd) {
            mOcgmd = ocgmd;
            final OmadmControllerDisplayDialog dialog_fragment = new OmadmControllerDisplayDialog();
            final Bundle args = new Bundle();
            args.putInt(DIALOG_TITLE, R.string.system_update_omadm_controller_display_dialog_title);
            args.putString(DIALOG_MESSAGE, mOcgmd.mDispMsg);
            dialog_fragment.setArguments(args);
            return dialog_fragment;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int title = getArguments().getInt(DIALOG_TITLE);
            final String message = getArguments().getString(DIALOG_MESSAGE);
            final OmadmControllerAlertDialog builder = new OmadmControllerAlertDialog(getActivity(), title,
                    DMCLT_UI_TYPE_DISPLAY) {
                @Override
                public void doPositiveClick() {
                    // Do nothing we do not expect any user reaction except close dialog for this dialog type
                }
            };
            builder.setMessage(message);
            final Dialog dialog = builder.create();
            dialog.setOnShowListener(builder);
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onDetach() {
            super.onDetach();
            ((OmadmControllerDialog) getActivity()).finish();
        }
    }

    // Implementation of DMCLT_UI_TYPE_CONFIRM dialog type
    public static class OmadmControllerConfirmDialog extends DialogFragment {
        private static OmadmControllerGuiMessageDescriptor mOcgmd;
        public static OmadmControllerConfirmDialog newInstance(OmadmControllerGuiMessageDescriptor ocgmd) {
            mOcgmd = ocgmd;
            final OmadmControllerConfirmDialog dialog_fragment = new OmadmControllerConfirmDialog();
            final Bundle args = new Bundle();
            args.putInt(DIALOG_TITLE, R.string.system_update_omadm_controller_confirm_dialog_title);
            args.putString(DIALOG_MESSAGE, mOcgmd.mDispMsg);
            dialog_fragment.setArguments(args);
            return dialog_fragment;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int title = getArguments().getInt(DIALOG_TITLE);
            final String message = getArguments().getString(DIALOG_MESSAGE);
            final OmadmControllerAlertDialog builder = new OmadmControllerAlertDialog(getActivity(), title,
                    DMCLT_UI_TYPE_CONFIRM) {
                @Override
                public void doPositiveClick() {
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                            DMCLT_UI_TYPE_CONFIRM, null, 0, null, OMADM_USER_CONFIRM);
                }

                @Override
                public void doNegativeClick() {
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                            DMCLT_UI_TYPE_CONFIRM, null, 0, null, OMADM_USER_DECLINE);
                }
            };
            builder.setMessage(message);
            final Dialog dialog = builder.create();
            dialog.setOnShowListener(builder);
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onDetach() {
            super.onDetach();
            ((OmadmControllerDialog) getActivity()).finish();
        }
    }

    // Implementation of DMCLT_UI_TYPE_USER_INPUT dialog type
    public static class OmadmControllerUserInputDialog extends DialogFragment {
        private static OmadmControllerGuiMessageDescriptor mOcgmd;
        public static OmadmControllerUserInputDialog newInstance(OmadmControllerGuiMessageDescriptor ocgmd) {
            mOcgmd = ocgmd;
            final OmadmControllerUserInputDialog dialog_fragment =
                new OmadmControllerUserInputDialog();
            final Bundle args = new Bundle();
            args.putInt(DIALOG_TITLE, R.string.system_update_omadm_controller_input_dialog_title);
            dialog_fragment.setArguments(args);
            return dialog_fragment;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int title = getArguments().getInt(DIALOG_TITLE);
            View viewInflated = LayoutInflater.from(getContext()).inflate(R.layout.omadm_controller_text_input, (ViewGroup) getView(), false);
            final AutoCompleteTextView input_view = (AutoCompleteTextView) viewInflated.findViewById(R.id.input);
            int input_type_flag;
            switch (mOcgmd.mInputType) {
                case DMCLT_UI_INPUT_ALPHA:
                    input_type_flag = InputType.TYPE_CLASS_TEXT;
                    break;
                case DMCLT_UI_INPUT_NUM:
                    input_type_flag = InputType.TYPE_CLASS_NUMBER;
                    break;
                case DMCLT_UI_INPUT_DATE:
                    input_type_flag = InputType.TYPE_CLASS_DATETIME |
                                      InputType.TYPE_DATETIME_VARIATION_DATE;
                    break;
                case DMCLT_UI_INPUT_TIME:
                    input_type_flag = InputType.TYPE_CLASS_DATETIME |
                                      InputType.TYPE_DATETIME_VARIATION_TIME;
                    break;
                case DMCLT_UI_INPUT_PHONE:
                    input_type_flag = InputType.TYPE_CLASS_PHONE;
                    break;
                case DMCLT_UI_INPUT_IP:
                    input_type_flag = InputType.TYPE_CLASS_NUMBER |
                                      InputType.TYPE_NUMBER_FLAG_DECIMAL;
                    break;
                default:
                    input_type_flag = InputType.TYPE_CLASS_TEXT;
                    break;
            }
            if (mOcgmd.mEchoType == DMCLT_UI_ECHO_PASSWD) {
                input_type_flag |= InputType.TYPE_TEXT_VARIATION_PASSWORD;
            }
            input_view.setInputType(input_type_flag);
            input_view.setHint(mOcgmd.mDispMsg);
            final OmadmControllerAlertDialog builder = new OmadmControllerAlertDialog(getActivity(), title,
                    DMCLT_UI_TYPE_USER_INPUT) {
                @Override
                public void doPositiveClick() {
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                            DMCLT_UI_TYPE_USER_INPUT, input_view.getText().toString(),
                            0, null, OMADM_USER_CONFIRM);
                }

                @Override
                public void doNegativeClick() {
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                            DMCLT_UI_TYPE_USER_INPUT, null, 0, null, OMADM_USER_DECLINE);
                }
            };
            builder.setView(viewInflated);
            final Dialog dialog = builder.create();
            dialog.setOnShowListener(builder);
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onDetach() {
            super.onDetach();
            ((OmadmControllerDialog) getActivity()).finish();
        }
    }

    // Implementation of DMCLT_UI_TYPE_USER_CHOICE dialog type
    public static class OmadmControllerUserChoiceDialog extends DialogFragment {
        private static OmadmControllerGuiMessageDescriptor mOcgmd;
        public static OmadmControllerUserChoiceDialog newInstance(OmadmControllerGuiMessageDescriptor ocgmd) {
            mOcgmd = ocgmd;
            final OmadmControllerUserChoiceDialog input_dialog =
                new OmadmControllerUserChoiceDialog();
            final Bundle args = new Bundle();
            args.putInt(DIALOG_TITLE, R.string.system_update_omadm_controller_user_choice_dialog_title);
            args.putStringArray(DIALOG_CHOICES, mOcgmd.mChoices);
            input_dialog.setArguments(args);
            return input_dialog;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int title = getArguments().getInt(DIALOG_TITLE);
            final String[] choices = getArguments().getStringArray(DIALOG_CHOICES);
            final OmadmControllerAlertDialog builder = new OmadmControllerAlertDialog(getActivity(), title,
                    DMCLT_UI_TYPE_USER_CHOICE) {
                @Override
                public void doNegativeClick() {
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                            DMCLT_UI_TYPE_USER_CHOICE, null, 0, null, OMADM_USER_DECLINE);
                }
            };
            builder.setItems(choices,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int selected) {
                            if (selected >= 0 && selected < mOcgmd.mChoices.length) {
                                ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                                    DMCLT_UI_TYPE_USER_CHOICE, null, 1, mOcgmd.mChoices[selected], OMADM_USER_CONFIRM);
                            }
                        }
                    });
            final Dialog dialog = builder.create();
            dialog.setOnShowListener(builder);
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onDetach() {
            super.onDetach();
            ((OmadmControllerDialog) getActivity()).finish();
        }
    }

    // Implementation of DMCLT_UI_TYPE_USER_MULTICHOICE dialog type
    public static class OmadmControllerUserMultiChoiceDialog extends DialogFragment {
        private static OmadmControllerGuiMessageDescriptor mOcgmd;
        private boolean[] mOmadmSelectedItems;
        private int selected_choices_count = 0;
        public static OmadmControllerUserMultiChoiceDialog newInstance(OmadmControllerGuiMessageDescriptor ocgmd) {
            mOcgmd = ocgmd;
            final OmadmControllerUserMultiChoiceDialog dialog_fragment =
                new OmadmControllerUserMultiChoiceDialog();
            final Bundle args = new Bundle();
            args.putInt(DIALOG_TITLE, R.string.system_update_omadm_controller_user_choice_dialog_title);
            args.putStringArray(DIALOG_CHOICES, mOcgmd.mChoices);
            dialog_fragment.setArguments(args);
            return dialog_fragment;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            mOmadmSelectedItems = new boolean[mOcgmd.mChoices.length];
            final StringBuilder selected_choices = new StringBuilder();
            Arrays.fill(mOmadmSelectedItems, Boolean.FALSE);
            final int title = getArguments().getInt(DIALOG_TITLE);
            final String[] choices = getArguments().getStringArray(DIALOG_CHOICES);
            final OmadmControllerAlertDialog builder = new OmadmControllerAlertDialog(getActivity(), title,
                    DMCLT_UI_TYPE_USER_MULTICHOICE) {
                @Override
                public void doPositiveClick() {
                    int i = 0;
                    for (boolean enable : mOmadmSelectedItems) {
                        if (enable) {
                            selected_choices.append(mOcgmd.mChoices[i]);
                            selected_choices.append(OmadmManager.OMADM_DELIMITER);
                            selected_choices_count++;
                        }
                        i++;
                    }
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                        DMCLT_UI_TYPE_USER_MULTICHOICE, null, selected_choices_count,
                        selected_choices.toString(), OMADM_USER_CONFIRM);
                }

                @Override
                public void doNegativeClick() {
                    ((OmadmControllerDialog) getActivity()).sendUserReplyToOmadmController(
                            DMCLT_UI_TYPE_USER_MULTICHOICE, null, 0, null, OMADM_USER_DECLINE);
                }
            };
            builder.setMultiChoiceItems(choices, mOmadmSelectedItems,
                    new DialogInterface.OnMultiChoiceClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                            mOmadmSelectedItems[which] = isChecked;
                        }
                    });
            final Dialog dialog = builder.create();
            dialog.setOnShowListener(builder);
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onDetach() {
            super.onDetach();
            ((OmadmControllerDialog) getActivity()).finish();
        }
    }

    // Implementation of DMCLT_UI_TYPE_DISPLAY_UIE dialog type
    public static class OmadmControllerDisplayUIEDialog extends DialogFragment {
        private static OmadmControllerGuiMessageDescriptor mOcgmd;
        public static OmadmControllerDisplayUIEDialog newInstance(OmadmControllerGuiMessageDescriptor ocgmd) {
            mOcgmd = ocgmd;
            final OmadmControllerDisplayUIEDialog dialog_fragment = new OmadmControllerDisplayUIEDialog();
            final Bundle args = new Bundle();
            args.putInt(DIALOG_TITLE, R.string.system_update_omadm_controller_display_uie_dialog_title);
            args.putInt(DIALOG_MESSAGE, R.string.system_update_omadm_controller_display_uie_dialog_body);
            dialog_fragment.setArguments(args);
            return dialog_fragment;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final int title = getArguments().getInt(DIALOG_TITLE);
            final int message = getArguments().getInt(DIALOG_MESSAGE);
            final OmadmControllerAlertDialog builder = new OmadmControllerAlertDialog(getActivity(), title,
                    DMCLT_UI_TYPE_DISPLAY_UIE) {
                @Override
                public void doPositiveClick() {
                    // Do nothing we do not expect any user reaction except close dialog for this dialog type
                }
            };
            builder.setMessage(message);
            final Dialog dialog = builder.create();
            dialog.setOnShowListener(builder);
            dialog.setCanceledOnTouchOutside(false);
            return dialog;
        }

        @Override
        public void onDetach() {
            super.onDetach();
            ((OmadmControllerDialog) getActivity()).finish();
        }
    }

    private void showOmadmControllerDialog(OmadmControllerGuiMessageDescriptor ocgmd) {
        switch (ocgmd.mType) {
            case DMCLT_UI_TYPE_DISPLAY:
                 OmadmControllerDisplayDialog.newInstance(ocgmd).show(
                         getFragmentManager(), DMCLT_UI_TYPE_DISPLAY_TAG);
                break;
            case DMCLT_UI_TYPE_CONFIRM:
                 OmadmControllerConfirmDialog.newInstance(ocgmd).show(
                         getFragmentManager(), DMCLT_UI_TYPE_CONFIRM_TAG);
                break;
            case DMCLT_UI_TYPE_USER_INPUT:
                 OmadmControllerUserInputDialog.newInstance(ocgmd).show(
                         getFragmentManager(), DMCLT_UI_TYPE_USER_INPUT_TAG);
                break;
            case DMCLT_UI_TYPE_USER_CHOICE:
                OmadmControllerUserChoiceDialog.newInstance(ocgmd).show(
                        getFragmentManager(), DMCLT_UI_TYPE_USER_CHOICE_TAG);
               break;
            case DMCLT_UI_TYPE_USER_MULTICHOICE:
                OmadmControllerUserMultiChoiceDialog.newInstance(ocgmd).show(
                        getFragmentManager(), DMCLT_UI_TYPE_USER_MUILTI_CHOICE_TAG);
               break;
            case DMCLT_UI_TYPE_DISPLAY_UIE:
                 OmadmControllerDisplayUIEDialog.newInstance(ocgmd).show(
                         getFragmentManager(), DMCLT_UI_TYPE_DISPLAY_UIE_TAG);
                break;
            default:
                break;
        }
    }

    public void sendUserReplyToOmadmController(
            int type, String input_text, int selected_choices_count,
            String selected_choices, int button_id) {
        Utils.getInstance(this).sendUserReplyToOmadmController(
            type, input_text, selected_choices_count,
            selected_choices, button_id);
    }
}
