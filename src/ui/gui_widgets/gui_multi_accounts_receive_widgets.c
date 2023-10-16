#include "gui_multi_accounts_receive_widgets.h"
#include "gui_status_bar.h"
#include "gui_chain.h"
#include "gui_views.h"
#include "gui_hintbox.h"
#include "account_public_info.h"
#include "librust_c.h"
#include "assert.h"
#include "gui_keyboard.h"
#include "draw/lv_draw_mask.h"
#include "stdio.h"
#include "user_utils.h"
#include "inttypes.h"
#include "gui_home_widgets.h"
#include "gui_fullscreen_mode.h"
#include "keystore.h"
#include "gui_page.h"
#include "gui.h"
#include "gui_tutorial_widgets.h"
#include "account_manager.h"

#define GENERAL_ADDRESS_INDEX_MAX 999999999
#define ACCOUNT_INDEX_MAX 24

typedef enum
{
    RECEIVE_TILE_QRCODE = 0,
    RECEIVE_TILE_SWITCH_ACCOUNT,

    RECEIVE_TILE_BUTT,
} MultiAccountsReceiveTile;

typedef struct
{
    lv_obj_t *addressCountLabel;
    lv_obj_t *addressLabel;
    lv_obj_t *checkBox;
    lv_obj_t *checkedImg;
    lv_obj_t *uncheckedImg;
} SwitchAddressWidgetsItem_t;

typedef struct
{
    lv_obj_t *checkBox;
    lv_obj_t *checkedImg;
    lv_obj_t *uncheckedImg;
} PathWidgetsItem_t;

typedef struct
{
    lv_obj_t *cont;
    lv_obj_t *tileView;
    lv_obj_t *tileQrCode;
    lv_obj_t *tileSwitchAccount;
    lv_obj_t *attentionCont;
    lv_obj_t *qrCodeCont;
    lv_obj_t *qrCode;
    lv_obj_t *addressLabel;
    lv_obj_t *addressCountLabel;
    lv_obj_t *moreCont;
    lv_obj_t *addressButton;
    lv_obj_t *leftBtnImg;
    lv_obj_t *rightBtnImg;
    lv_obj_t *inputAccountLabel;
    lv_obj_t *overflowLabel;
    lv_obj_t *inputAccountCont;
    lv_obj_t *inputAccountKeyboard;
    SwitchAddressWidgetsItem_t switchAddressWidgets[5];
} MultiAccountsReceiveWidgets_t;

typedef struct
{
    uint32_t index;
    char address[128];
    char path[32];
} AddressDataItem_t;

typedef struct
{
    char title[32];
    char subTitle[32];
    char path[32];
} PathItem_t;

static void GuiCreateMoreWidgets(lv_obj_t *parent);
static void GuiMultiAccountsReceiveGotoTile(MultiAccountsReceiveTile tile);
static void GuiCreateQrCodeWidget(lv_obj_t *parent);
static void GuiCreateSwitchAccountWidget(lv_obj_t *parent);
static void GuiCreateSwitchAccountButtons(lv_obj_t *parent);
static void GuiCreateInputAccountWidgets(lv_obj_t *parent);

static void RefreshQrCode(void);
static void RefreshSwitchAddress(void);
static int GetMaxAddressIndex(void);
static void GetAttentionText(char* text);
static void SetKeyboardValid(bool);

static void CloseAttentionHandler(lv_event_t *e);
static void MoreHandler(lv_event_t *e);
static void TutorialHandler(lv_event_t *e);
static void SwitchAccountHandler(lv_event_t *e);
static void InputAccountKeyboardHandler(lv_event_t *e);
static void LeftBtnHandler(lv_event_t *e);
static void RightBtnHandler(lv_event_t *e);
static bool IsAddressSwitchable();
static bool HasMoreBtn();
static void SwitchAddressHandler(lv_event_t *e);
static void SelectAddressHandler(lv_event_t *e);
static void AddressLongModeCut(char *out, const char *address, uint32_t targetLen);

static void ModelGetAddress(uint32_t index, AddressDataItem_t *item);

static MultiAccountsReceiveWidgets_t g_multiAccountsReceiveWidgets;
static MultiAccountsReceiveTile g_multiAccountsReceiveTileNow;
static HOME_WALLET_CARD_ENUM g_chainCard;

// to do: stored.
static uint32_t g_showIndex;
static uint32_t g_selectIndex[3] = {0};
static uint32_t g_selectedAccount[3] = {0};
static PageWidget_t *g_pageWidget;
static bool g_inputAccountValid = true;

