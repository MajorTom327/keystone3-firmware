/**************************************************************************************************
 * Copyright (c) keyst.one. 2020-2025. All rights reserved.
 * Description: Passphrase input UI widgets.
 * Author: leon sun
 * Create: 2023-6-8
 ************************************************************************************************/

#include "gui.h"
#include "gui_passphrase_widgets.h"
#include "gui_status_bar.h"
#include "gui_chain.h"
#include "gui_views.h"
#include "gui_hintbox.h"
#include "gui_keyboard.h"
#include "gui_enter_passcode.h"
#include "gui_lock_widgets.h"
#include "gui_model.h"
#include "stdio.h"
#include "secret_cache.h"
#include "keystore.h"
#include "gui_keyboard.h"
#include "motor_manager.h"
#include "gui_page.h"

typedef struct {
    lv_obj_t *cont;
    lv_obj_t *passphraseInputCont;
    KeyBoard_t *keyboard;
    lv_obj_t *inputTa;
    lv_obj_t *repeatTa;
    lv_obj_t *errLabel;
    lv_obj_t *lenOverLabel;
} PassphraseWidgets_t;

typedef struct ContLabelWidget_t {
    lv_obj_t *label;
    lv_obj_t *cont;
} ContLabelWidget_t;


static void SkipHandler(lv_event_t *e);
static void SetKeyboardTaHandler(lv_event_t *e);
static void UpdatePassPhraseHandler(lv_event_t *e);

static PassphraseWidgets_t g_passphraseWidgets;
static ContLabelWidget_t g_waitAnimWidget;
static PageWidget_t *g_pageWidget;

