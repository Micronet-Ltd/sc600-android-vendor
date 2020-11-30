/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

#include "./pal_test_engine.h"

static __pthread_sem_t pthread_sem;

static void __ui_cb(void* data)
{
    UNUSED(data);
    printf("%s\n", __FUNCTION__);
    int sem_rc = __pthread_sem_notify(&pthread_sem);
}

static void __print_help()
{
    printf("pal_fumo_dlg_test requires params:\n");
    printf("[0] int state {10:idle, 20:dl.failed, }\n");
    printf("[1] int msg_type {0:popup, 1:notification}\n");
    printf("[2] int msg_mode {0:normal, 1:persistent}\n");
    printf("[3] int icon_type {0:node, 1:devices}\n");
    printf("[4] char* title\n");
    printf("[5] char* message\n");
    printf("[6] char* progress_bar_caption\n");
    printf("[7] int button_flags\n");
    printf("[8] char* install_param\n");
    printf("[9] int wait_time\n");
    printf("[10] char* hyper_link_caption\n");
    printf("[11] char* hyper_link\n");
}

int pal_fumo_dlg_test(pal_int_dlgui_fn pfn, int argc, char** argv)
{
    if (NULL == pfn) {
        printf("pfn is null\n");
        return RESULT_ERROR_RESOURCE_IS_NOT_AVAILABLE;
    }
    int rc = RESULT_SUCCESS;
    pal_dialog_ui_t ui;
    if (argc < 11) {
        __print_help();
        return RESULT_ERROR_INVALID_ARGS;
    }
    int sem_rc = __pthread_sem_init(&pthread_sem);
    BZEROTYPE(ui);
    int wait_time = 10;
    if (argc >  0) ui.state = atoi(argv[0]);
    if (argc >  1) ui.message_type = atoi(argv[1]);
    if (argc >  2) ui.message_mode = atoi(argv[2]);
    if (argc >  3) ui.icon_type = atoi(argv[3]);
    if (argc >  4) ui.header_text = argv[4];
    if (argc >  5) ui.message_text = argv[5];
    if (argc >  6) ui.progress_bar_caption = argv[6];
    if (argc >  7) ui.button_type = strtol(argv[7], NULL, 16);
    if (argc >  8) ui.install_param = argv[9];
    if (argc >  9) wait_time = atoi(argv[10]);
    if (argc > 10) {
        ui.hyper_link_caption = argv[11];
    } else {
        ui.hyper_link_caption = strdup("");
    }
    if (argc > 12) {
        ui.hyper_link = argv[12];
    } else {
        ui.hyper_link = strdup("");
    }
    ui.user_reaction = __ui_cb;
    rc = pal_init();
    if (rc != RESULT_SUCCESS) {
        printf("pal_init() = %d\n", rc);
        return rc;
    }
    rc = pfn(&ui);
    if (sem_rc < 0) {
        printf("sem_init():failed");
    } else {
        struct timespec ts;
        __WRN_NE(clock_gettime(CLOCK_REALTIME, &ts), 0);
        ts.tv_sec += wait_time;
        ts.tv_nsec = 0;
        sem_rc = __pthread_sem_timedwait(&pthread_sem, &ts);
        if (sem_rc < 0) {
            printf("sem_timedwait : failed: use sleep\n");
            sleep(wait_time);
        }
    }
    __pthread_sem_deinit(&pthread_sem);
    //pal_fini();
    return rc;
}

static const pal_func_description_t __fumo_funcs[] = {
    {"dialog", "pal_fumo_gui_create_message", __PAL_ARGS_FUMODLG,
        "(pal_fumo_gui_message_descriptor*)"},
    {NULL, NULL, -1, NULL},
};


int pal_fumo_test(int argc, char** argv)
{
    return pal_test_subsection(argc, argv, (pal_func_description_t*)__fumo_funcs);
}