void GuiMultiAccountsReceiveInit(uint8_t chain)
{
    g_chainCard = chain;
    g_pageWidget = CreatePageWidget();
    g_multiAccountsReceiveWidgets.cont = g_pageWidget->contentZone;
    g_multiAccountsReceiveWidgets.tileView = lv_tileview_create(g_multiAccountsReceiveWidgets.cont);
    lv_obj_set_style_bg_opa(g_multiAccountsReceiveWidgets.tileView, LV_OPA_0, LV_PART_SCROLLBAR & LV_STATE_SCROLLED);
    lv_obj_set_style_bg_opa(g_multiAccountsReceiveWidgets.tileView, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    g_multiAccountsReceiveWidgets.tileQrCode = lv_tileview_add_tile(g_multiAccountsReceiveWidgets.tileView, RECEIVE_TILE_QRCODE, 0, LV_DIR_HOR);
    GuiCreateQrCodeWidget(g_multiAccountsReceiveWidgets.tileQrCode);
    if (IsAddressSwitchable())
    {
        g_multiAccountsReceiveWidgets.tileSwitchAccount = lv_tileview_add_tile(g_multiAccountsReceiveWidgets.tileView, RECEIVE_TILE_SWITCH_ACCOUNT, 0, LV_DIR_HOR);
        GuiCreateSwitchAccountWidget(g_multiAccountsReceiveWidgets.tileSwitchAccount);
        GuiCreateSwitchAccountButtons(g_multiAccountsReceiveWidgets.tileSwitchAccount);
    }
    lv_obj_clear_flag(g_multiAccountsReceiveWidgets.tileView, LV_OBJ_FLAG_SCROLLABLE);
}

void GuiMultiAccountsReceiveDeInit(void)
{
    GUI_DEL_OBJ(g_multiAccountsReceiveWidgets.moreCont)
    GUI_DEL_OBJ(g_multiAccountsReceiveWidgets.attentionCont)
    GUI_DEL_OBJ(g_multiAccountsReceiveWidgets.cont)

    CLEAR_OBJECT(g_multiAccountsReceiveWidgets);
    GuiFullscreenModeCleanUp();
    if (g_pageWidget != NULL)
    {
        DestroyPageWidget(g_pageWidget);
        g_pageWidget = NULL;
    }
}

void GuiMultiAccountsReceiveRefresh(void)
{
    char title[30];
    switch (g_multiAccountsReceiveTileNow)
    {
    case RECEIVE_TILE_QRCODE:
        snprintf(title, sizeof(title), _("receive_coin_fmt"), GetCoinCardByIndex(g_chainCard)->coin);
        SetNavBarLeftBtn(g_pageWidget->navBarWidget, NVS_BAR_CLOSE, CloseTimerCurrentViewHandler, NULL);
        SetCoinWallet(g_pageWidget->navBarWidget, g_chainCard, title);
        SetNavBarRightBtn(g_pageWidget->navBarWidget, HasMoreBtn() ? NVS_BAR_MORE_INFO : NVS_RIGHT_BUTTON_BUTT, MoreHandler, NULL);
        RefreshQrCode();
        break;
    case RECEIVE_TILE_SWITCH_ACCOUNT:
        SetNavBarLeftBtn(g_pageWidget->navBarWidget, NVS_BAR_RETURN, ReturnHandler, NULL);
        SetMidBtnLabel(g_pageWidget->navBarWidget, NVS_BAR_MID_LABEL, _("switch_account"));
        SetNavBarRightBtn(g_pageWidget->navBarWidget, NVS_RIGHT_BUTTON_BUTT, NULL, NULL);
        g_showIndex = g_selectIndex[GetCurrentAccountIndex()] / 5 * 5;
        if (g_showIndex < 5)
        {
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.leftBtnImg, LV_OPA_30, LV_PART_MAIN);
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.rightBtnImg, LV_OPA_COVER, LV_PART_MAIN);
        }
        else if (g_showIndex >= GetMaxAddressIndex() - 5)
        {
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.leftBtnImg, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.rightBtnImg, LV_OPA_30, LV_PART_MAIN);
        }
        else
        {
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.leftBtnImg, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.rightBtnImg, LV_OPA_COVER, LV_PART_MAIN);
        }
        break;
    default:
        break;
    }
}

void GuiMultiAccountsReceivePrevTile(void)
{
    GuiMultiAccountsReceiveGotoTile(RECEIVE_TILE_QRCODE);
}