void GuiPassphraseInit(void)
{
    lv_obj_t *label, *btn, *ta, *img, *line, *baffle;
    static lv_point_t points[2] =  {{0, 0}, {408, 0}};

    g_pageWidget = CreatePageWidget();
    g_passphraseWidgets.cont = g_pageWidget->contentZone;
    g_passphraseWidgets.passphraseInputCont = GuiCreateHintBox(g_passphraseWidgets.cont, 480, 614, false);
    label = GuiCreateIllustrateLabel(g_passphraseWidgets.passphraseInputCont, "Passphrase");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 36, 216);
    lv_obj_set_style_text_opa(label, LV_OPA_70, LV_PART_MAIN);
    btn = GuiCreateBtn(g_passphraseWidgets.passphraseInputCont, "Skip");
    lv_obj_set_size(btn, 63, 42);
    lv_obj_set_style_radius(btn, 15, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, WHITE_COLOR_OPA20, LV_PART_MAIN);
    label = lv_obj_get_child(btn, 0);
    lv_obj_set_style_text_font(label, g_defIllustrateFont, LV_PART_MAIN);
    lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -36, 210);
    lv_obj_add_event_cb(btn, SkipHandler, LV_EVENT_CLICKED, NULL);

    btn = lv_btn_create(g_passphraseWidgets.passphraseInputCont);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_size(btn, 352, 60);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 36, 294);
    ta = lv_textarea_create(btn);
    lv_obj_set_align(ta, LV_ALIGN_CENTER);
    lv_obj_set_size(ta, 352, 60);
    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, _("Input passphrase"));
    lv_obj_set_style_bg_color(ta, DARK_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_color(ta, WHITE_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_opa(ta, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_text_font(ta, &openSans_24, LV_PART_MAIN);
    lv_obj_add_event_cb(ta, SetKeyboardTaHandler, LV_EVENT_ALL, ta);
    lv_textarea_set_max_length(ta, GUI_DEFINE_MAX_PASSCODE_LEN);
    lv_textarea_set_one_line(ta, true);
    lv_obj_set_scrollbar_mode(ta, LV_SCROLLBAR_MODE_OFF);
    img = GuiCreateImg(g_passphraseWidgets.passphraseInputCont, &imgEyeOff);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 408, 294);
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img, SwitchPasswordModeHandler, LV_EVENT_CLICKED, ta);
    lv_obj_add_event_cb(img, NULL, LV_EVENT_CLICKED, ta);
    g_passphraseWidgets.inputTa = ta;
    line = GuiCreateLine(g_passphraseWidgets.passphraseInputCont, points, 2);
    lv_obj_align(line, LV_ALIGN_TOP_MID, 0, 348);

    btn = lv_btn_create(g_passphraseWidgets.passphraseInputCont);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_outline_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_size(btn, 352, 60);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 36, 378);
    ta = lv_textarea_create(btn);
    lv_obj_set_align(ta, LV_ALIGN_CENTER);
    lv_obj_set_size(ta, 352, 60);
    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, _("Repeat passphrase"));
    lv_obj_set_style_bg_color(ta, DARK_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_color(ta, WHITE_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_opa(ta, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_text_font(ta, &openSans_24, LV_PART_MAIN);
    lv_textarea_set_max_length(ta, GUI_DEFINE_MAX_PASSCODE_LEN);
    lv_textarea_set_one_line(ta, true);
    lv_obj_set_scrollbar_mode(ta, LV_SCROLLBAR_MODE_OFF);

    lv_obj_add_event_cb(ta, SetKeyboardTaHandler, LV_EVENT_ALL, ta);
    img = GuiCreateImg(g_passphraseWidgets.passphraseInputCont, &imgEyeOff);
    lv_obj_align(img, LV_ALIGN_TOP_LEFT, 408, 378);
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img, SwitchPasswordModeHandler, LV_EVENT_CLICKED, ta);
    lv_obj_add_event_cb(img, NULL, LV_EVENT_CLICKED, ta);
    lv_textarea_set_max_length(ta, GUI_DEFINE_MAX_PASSCODE_LEN);
    lv_textarea_set_one_line(ta, true);
    g_passphraseWidgets.repeatTa = ta;
    line = GuiCreateLine(g_passphraseWidgets.passphraseInputCont, points, 2);
    lv_obj_align(line, LV_ALIGN_TOP_MID, 0, 432);

    g_passphraseWidgets.keyboard = GuiCreateFullKeyBoard(g_passphraseWidgets.passphraseInputCont, UpdatePassPhraseHandler, KEY_STONE_FULL_L, NULL);
    GuiSetKeyBoardMinTaLen(g_passphraseWidgets.keyboard, 0);
    GuiSetFullKeyBoardTa(g_passphraseWidgets.keyboard, g_passphraseWidgets.inputTa);

    label = GuiCreateIllustrateLabel(g_passphraseWidgets.passphraseInputCont, _("Passphrase does not match"));
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 36, 436);
    lv_obj_set_style_text_color(label, RED_COLOR, LV_PART_MAIN);
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    g_passphraseWidgets.errLabel = label;

    label = GuiCreateIllustrateLabel(g_passphraseWidgets.passphraseInputCont, _("input length cannot exceed 128 characters"));
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 436);
    lv_obj_set_style_text_color(label, RED_COLOR, LV_PART_MAIN);
    lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    g_passphraseWidgets.lenOverLabel = label;

    baffle = GuiCreateContainerWithParent(g_passphraseWidgets.passphraseInputCont, 480, 800 - 614);
    lv_obj_align(baffle, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(baffle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_opa(baffle, LV_OPA_0, LV_PART_MAIN);
}


void GuiPassphraseDeInit(void)
{
    GUI_DEL_OBJ(g_passphraseWidgets.passphraseInputCont)
    GUI_DEL_OBJ(g_passphraseWidgets.cont)
    printf("GuiPassphraseDeInit!\r\n");
    CLEAR_OBJECT(g_passphraseWidgets);
    if (g_pageWidget != NULL) {
        DestroyPageWidget(g_pageWidget);
        g_pageWidget = NULL;
    }
}


void GuiPassphraseRefresh(void)
{
    SetNavBarLeftBtn(g_pageWidget->navBarWidget, NVS_BAR_RETURN, ReturnHandler, NULL);
    SetMidBtnLabel(g_pageWidget->navBarWidget, NVS_BAR_MID_LABEL, _("wallet_setting_passphrase"));
}


void GuiPassphrasePrevTile(void)
{

}


void GuiPassphraseDone(void)
{
    printf("GuiPassphraseDone\n");
    GuiDeleteAnimHintBox();
    g_waitAnimWidget.cont = NULL;
    GuiCLoseCurrentWorkingView();
    GuiLockScreenHidden();
    if (g_homeView.isActive) {
        GuiLockScreenTurnOff();
    } else {
        GuiFrameOpenView(&g_homeView);
    }
}

static void SkipHandler(lv_event_t *e)
{
#ifndef COMPILE_SIMULATOR
    SetPassphraseMark(false);
#endif
    GuiCLoseCurrentWorkingView();
    GuiLockScreenHidden();
    if (g_homeView.isActive) {
        GuiLockScreenTurnOff();
    } else {
        GuiFrameOpenView(&g_homeView);
    }
}


static void SetKeyboardTaHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t *ta = lv_event_get_user_data(e);
        if (ta == g_passphraseWidgets.repeatTa) {
            GuiSetFullKeyBoardConfirm(g_passphraseWidgets.keyboard, true);
        } else {
            GuiSetFullKeyBoardConfirm(g_passphraseWidgets.keyboard, false);
        }
        GuiSetFullKeyBoardTa(g_passphraseWidgets.keyboard, ta);

        lv_obj_add_flag(g_passphraseWidgets.lenOverLabel, LV_OBJ_FLAG_HIDDEN);
    } else if (code == KEY_STONE_KEYBOARD_CHANGE) {
        lv_keyboard_user_mode_t *keyMode = lv_event_get_param(e);
        g_passphraseWidgets.keyboard->mode = *keyMode;
        GuiKeyBoardSetMode(g_passphraseWidgets.keyboard);
        if (*keyMode == KEY_STONE_SYMBOL) {
            GuiUpdatePassPhraseKb(g_passphraseWidgets.keyboard);
        }
    }
}