static void GuiCreateMoreWidgets(lv_obj_t *parent)
{
    lv_obj_t *cont, *btn, *img, *label;

    g_multiAccountsReceiveWidgets.moreCont = GuiCreateHintBox(parent, 480, 228, true);
    lv_obj_add_event_cb(lv_obj_get_child(g_multiAccountsReceiveWidgets.moreCont, 0), CloseHintBoxHandler, LV_EVENT_CLICKED, &g_multiAccountsReceiveWidgets.moreCont);
    cont = g_multiAccountsReceiveWidgets.moreCont;

    btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 456, 84);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 120 + 572);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, TutorialHandler, LV_EVENT_CLICKED, NULL);
    img = GuiCreateImg(btn, &imgTutorial);
    lv_obj_align(img, LV_ALIGN_CENTER, -186, 0);
    label = GuiCreateLabelWithFont(btn, _("Tutorial"), &openSans_24);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 60, 4);

    btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 456, 84);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 24 + 572);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, SwitchAccountHandler, LV_EVENT_CLICKED, NULL);
    img = GuiCreateImg(btn, &imgTutorial);
    lv_obj_align(img, LV_ALIGN_CENTER, -186, 0);
    label = GuiCreateLabelWithFont(btn, _("SwitchAccount"), &openSans_24);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 60, 4);
}

static void GuiMultiAccountsReceiveGotoTile(MultiAccountsReceiveTile tile)
{
    g_multiAccountsReceiveTileNow = tile;
    GuiMultiAccountsReceiveRefresh();
    lv_obj_set_tile_id(g_multiAccountsReceiveWidgets.tileView, g_multiAccountsReceiveTileNow, 0, LV_ANIM_OFF);
}

lv_obj_t* CreateMultiAccountsReceiveQRCode(lv_obj_t* parent, uint16_t w, uint16_t h)
{
    lv_obj_t* qrcode = lv_qrcode_create(parent, w, BLACK_COLOR, WHITE_COLOR);
    lv_obj_add_flag(qrcode, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(qrcode, GuiFullscreenModeHandler, LV_EVENT_CLICKED, NULL);
    lv_qrcode_update(qrcode, "", 0);
    return qrcode;
}

static uint16_t GetAddrYExtend(void)
{
    if (g_chainCard == HOME_WALLET_CARD_SUI)
    {
        return 30;
    }
    return 0;
}

static void GuiCreateQrCodeWidget(lv_obj_t *parent)
{
    lv_obj_t *tempObj;
    uint16_t yOffset = 0;
    uint16_t addrYExtend = GetAddrYExtend();

    g_multiAccountsReceiveWidgets.qrCodeCont = GuiCreateContainerWithParent(parent, 408, 524 + addrYExtend);
    lv_obj_align(g_multiAccountsReceiveWidgets.qrCodeCont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(g_multiAccountsReceiveWidgets.qrCodeCont, DARK_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_radius(g_multiAccountsReceiveWidgets.qrCodeCont, 24, LV_PART_MAIN);

    yOffset += 36;
    g_multiAccountsReceiveWidgets.qrCode = CreateMultiAccountsReceiveQRCode(g_multiAccountsReceiveWidgets.qrCodeCont, 336, 336);
    GuiFullscreenModeInit(480, 800, WHITE_COLOR);
    GuiFullscreenModeCreateObject(CreateMultiAccountsReceiveQRCode, 420, 420);

    lv_obj_align(g_multiAccountsReceiveWidgets.qrCode, LV_ALIGN_TOP_MID, 0, yOffset);
    yOffset += 336;

    yOffset += 16;
    g_multiAccountsReceiveWidgets.addressLabel = GuiCreateNoticeLabel(g_multiAccountsReceiveWidgets.qrCodeCont, "");
    lv_obj_set_width(g_multiAccountsReceiveWidgets.addressLabel, 336);
    lv_obj_align(g_multiAccountsReceiveWidgets.addressLabel, LV_ALIGN_TOP_MID, 0, yOffset);
    yOffset += 60;

    yOffset += 16;
    g_multiAccountsReceiveWidgets.addressCountLabel = GuiCreateIllustrateLabel(g_multiAccountsReceiveWidgets.qrCodeCont, "");
    lv_obj_align(g_multiAccountsReceiveWidgets.addressCountLabel, LV_ALIGN_TOP_LEFT, 36, yOffset + addrYExtend);

    g_multiAccountsReceiveWidgets.addressButton = lv_btn_create(g_multiAccountsReceiveWidgets.qrCodeCont);
    lv_obj_set_size(g_multiAccountsReceiveWidgets.addressButton, 336, 36);
    lv_obj_align(g_multiAccountsReceiveWidgets.addressButton, LV_ALIGN_TOP_MID, 0, 464 + addrYExtend);
    lv_obj_set_style_bg_opa(g_multiAccountsReceiveWidgets.addressButton, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(g_multiAccountsReceiveWidgets.addressButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(g_multiAccountsReceiveWidgets.addressButton, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(g_multiAccountsReceiveWidgets.addressButton, 0, LV_PART_MAIN);
    if (IsAddressSwitchable())
    {
        lv_obj_add_event_cb(g_multiAccountsReceiveWidgets.addressButton, SelectAddressHandler, LV_EVENT_CLICKED, NULL);
        tempObj = GuiCreateImg(g_multiAccountsReceiveWidgets.addressButton, &imgArrowRight);
        lv_obj_set_style_img_opa(tempObj, LV_OPA_56, LV_PART_MAIN);
        lv_obj_align(tempObj, LV_ALIGN_CENTER, 150, 0);
    }

    const char* coin = GetCoinCardByIndex(g_chainCard)->coin;
    if (!GetFirstReceive(coin))
    {
        g_multiAccountsReceiveWidgets.attentionCont = GuiCreateHintBox(parent, 480, 386, false);
        tempObj = GuiCreateImg(g_multiAccountsReceiveWidgets.attentionCont, &imgInformation);
        lv_obj_align(tempObj, LV_ALIGN_TOP_LEFT, 36, 462);
        tempObj = GuiCreateLittleTitleLabel(g_multiAccountsReceiveWidgets.attentionCont, _("Attention"));
        lv_obj_align(tempObj, LV_ALIGN_TOP_LEFT, 36, 558);
        char attentionText[150];
        GetAttentionText(attentionText);
        tempObj = GuiCreateLabelWithFont(g_multiAccountsReceiveWidgets.attentionCont, attentionText, &openSans_20);
        lv_obj_align(tempObj, LV_ALIGN_TOP_LEFT, 36, 610);
        tempObj = GuiCreateBtn(g_multiAccountsReceiveWidgets.attentionCont, _("got_it"));
        lv_obj_set_size(tempObj, 122, 66);
        lv_obj_set_style_radius(tempObj, 24, LV_PART_MAIN);
        lv_obj_set_style_bg_color(tempObj, WHITE_COLOR_OPA20, LV_PART_MAIN);
        lv_obj_align(tempObj, LV_ALIGN_BOTTOM_RIGHT, -36, -24);
        lv_obj_add_event_cb(tempObj, CloseAttentionHandler, LV_EVENT_CLICKED, NULL);
        SetFirstReceive(coin, true);
    }
}

void GetAttentionText(char* text)
{
    switch (g_chainCard)
    {
    default:
        sprintf(text, _("receive_coin_hint_fmt"), GetCoinCardByIndex(g_chainCard)->coin);
    }
}

static void GuiCreateSwitchAccountWidget(lv_obj_t *parent)
{
    // Create the account list page.
    uint32_t index;
    lv_obj_t *cont = GuiCreateContainerWithParent(parent, 408, 514);
    lv_obj_t *line;
    static lv_point_t points[2] = {{0, 0}, {360, 0}};
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(cont, DARK_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_radius(cont, 24, LV_PART_MAIN);
    index = 0;
    for (uint32_t i = 0; i < 5; i++)
    {
        g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressCountLabel = GuiCreateLabelWithFont(cont, "", &openSans_24);
        lv_obj_align(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressCountLabel, LV_ALIGN_TOP_LEFT, 24, 30 + 103 * i);
        g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressLabel = GuiCreateNoticeLabel(cont, "");
        lv_obj_align(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressLabel, LV_ALIGN_TOP_LEFT, 24, 56 + 103 * i);
        if (i > 0)
        {
            line = GuiCreateLine(cont, points, 2);
            lv_obj_align(line, LV_ALIGN_TOP_LEFT, 24, 102 * i);
        }

        g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox = lv_btn_create(cont);
        lv_obj_set_size(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, 408, 82);
        lv_obj_align(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_ALIGN_TOP_LEFT, 0, 10 + 102 * i);
        lv_obj_set_style_bg_opa(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_OPA_TRANSP, LV_STATE_CHECKED);
        lv_obj_set_style_border_width(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_outline_width(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, 0, LV_PART_MAIN);
        lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_event_cb(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, SwitchAddressHandler, LV_EVENT_CLICKED, NULL);

        g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg = GuiCreateImg(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, &imgMessageSelect);
        lv_obj_align(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_ALIGN_CENTER, 162, 0);
        lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
        g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg = GuiCreateImg(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, &imgUncheckCircle);
        lv_obj_align(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_ALIGN_CENTER, 162, 0);
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);

        index++;
    }
    RefreshSwitchAddress();
}

static void GuiCreateSwitchAccountButtons(lv_obj_t *parent)
{
    lv_obj_t *btn;
    lv_obj_t *img;

    btn = GuiCreateBtn(parent, "");
    lv_obj_set_size(btn, 96, 66);
    lv_obj_set_style_radius(btn, 24, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, DARK_BG_COLOR, LV_PART_MAIN);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 36, -24);
    img = GuiCreateImg(btn, &imgArrowLeft);
    lv_obj_set_align(img, LV_ALIGN_CENTER);
    if (g_showIndex < 5)
    {
        lv_obj_set_style_img_opa(img, LV_OPA_30, LV_PART_MAIN);
    }
    lv_obj_add_event_cb(btn, LeftBtnHandler, LV_EVENT_CLICKED, NULL);
    g_multiAccountsReceiveWidgets.leftBtnImg = img;

    btn = GuiCreateBtn(parent, "");
    lv_obj_set_size(btn, 96, 66);
    lv_obj_set_style_radius(btn, 24, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, DARK_BG_COLOR, LV_PART_MAIN);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -36, -24);
    img = GuiCreateImg(btn, &imgArrowRight);
    lv_obj_set_align(img, LV_ALIGN_CENTER);
    lv_obj_set_style_opa(img, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_add_event_cb(btn, RightBtnHandler, LV_EVENT_CLICKED, NULL);
    g_multiAccountsReceiveWidgets.rightBtnImg = img;
}

static void RefreshQrCode(void)
{
    AddressDataItem_t addressDataItem;

    ModelGetAddress(g_selectIndex[GetCurrentAccountIndex()], &addressDataItem);
    lv_qrcode_update(g_multiAccountsReceiveWidgets.qrCode, addressDataItem.address, strlen(addressDataItem.address));
    lv_obj_t *fullscreen_qrcode = GuiFullscreenModeGetCreatedObjectWhenVisible();
    if (fullscreen_qrcode)
    {
        lv_qrcode_update(fullscreen_qrcode, addressDataItem.address, strlen(addressDataItem.address));
    }
    char string[128];
    AddressLongModeCut(string, addressDataItem.address, 24);
    lv_label_set_text(g_multiAccountsReceiveWidgets.addressLabel, string);
    lv_label_set_text_fmt(g_multiAccountsReceiveWidgets.addressCountLabel, "Address-%u", (addressDataItem.index + 1));
}

static void RefreshSwitchAddress(void)
{
    AddressDataItem_t addressDataItem;
    char string[128];
    uint32_t index = g_showIndex;
    bool end = false;
    for (uint32_t i = 0; i < 5; i++)
    {
        ModelGetAddress(index, &addressDataItem);
        lv_label_set_text_fmt(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressCountLabel, "Address-%u", (addressDataItem.index + 1));
        AddressLongModeCut(string, addressDataItem.address, 24);
        lv_label_set_text(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressLabel, string);
        if (end)
        {
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressCountLabel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressLabel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);
            continue;
        }
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressCountLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].addressLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);
        if (index == g_selectIndex[GetCurrentAccountIndex()])
        {
            lv_obj_add_state(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_STATE_CHECKED);
            lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_clear_state(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_STATE_CHECKED);
            lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);
        }
        if (index == GetMaxAddressIndex())
        {
            end = true;
        }
        index++;
    }
}

static int GetMaxAddressIndex(void)
{
    return GENERAL_ADDRESS_INDEX_MAX;
}

static void CloseAttentionHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_add_flag(g_multiAccountsReceiveWidgets.attentionCont, LV_OBJ_FLAG_HIDDEN);
    }
}