static void UpdatePassPhraseHandler(lv_event_t *e)
{
    static bool delayFlag = false;
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_READY) {
        if (lv_keyboard_get_textarea(g_passphraseWidgets.keyboard->kb) == g_passphraseWidgets.inputTa) {
            lv_keyboard_set_textarea(g_passphraseWidgets.keyboard->kb, g_passphraseWidgets.repeatTa);
        } else {
            const char *input = lv_textarea_get_text(g_passphraseWidgets.inputTa);
            const char *repeat = lv_textarea_get_text(g_passphraseWidgets.repeatTa);
            if (!strcmp(input, repeat)) {
                SecretCacheSetPassphrase((char *)repeat);
                g_waitAnimWidget.cont = GuiCreateAnimHintBox(lv_scr_act(), 480, 278, 82);
                g_waitAnimWidget.label = GuiCreateTextLabel(g_waitAnimWidget.cont, _("seed_check_wait_verify"));
                lv_obj_align(g_waitAnimWidget.label, LV_ALIGN_BOTTOM_MID, 0, -76);
                GuiModelSettingWritePassphrase();
            } else {
                delayFlag = true;
                lv_obj_clear_flag(g_passphraseWidgets.errLabel, LV_OBJ_FLAG_HIDDEN);
            }
        }
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        const char *intputText = lv_textarea_get_text(g_passphraseWidgets.inputTa);
        if (!lv_obj_has_flag(g_passphraseWidgets.errLabel, LV_OBJ_FLAG_HIDDEN)) {
            if (delayFlag == true) {
                delayFlag = false;
            } else {
                lv_obj_add_flag(g_passphraseWidgets.errLabel, LV_OBJ_FLAG_HIDDEN);
            }
        }

        if (lv_keyboard_get_textarea(lv_event_get_target(e)) == g_passphraseWidgets.inputTa) {
            if (strlen(intputText) >= GUI_DEFINE_MAX_PASSCODE_LEN) {
                lv_obj_clear_flag(g_passphraseWidgets.lenOverLabel, LV_OBJ_FLAG_HIDDEN);
            } else if (strlen(intputText) < GUI_DEFINE_MAX_PASSCODE_LEN) {
                lv_obj_add_flag(g_passphraseWidgets.lenOverLabel, LV_OBJ_FLAG_HIDDEN);
            }
        }
        Vibrate(SLIGHT);
    }
}