static void SwitchAccountHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        GUI_DEL_OBJ(g_multiAccountsReceiveWidgets.moreCont);
        GuiCreateInputAccountWidgets(g_multiAccountsReceiveWidgets.tileSwitchAccount);
    }
}

static void CloseSwitchAccountHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_add_flag(g_multiAccountsReceiveWidgets.inputAccountCont, LV_OBJ_FLAG_HIDDEN);
    }
}

static void GuiCreateInputAccountWidgets(lv_obj_t *parent)
{
    lv_obj_t *cont, *label, *line, *closeBtn, *closeImg;
    static lv_point_t points[2] = {{0, 0}, {408, 0}};
    g_inputAccountValid = false;

    if (g_multiAccountsReceiveWidgets.inputAccountCont == NULL) {
        g_multiAccountsReceiveWidgets.inputAccountCont = GuiCreateHintBox(parent, 480, 530, true);
        lv_obj_add_event_cb(lv_obj_get_child(g_multiAccountsReceiveWidgets.inputAccountCont, 0), CloseHintBoxHandler, LV_EVENT_CLICKED, &g_multiAccountsReceiveWidgets.inputAccountCont);
        cont = g_multiAccountsReceiveWidgets.inputAccountCont;

        label = GuiCreateLabelWithFont(cont, _("receive_ada_change_account_title"), &openSans_20);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 36, 30 + 270);
        lv_obj_set_style_text_opa(label, LV_OPA_56, LV_PART_MAIN);
        label = GuiCreateLabelWithFont(cont, "Address-", &openSans_24);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 36, 108 + 270);
        lv_obj_set_style_text_opa(label, LV_OPA_56, LV_PART_MAIN);
        g_multiAccountsReceiveWidgets.inputAccountLabel = GuiCreateLabelWithFont(cont, "", &openSans_24);
        lv_obj_align(g_multiAccountsReceiveWidgets.inputAccountLabel, LV_ALIGN_TOP_LEFT, 38 + lv_obj_get_self_width(label), 108 + 270);
        label = GuiCreateLabelWithFont(cont, _("receive_ada_change_account_limit"), &openSans_20);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 36, 170 + 270);
        lv_obj_set_style_text_color(label, RED_COLOR, LV_PART_MAIN);
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        g_multiAccountsReceiveWidgets.overflowLabel = label;

        line = GuiCreateLine(cont, points, 2);
        lv_obj_align(line, LV_ALIGN_TOP_MID, 0, 160 + 270);

        lv_obj_t *keyboard = GuiCreateNumKeyboard(cont, InputAccountKeyboardHandler, NUM_KEYBOARD_NORMAL, NULL);
        lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, -2);
        lv_obj_add_style(keyboard, &g_numBtnmStyle, LV_PART_ITEMS);
        lv_obj_add_style(keyboard, &g_enterPressBtnmStyle, LV_STATE_PRESSED | LV_PART_ITEMS);
        lv_btnmatrix_set_btn_ctrl(keyboard, 11, LV_BTNMATRIX_CTRL_DISABLED);
        g_multiAccountsReceiveWidgets.inputAccountKeyboard = keyboard;

        closeBtn = lv_btn_create(cont);
        lv_obj_set_size(closeBtn, 36, 36);
        lv_obj_align(closeBtn, LV_ALIGN_TOP_RIGHT, -36, 27 + 270);
        lv_obj_set_style_bg_opa(closeBtn, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(closeBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_outline_width(closeBtn, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(closeBtn, 0, LV_PART_MAIN);
        lv_obj_add_event_cb(closeBtn, CloseSwitchAccountHandler, LV_EVENT_CLICKED, NULL);
        closeImg = GuiCreateImg(closeBtn, &imgClose);
        lv_obj_align(closeImg, LV_ALIGN_CENTER, 0, 0);
    } else {
        lv_label_set_text(g_multiAccountsReceiveWidgets.inputAccountLabel, "");
        lv_obj_clear_flag(g_multiAccountsReceiveWidgets.inputAccountCont, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(g_multiAccountsReceiveWidgets.overflowLabel, LV_OBJ_FLAG_HIDDEN);
    }
}

static void InputAccountKeyboardHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    lv_obj_draw_part_dsc_t *dsc;
    const char *txt;
    char input[16];
    uint32_t len;
    uint64_t longInt;

    if (code == LV_EVENT_CLICKED) {
        txt = lv_btnmatrix_get_btn_text(obj, id);
        strcpy(input, lv_label_get_text(g_multiAccountsReceiveWidgets.inputAccountLabel));
        if (strcmp(txt, LV_SYMBOL_OK) == 0) {
            if (g_inputAccountValid) {
                sscanf(input, "%u", &g_selectedAccount[GetCurrentAccountIndex()]);
                RefreshQrCode();
                lv_obj_add_flag(g_multiAccountsReceiveWidgets.inputAccountCont, LV_OBJ_FLAG_HIDDEN);
                g_inputAccountValid = false;
            }
        } else if (strcmp(txt, "-") == 0) {
            len = strlen(input);
            if (len >= 1) {
                input[len - 1] = '\0';
                lv_label_set_text(g_multiAccountsReceiveWidgets.inputAccountLabel, input);
                lv_obj_add_flag(g_multiAccountsReceiveWidgets.overflowLabel, LV_OBJ_FLAG_HIDDEN);
                if (strlen(input) >= 1) {
                    g_inputAccountValid = true;
                } else {
                    g_inputAccountValid = false;
                }
            }
        } else if (strlen(input) < 15) {
            strcat(input, txt);
            longInt = strtol(input, NULL, 10);
            if (longInt >= ACCOUNT_INDEX_MAX) {
                input[0] = '2';
                input[1] = '3';
                input[2] = '\0';
                lv_obj_clear_flag(g_multiAccountsReceiveWidgets.overflowLabel, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(g_multiAccountsReceiveWidgets.overflowLabel, LV_OBJ_FLAG_HIDDEN);
            }
            if (longInt > 0) {
                if (input[0] == '0') {
                    lv_label_set_text(g_multiAccountsReceiveWidgets.inputAccountLabel, input + 1);
                } else {
                    lv_label_set_text(g_multiAccountsReceiveWidgets.inputAccountLabel, input);
                }
            } else {
                lv_label_set_text(g_multiAccountsReceiveWidgets.inputAccountLabel, "0");
            }
            g_inputAccountValid = true;
        } else {
            g_inputAccountValid = false;
            printf("input to long\r\n");
        }
        SetKeyboardValid(g_inputAccountValid);
    } else if (code == LV_EVENT_DRAW_PART_BEGIN) {
        dsc = lv_event_get_draw_part_dsc(e);
        if (dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            /*Change the draw descriptor of the 12th button*/
            if (dsc->id == 9) {
                dsc->label_dsc->opa = LV_OPA_TRANSP;
            } else if (dsc->id == 11) {
                dsc->rect_dsc->bg_color = ORANGE_COLOR;
                dsc->label_dsc->opa = LV_OPA_TRANSP;
            } else {
                dsc->rect_dsc->bg_color = DARK_GRAY_COLOR;
            }
        }
    } else if (code == LV_EVENT_DRAW_PART_END) {
        dsc = lv_event_get_draw_part_dsc(e);
        /*When the button matrix draws the buttons...*/
        if (dsc->class_p == &lv_btnmatrix_class && dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
            /*Add custom content to the 4th button when the button itself was drawn*/
            if (dsc->id == 9 || dsc->id == 11) {
                lv_img_header_t header;
                lv_draw_img_dsc_t img_draw_dsc;
                lv_area_t a;
                const lv_img_dsc_t *imgDsc;
                lv_res_t res;
                imgDsc = dsc->id == 9 ? &imgBackspace : &imgCheck;
                res = lv_img_decoder_get_info(imgDsc, &header);
                if (res != LV_RES_OK)
                    return;
                a.x1 = dsc->draw_area->x1 + (lv_area_get_width(dsc->draw_area) - header.w) / 2;
                a.x2 = a.x1 + header.w - 1;
                a.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - header.h) / 2;
                a.y2 = a.y1 + header.h - 1;
                lv_draw_img_dsc_init(&img_draw_dsc);
                img_draw_dsc.recolor = lv_color_black();
                if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)
                    img_draw_dsc.recolor_opa = LV_OPA_30;

                lv_draw_img(dsc->draw_ctx, &img_draw_dsc, &a, imgDsc);
            }
        }
    }
}

static void MoreHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        if (g_multiAccountsReceiveWidgets.moreCont == NULL)
        {
            GuiCreateMoreWidgets(g_multiAccountsReceiveWidgets.tileQrCode);
        }
        else
        {
            lv_obj_del(g_multiAccountsReceiveWidgets.moreCont);
            g_multiAccountsReceiveWidgets.moreCont = NULL;
        }
    }
}

static void TutorialHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        GUI_DEL_OBJ(g_multiAccountsReceiveWidgets.moreCont);

        TUTORIAL_LIST_INDEX_ENUM index = TUTORIAL_ETH_RECEIVE;
        GuiFrameOpenViewWithParam(&g_tutorialView, &index, sizeof(index));
    }
}

static void SetKeyboardValid(bool validation)
{
    if (validation) {
        if (lv_btnmatrix_has_btn_ctrl(g_multiAccountsReceiveWidgets.inputAccountKeyboard, 11, LV_BTNMATRIX_CTRL_DISABLED)) {
            lv_btnmatrix_clear_btn_ctrl(g_multiAccountsReceiveWidgets.inputAccountKeyboard, 11, LV_BTNMATRIX_CTRL_DISABLED);
        }
        lv_btnmatrix_set_btn_ctrl(g_multiAccountsReceiveWidgets.inputAccountKeyboard, 11, LV_BTNMATRIX_CTRL_CHECKED);
    } else {
        if (lv_btnmatrix_has_btn_ctrl(g_multiAccountsReceiveWidgets.inputAccountKeyboard, 11, LV_BTNMATRIX_CTRL_CHECKED)) {
            lv_btnmatrix_clear_btn_ctrl(g_multiAccountsReceiveWidgets.inputAccountKeyboard, 11, LV_BTNMATRIX_CTRL_CHECKED);
        }
        lv_btnmatrix_set_btn_ctrl(g_multiAccountsReceiveWidgets.inputAccountKeyboard, 11, LV_BTNMATRIX_CTRL_DISABLED);
    }
}

static void LeftBtnHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.rightBtnImg, LV_OPA_COVER, LV_PART_MAIN);
        if (g_showIndex >= 5)
        {
            g_showIndex -= 5;
            RefreshSwitchAddress();
        }
        if (g_showIndex < 5)
        {
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.leftBtnImg, LV_OPA_30, LV_PART_MAIN);
        }
    }
}

static void RightBtnHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.leftBtnImg, LV_OPA_COVER, LV_PART_MAIN);
        if (g_showIndex < GetMaxAddressIndex() - 5)
        {
            g_showIndex += 5;
            RefreshSwitchAddress();
        }
        if (g_showIndex >= GetMaxAddressIndex() - 5)
        {
            lv_obj_set_style_img_opa(g_multiAccountsReceiveWidgets.rightBtnImg, LV_OPA_30, LV_PART_MAIN);
        }
    }
}

static bool IsAddressSwitchable()
{
    switch (g_chainCard)
    {
    case HOME_WALLET_CARD_ADA:
        return true;
    default:
        return false;
    }
}

static bool HasMoreBtn()
{
    switch (g_chainCard)
    {
    case HOME_WALLET_CARD_ADA:
        return true;

    default:
        return false;
    }
}

static void SwitchAddressHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *checkBox;

    if (code == LV_EVENT_CLICKED)
    {
        checkBox = lv_event_get_target(e);
        for (uint32_t i = 0; i < 5; i++)
        {
            if (checkBox == g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox)
            {
                lv_obj_add_state(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_STATE_CHECKED);
                lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);
                g_selectIndex[GetCurrentAccountIndex()] = g_showIndex + i;
            }
            else
            {
                lv_obj_clear_state(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkBox, LV_STATE_CHECKED);
                lv_obj_add_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].checkedImg, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(g_multiAccountsReceiveWidgets.switchAddressWidgets[i].uncheckedImg, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
}

static void SelectAddressHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        GuiMultiAccountsReceiveGotoTile(RECEIVE_TILE_SWITCH_ACCOUNT);
        RefreshSwitchAddress();
    }
}

static void AddressLongModeCut(char *out, const char *address, uint32_t targetLen)
{
    uint32_t len;

    len = strlen(address);
    if (len <= targetLen)
    {
        strcpy(out, address);
        return;
    }
    strncpy(out, address, targetLen / 2);
    out[targetLen/2] = 0;
    strcat(out, "...");
    strcat(out, address + len - targetLen / 2);
}

#ifdef COMPILE_SIMULATOR

static void ModelGetAddress(uint32_t index, AddressDataItem_t *item)
{
    char hdPath[128];
    sprintf(hdPath, "m/1852'/1815'/%u'/0/%u", g_selectedAccount[GetCurrentAccountIndex()], index);
    printf("hdPath=%s\r\n", hdPath);
    item->index = index;
    sprintf(item->address, "addr1q95l5x7exwzhgupzs0v0ku0censcx8p75jz52cl4uszr463n5nclg6z9gazt9lekgje2k7w53em2xxrljqh73gdul2ks9zxj4d", index);
    strcpy(item->path, hdPath);
}

#else

static void ModelGetAddress(uint32_t index, AddressDataItem_t *item)
{
    char *xPub, hdPath[128];
    SimpleResponse_c_char *result;
    uint32_t currentAccount = g_selectedAccount[GetCurrentAccountIndex()];
    uint32_t currentIndex = g_selectIndex[GetCurrentAccountIndex()];

    switch (g_chainCard)
    {
    case HOME_WALLET_CARD_ADA:
        xPub = GetCurrentAccountPublicKey(XPUB_TYPE_ADA_0 + currentAccount);
        sprintf(hdPath, "m/1852'/1815'/%u'/0/%u", currentAccount, index);
        //cardano mainnet;
        uint8_t network = 1;
        result = cardano_get_base_address(xPub, index, 1);
        break;
    default:
        break;
    }
    ASSERT(xPub);

    if (result->error_code == 0)
    {
        item->index = index;
        printf("address=%s", item->address);
        strcpy(item->address, result->data);
        strcpy(item->path, hdPath);
    }
    free_simple_response_c_char(result);
}

#endif

void GuiResetCurrentMultiAccountsAddressIndex(void)
{
    g_selectIndex[GetCurrentAccountIndex()] = 0;
}

void GuiResetCurrentMultiAccountsSelectedAccount(void)
{
    g_selectedAccount[GetCurrentAccountIndex()] = 0;
}

void GuiResetAllMultiAccountsAddressIndex(void)
{
    memset(g_selectIndex, 0, sizeof(g_selectIndex));
}

void GuiResetAllMultiAccountsSelectedAccount(void)
{
    memset(g_selectedAccount, 0, sizeof(g_selectedAccount));
}