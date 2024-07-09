#ifndef BTC_ONLY
#include "gui_analyze.h"
#include "rust.h"
#include "account_public_info.h"
#include "keystore.h"
#include "gui_chain.h"
#include "version.h"
#include "secret_cache.h"
#include "screen_manager.h"
#include "account_manager.h"
#include "assert.h"
#include "cjson/cJSON.h"
#include "user_memory.h"
#include "gui_qr_hintbox.h"
static uint8_t GetSolPublickeyIndex(char* rootPath);

static bool g_isMulti = false;
static URParseResult *g_urResult = NULL;
static URParseMultiResult *g_urMultiResult = NULL;
static void *g_parseResult = NULL;
static ViewType g_viewType = ViewTypeUnKnown;
#define CHECK_FREE_PARSE_SOL_RESULT(result)                                                                                       \
    if (result != NULL)                                                                                                           \
    {                                                                                                                             \
        switch (g_viewType)                                                                                                       \
        {                                                                                                                         \
        case SolanaTx:                                                                                                            \
            free_TransactionParseResult_DisplaySolanaTx((PtrT_TransactionParseResult_DisplaySolanaTx)result);                     \
            break;                                                                                                                \
        case SolanaMessage:                                                                                                       \
            free_TransactionParseResult_DisplaySolanaMessage((PtrT_TransactionParseResult_DisplaySolanaMessage)result);           \
            break;                                                                                                                \
        default:                                                                                                                  \
            break;                                                                                                                \
        }                                                                                                                         \
        result = NULL;                                                                                                            \
    }

void GuiSetSolUrData(URParseResult *urResult, URParseMultiResult *urMultiResult, bool multi)
{
    g_urResult = urResult;
    g_urMultiResult = urMultiResult;
    g_isMulti = multi;
    g_viewType = g_isMulti ? urMultiResult->t : g_urResult->t;
}

UREncodeResult *GuiGetSolSignQrCodeData(void)
{
    bool enable = IsPreviousLockScreenEnable();
    SetLockScreen(false);
    UREncodeResult *encodeResult;
    void *data = g_isMulti ? g_urMultiResult->data : g_urResult->data;
    do {
        uint8_t seed[64];
        int len = GetMnemonicType() == MNEMONIC_TYPE_BIP39 ? sizeof(seed) : GetCurrentAccountEntropyLen();
        GetAccountSeed(GetCurrentAccountIndex(), seed, SecretCacheGetPassword());
        encodeResult = solana_sign_tx(data, seed, len);
        ClearSecretCache();
        CHECK_CHAIN_BREAK(encodeResult);
    } while (0);
    SetLockScreen(enable);
    return encodeResult;
}

void *GuiGetSolData(void)
{
    CHECK_FREE_PARSE_SOL_RESULT(g_parseResult);
    void *data = g_isMulti ? g_urMultiResult->data : g_urResult->data;
    do {
        PtrT_TransactionParseResult_DisplaySolanaTx parseResult = solana_parse_tx(data);
        CHECK_CHAIN_BREAK(parseResult);
        g_parseResult = (void *)parseResult;
    } while (0);
    return g_parseResult;
}

PtrT_TransactionCheckResult GuiGetSolCheckResult(void)
{
    uint8_t mfp[4];
    void *data = g_isMulti ? g_urMultiResult->data : g_urResult->data;
    GetMasterFingerPrint(mfp);
    return solana_check(data,  mfp, sizeof(mfp));
}

void *GuiGetSolMessageData(void)
{
    CHECK_FREE_PARSE_SOL_RESULT(g_parseResult);
    void *data = g_isMulti ? g_urMultiResult->data : g_urResult->data;
    do {
        char *path = sol_get_path(data);
        char pubkeyIndex = GetSolPublickeyIndex(path);
        char *pubKey = GetCurrentAccountPublicKey(pubkeyIndex);
        PtrT_TransactionParseResult_DisplaySolanaMessage parseResult = solana_parse_message(data, pubKey);
        free_ptr_string(path);
        CHECK_CHAIN_BREAK(parseResult);
        g_parseResult = (void *)parseResult;
    } while (0);
    return g_parseResult;
}

void FreeSolMemory(void)
{
    CHECK_FREE_UR_RESULT(g_urResult, false);
    CHECK_FREE_UR_RESULT(g_urMultiResult, true);
    CHECK_FREE_PARSE_SOL_RESULT(g_parseResult);
}

void GetSolMessageType(void *indata, void *param, uint32_t maxLen)
{
    DisplaySolanaMessage *message = (DisplaySolanaMessage *)param;
    if (message->utf8_message) {
        strcpy_s((char *)indata, maxLen, "utf8_message");
    } else {
        strcpy_s((char *)indata, maxLen, "raw_message");
    }

}

void GetSolMessageFrom(void *indata, void *param, uint32_t maxLen)
{
    DisplaySolanaMessage *message = (DisplaySolanaMessage *)param;
    if (strlen(message->from) >= maxLen) {
        snprintf((char *)indata, maxLen - 3, "%s", message->from);
        strcat((char *)indata, "...");
        snprintf((char *)indata, maxLen, "%.*s...", maxLen - 4, message->from);
    } else {
        strcpy_s((char *)indata, maxLen, message->from);
    }
}

void GetSolMessageUtf8(void *indata, void *param, uint32_t maxLen)
{
    DisplaySolanaMessage *message = (DisplaySolanaMessage *)param;
    if (strlen(message->utf8_message) >= maxLen) {
        snprintf((char *)indata, maxLen - 3, "%s", message->utf8_message);
        strcat((char *)indata, "...");
    } else {
        snprintf((char *)indata, maxLen, "%s", message->utf8_message);
    }
}

void GetSolMessageRaw(void *indata, void *param, uint32_t maxLen)
{
    int len = strlen("\n#F5C131 The data is not parseable. Please#\n#F5C131 refer to the software wallet interface#\n#F5C131 for viewing.#");
    DisplaySolanaMessage *message = (DisplaySolanaMessage *)param;
    if (strlen(message->raw_message) >= maxLen - len) {
        snprintf((char *)indata, maxLen - 3 - len, "%s", message->raw_message);
        strcat((char *)indata, "...");
    } else {
        snprintf((char *)indata, maxLen, "%s%s", message->raw_message, "\n#F5C131 The data is not parseable. Please#\n#F5C131 refer to the software wallet interface#\n#F5C131 for viewing.#");
    }
}

static uint8_t GetSolPublickeyIndex(char* rootPath)
{
    if (strcmp(rootPath, "44'/501'/0'") == 0) return XPUB_TYPE_SOL_BIP44_0;
    if (strcmp(rootPath, "44'/501'/1'") == 0) return XPUB_TYPE_SOL_BIP44_1;
    if (strcmp(rootPath, "44'/501'/2'") == 0) return XPUB_TYPE_SOL_BIP44_2;
    if (strcmp(rootPath, "44'/501'/3'") == 0) return XPUB_TYPE_SOL_BIP44_3;
    if (strcmp(rootPath, "44'/501'/4'") == 0) return XPUB_TYPE_SOL_BIP44_4;
    if (strcmp(rootPath, "44'/501'/5'") == 0) return XPUB_TYPE_SOL_BIP44_5;
    if (strcmp(rootPath, "44'/501'/6'") == 0) return XPUB_TYPE_SOL_BIP44_6;
    if (strcmp(rootPath, "44'/501'/7'") == 0) return XPUB_TYPE_SOL_BIP44_7;
    if (strcmp(rootPath, "44'/501'/8'") == 0) return XPUB_TYPE_SOL_BIP44_8;
    if (strcmp(rootPath, "44'/501'/9'") == 0) return XPUB_TYPE_SOL_BIP44_9;
    if (strcmp(rootPath, "44'/501'") == 0) return XPUB_TYPE_SOL_BIP44_ROOT;
    if (strcmp(rootPath, "44'/501'/0'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_0;
    if (strcmp(rootPath, "44'/501'/1'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_1;
    if (strcmp(rootPath, "44'/501'/2'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_2;
    if (strcmp(rootPath, "44'/501'/3'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_3;
    if (strcmp(rootPath, "44'/501'/4'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_4;
    if (strcmp(rootPath, "44'/501'/5'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_5;
    if (strcmp(rootPath, "44'/501'/6'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_6;
    if (strcmp(rootPath, "44'/501'/7'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_7;
    if (strcmp(rootPath, "44'/501'/8'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_8;
    if (strcmp(rootPath, "44'/501'/9'/0'") == 0) return XPUB_TYPE_SOL_BIP44_CHANGE_9;
    ASSERT(0);

    return -1;
}

static void SetContainerDefaultStyle(lv_obj_t *container)
{
    lv_obj_set_style_radius(container, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(container, WHITE_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(container, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void SetTitleLabelStyle(lv_obj_t *label)
{
    lv_obj_set_style_text_font(label, g_defIllustrateFont, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, WHITE_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_opa(label, 144, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void SetContentLableStyle(lv_obj_t *label)
{
    lv_obj_set_style_text_font(label, g_defIllustrateFont, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, WHITE_COLOR, LV_PART_MAIN);
}

static void SetVotesOnOrderLableStyle(lv_obj_t *label)
{
    lv_obj_set_style_text_font(label, g_defLittleTitleFont, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, WHITE_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_opa(label, 144, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void SetVotesOnContentLableStyle(lv_obj_t *label)
{
    lv_obj_set_style_text_font(label, g_defLittleTitleFont, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF5870A), LV_PART_MAIN);
}

static void GuiShowSolTxTransferOverview(lv_obj_t *parent, PtrT_DisplaySolanaTxOverview overviewData)
{

    lv_obj_t *valueContainer = GuiCreateContainerWithParent(parent, 408, 106);
    lv_obj_align(valueContainer, LV_ALIGN_DEFAULT, 0, 0);
    SetContainerDefaultStyle(valueContainer);

    lv_obj_t *label = lv_label_create(valueContainer);
    lv_label_set_text(label, "Value");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    SetTitleLabelStyle(label);

    label = lv_label_create(valueContainer);
    lv_label_set_text(label, overviewData->transfer_value);
    lv_obj_set_style_text_font(label, &openSansEnLittleTitle, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF5870A), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 50);

    lv_obj_t *mainActionContainer = GuiCreateContainerWithParent(parent, 408, 62);
    lv_obj_align_to(mainActionContainer, valueContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    SetContainerDefaultStyle(mainActionContainer);

    label = lv_label_create(mainActionContainer);
    lv_label_set_text(label, "Main Action");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    SetTitleLabelStyle(label);

    label = lv_label_create(mainActionContainer);
    lv_label_set_text(label, overviewData->main_action);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 151, 16);
    SetContentLableStyle(label);

    lv_obj_t *addressContainer = GuiCreateContainerWithParent(parent, 408, 224);
    lv_obj_align_to(addressContainer, mainActionContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    SetContainerDefaultStyle(addressContainer);

    label = lv_label_create(addressContainer);
    lv_label_set_text(label, "From");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    SetTitleLabelStyle(label);

    label = lv_label_create(addressContainer);
    lv_label_set_text(label, overviewData->transfer_from);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, 306);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 54);
    SetContentLableStyle(label);

    label = lv_label_create(addressContainer);
    lv_label_set_text(label, "To");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 130);
    SetTitleLabelStyle(label);

    label = lv_label_create(addressContainer);
    lv_label_set_text(label, overviewData->transfer_to);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, 306);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 168);
    SetContentLableStyle(label);
}

static void GuiShowSolTxVoteOverview(lv_obj_t *parent, PtrT_DisplaySolanaTxOverview overviewData)
{
    lv_obj_t *votesOnContainer = GuiCreateContainerWithParent(parent, 408, 150);
    lv_obj_align(votesOnContainer, LV_ALIGN_DEFAULT, 0, 0);
    SetContainerDefaultStyle(votesOnContainer);

    lv_obj_t *label = lv_label_create(votesOnContainer);
    lv_label_set_text(label, "Votes on");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    SetTitleLabelStyle(label);

    int containerYOffset = 0;
    int votesOnContainerHeight = 16 + 30 + 16;

    lv_obj_t *prevLabel = label;

    PtrT_VecFFI_DisplaySolanaTxOverviewVotesOn votesOn = overviewData->votes_on;
    for (int i = 0; i < votesOn->size; i++) {
        label = lv_label_create(votesOnContainer);
        char order[BUFFER_SIZE_16] = {0};
        snprintf_s(order, BUFFER_SIZE_16, "%d.", i + 1);
        lv_label_set_text(label, order);
        SetVotesOnOrderLableStyle(label);
        lv_obj_align_to(label, prevLabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
        prevLabel = label;

        label = lv_label_create(votesOnContainer);
        lv_label_set_text(label, votesOn->data[i].slot);
        SetVotesOnContentLableStyle(label);
        lv_obj_align_to(label, prevLabel, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

        votesOnContainerHeight = votesOnContainerHeight + 40 + 4;

    }
    lv_obj_set_height(votesOnContainer, votesOnContainerHeight);

    containerYOffset = containerYOffset + votesOnContainerHeight;

    lv_obj_t *mainActionContainer = GuiCreateContainerWithParent(parent, 408, 62);
    lv_obj_align_to(mainActionContainer, votesOnContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    SetContainerDefaultStyle(mainActionContainer);

    label = lv_label_create(mainActionContainer);
    lv_label_set_text(label, "Main Action");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    SetTitleLabelStyle(label);

    label = lv_label_create(mainActionContainer);
    lv_label_set_text(label, overviewData->main_action);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 151, 16);
    SetContentLableStyle(label);

    lv_obj_t *voteAccountContainer = GuiCreateContainerWithParent(parent, 408, 130);
    lv_obj_align_to(voteAccountContainer, mainActionContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 16);
    SetContainerDefaultStyle(voteAccountContainer);

    label = lv_label_create(voteAccountContainer);
    lv_label_set_text(label, "Vote Account");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    SetTitleLabelStyle(label);

    label = lv_label_create(voteAccountContainer);
    lv_label_set_text(label, overviewData->vote_account);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, 306);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 54);
    SetContentLableStyle(label);
}

static void GuiShowSolTxGeneralOverview(lv_obj_t *parent, PtrT_DisplaySolanaTxOverview overviewData)
{
    PtrT_VecFFI_DisplaySolanaTxOverviewGeneral general = overviewData->general;

    int containerYOffset = 0;

    for (int i = 0; i < general->size; i++) {
        lv_obj_t *container = GuiCreateContainerWithParent(parent, 408, 150);
        lv_obj_align(container, LV_ALIGN_DEFAULT, 0, containerYOffset);
        SetContainerDefaultStyle(container);

        char *program = general->data[i].program;
        lv_obj_t *orderLabel = lv_label_create(container);
        char order[BUFFER_SIZE_16] = {0};
        snprintf_s(order, BUFFER_SIZE_16, "#%d", i + 1);
        lv_label_set_text(orderLabel, order);
        lv_obj_set_style_text_font(orderLabel, g_defIllustrateFont, LV_PART_MAIN);
        lv_obj_set_style_text_color(orderLabel, lv_color_hex(0xF5870A), LV_PART_MAIN);
        lv_obj_align(orderLabel, LV_ALIGN_TOP_LEFT, 24, 16);

        lv_obj_t *label = lv_label_create(container);
        lv_label_set_text(label, "Program");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 62);
        SetTitleLabelStyle(label);

        label = lv_label_create(container);
        lv_label_set_text(label, program);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 121, 62);
        SetContentLableStyle(label);

        if (0 == strcmp(program, "Unknown")) {
            lv_obj_set_height(container, 108);
            containerYOffset = containerYOffset + 108 + 16;
            continue;
        } else {
            containerYOffset = containerYOffset + 150 + 16;
        }

        char *method = general->data[i].method;
        label = lv_label_create(container);
        lv_label_set_text(label, "Method");
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 100);
        SetTitleLabelStyle(label);

        label = lv_label_create(container);
        lv_label_set_text(label, method);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, 113, 100);
        SetContentLableStyle(label);
    }
}
static void GuiShowSolTxUnknownOverview(lv_obj_t *parent)
{
    uint16_t height = 177;
    lv_obj_t *container = GuiCreateContainerWithParent(parent, 408, 302);
    lv_obj_align(container, LV_ALIGN_DEFAULT, 0, 0);
    SetContainerDefaultStyle(container);

    lv_obj_t *img = GuiCreateImg(container, &imgUnknown);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t *label = GuiCreateTextLabel(container, _("unknown_transaction_title"));
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 144);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_refr_size(label);
    height += lv_obj_get_self_height(label);

    label = GuiCreateNoticeLabel(container, _("unknown_transaction_desc"));
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -36);
    lv_obj_set_width(label, 360);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_refr_size(label);
    height += lv_obj_get_self_height(label);
    lv_obj_set_height(container, height);
}

typedef struct {
    const char* address;
} SolanaAddressLearnMoreData;

void SolanaAddressLearnMore(lv_event_t *e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    SolanaAddressLearnMoreData* data = (SolanaAddressLearnMoreData*)lv_obj_get_user_data(obj);
    if (data != NULL) {
        char url[512];
        snprintf(url, sizeof(url), "https://solscan.io/account/%s#tableEntries", data->address);
        GuiQRCodeHintBoxOpenBig(url, "Address Lookup Table Program Url", _("solana_alt_notice"), url);
    }
}

static void GuiShowSolTxInstructionsOverview(lv_obj_t *parent, PtrT_DisplaySolanaTxOverview overviewData)
{
    PtrT_DisplaySolanaTxOverviewUnknownInstructions unknown_instructions = overviewData->unknown_instructions;
    PtrT_VecFFI_Instruction overview_instructions = unknown_instructions->overview_instructions;
    // notice container
    lv_obj_t *noticeContainer = GuiCreateContainerWithParent(parent, 408, LV_SIZE_CONTENT);
    lv_obj_align(noticeContainer, LV_ALIGN_DEFAULT, 0, 0);
    SetContainerDefaultStyle(noticeContainer);
    // Create a flex container for icon and notice label
    lv_obj_t *iconNoticeContainer = lv_obj_create(noticeContainer);
    lv_obj_set_size(iconNoticeContainer, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(iconNoticeContainer, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(iconNoticeContainer, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(iconNoticeContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(iconNoticeContainer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(iconNoticeContainer, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(iconNoticeContainer, 24, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(iconNoticeContainer, LV_OPA_TRANSP, LV_PART_MAIN); // Set transparent background
    // Create image inside the flex container
    lv_obj_t *img = GuiCreateImg(iconNoticeContainer, &imgInfoOrange);
    lv_obj_set_style_pad_right(img, 2, LV_PART_MAIN); // Add right padding to the image
    lv_obj_t *noticeLabel = lv_label_create(iconNoticeContainer);
    lv_obj_set_style_text_font(noticeLabel, g_defIllustrateFont, LV_PART_MAIN);
    lv_obj_set_style_text_color(noticeLabel, lv_color_hex(0xF5870A), LV_PART_MAIN);
    lv_label_set_text(noticeLabel, "Notice");

    lv_obj_t *noticeContent = lv_label_create(noticeContainer);
    lv_obj_set_width(noticeContent, lv_pct(90));
    lv_label_set_long_mode(noticeContent, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(noticeContent, g_defIllustrateFont, LV_PART_MAIN);
    lv_obj_set_style_text_color(noticeContent, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    // multi language support
    lv_label_set_text(noticeContent,_("solana_parse_tx_notice"));
    lv_obj_set_style_text_opa(noticeContent, 144, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(noticeContent, iconNoticeContainer, LV_ALIGN_OUT_BOTTOM_LEFT, 24, 10);

    int containerYOffset = 166 ;
    for (int i = 0; i < overview_instructions->size; i++) {
        // container will auto adjust height
        lv_obj_t *container = GuiCreateContainerWithParent(parent, 408, LV_SIZE_CONTENT);
        lv_obj_align(container, LV_ALIGN_DEFAULT, 0, containerYOffset);
        SetContainerDefaultStyle(container);

        lv_obj_t *orderLabel = lv_label_create(container);
        char order[BUFFER_SIZE_16] = {0};
        snprintf_s(order, BUFFER_SIZE_16, "Instruction#%d", i + 1);
        lv_label_set_text(orderLabel, order);
        lv_obj_set_style_text_font(orderLabel, g_defIllustrateFont, LV_PART_MAIN);
        lv_obj_set_style_text_color(orderLabel, lv_color_hex(0xF5870A), LV_PART_MAIN);
        lv_obj_align(orderLabel, LV_ALIGN_TOP_LEFT, 24, 16);

        PtrT_VecFFI_PtrString accounts = overview_instructions->data[i].accounts;
        // accounts label
        lv_obj_t *accounts_label = lv_label_create(container);
        if (accounts->size != 0) {
            lv_label_set_text(accounts_label, "accounts");
            lv_obj_set_style_text_color(accounts_label, WHITE_COLOR, LV_PART_MAIN);
            lv_obj_set_style_text_opa(accounts_label, 144, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_align(accounts_label, LV_ALIGN_TOP_LEFT, 24, 44);
        } else {
            lv_label_set_text(accounts_label, "");
            lv_obj_align(accounts_label, LV_ALIGN_TOP_LEFT, 24, 10);
        }
        // Create a flex container for accounts with scrolling
        lv_obj_t *accounts_cont = lv_obj_create(container);
        lv_obj_set_width(accounts_cont, lv_pct(88));
        if (accounts->size == 0) {
            lv_obj_set_height(accounts_cont, 0);
        } else if (accounts->size == 2) {
            lv_obj_set_height(accounts_cont, 150);
        } else {
            lv_obj_set_height(accounts_cont, 200);
        }
        lv_obj_set_flex_flow(accounts_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(accounts_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_align_to(accounts_cont, accounts_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
        lv_obj_set_style_bg_opa(accounts_cont, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_pad_left(accounts_cont, 0, LV_PART_MAIN);  // remove left padding
        lv_obj_set_style_pad_right(accounts_cont, 0, LV_PART_MAIN);  // remove right padding
        lv_obj_add_flag(accounts_cont, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_border_width(accounts_cont, 0, LV_PART_MAIN);

        for (int j = 0; j < accounts->size; j++) {
            lv_obj_t *account_cont = lv_obj_create(accounts_cont);
            lv_obj_set_style_pad_left(account_cont, 0, LV_PART_MAIN);  // remove left padding
            lv_obj_set_style_pad_right(account_cont, 0, LV_PART_MAIN);  // remove right padding
            lv_obj_set_style_border_width(account_cont, 0, LV_PART_MAIN);
            lv_obj_set_width(account_cont, lv_pct(100));
            lv_obj_set_height(account_cont, LV_SIZE_CONTENT);
            lv_obj_set_flex_flow(account_cont, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(account_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
            lv_obj_set_style_bg_opa(account_cont, LV_OPA_TRANSP, LV_PART_MAIN);
            lv_obj_set_style_pad_ver(account_cont, 2, LV_PART_MAIN); // vertical padding

            char order[BUFFER_SIZE_16] = {0};
            snprintf_s(order, BUFFER_SIZE_16, "%d.", j + 1);
            lv_obj_t *orderLabel = lv_label_create(account_cont);
            lv_label_set_text(orderLabel, order);
            lv_obj_set_style_text_font(orderLabel, g_defIllustrateFont, LV_PART_MAIN);
            lv_obj_set_style_text_color(orderLabel, WHITE_COLOR, LV_PART_MAIN);
            lv_obj_set_style_text_opa(orderLabel, 144, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_t *account_label = lv_label_create(account_cont);
            lv_obj_set_style_pad_right(account_cont, 0, LV_PART_MAIN);
            lv_obj_set_width(account_label, lv_pct(80));
            lv_obj_set_flex_grow(account_label, 1);
            lv_obj_set_flex_align(orderLabel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
            // check whether the account is a AddressLookupTable account
            // if account is start with "Table", then it is a AddressLookupTable account
            if (strncmp(accounts->data[j], "Table", 5) == 0) {
                lv_label_set_text(account_label, accounts->data[j] + 6);
                lv_obj_t *info_icon = GuiCreateImg(account_cont, &imgInfoSmall);
                lv_obj_set_style_pad_right(info_icon, 0, LV_PART_MAIN);
                // add account click event
                lv_obj_add_flag(info_icon, LV_OBJ_FLAG_CLICKABLE);
                SolanaAddressLearnMoreData* data = (SolanaAddressLearnMoreData*)malloc(sizeof(SolanaAddressLearnMoreData));
                if (data != NULL) {
                    const char* address = accounts->data[j] + 6; // remove "Table:" prefix
                    size_t address_len = strlen(address);
                    data->address = (char*)malloc(address_len + 1);
                    strcpy(data->address, address);
                    lv_obj_set_user_data(info_icon, data);
                    SolanaAddressLearnMoreData* retrieved_data = (SolanaAddressLearnMoreData*)lv_obj_get_user_data(info_icon);
                }
                lv_obj_add_event_cb(info_icon, SolanaAddressLearnMore, LV_EVENT_CLICKED, NULL);
            } else {
                lv_label_set_text(account_label, accounts->data[j]);
            }
            lv_obj_set_style_text_color(account_label, WHITE_COLOR, LV_PART_MAIN);
            lv_label_set_long_mode(account_label, LV_LABEL_LONG_WRAP);

            lv_obj_set_style_border_width(account_label, 0, LV_PART_MAIN);
        }
        // data label
        lv_obj_t *data_label = lv_label_create(container);
        lv_label_set_text(data_label, "data");
        lv_obj_set_style_text_color(data_label, WHITE_COLOR, LV_PART_MAIN);
        lv_obj_set_style_text_opa(data_label, 144, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_align_to(data_label, accounts_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
        // data value
        lv_obj_t *data_value = lv_label_create(container);
        lv_label_set_text(data_value, overview_instructions->data[i].data);
        lv_obj_set_style_text_font(data_value, g_defIllustrateFont, LV_PART_MAIN);
        lv_obj_set_style_text_color(data_value, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(data_value, data_label, LV_ALIGN_LEFT_MID, 50, 0);
        lv_obj_set_width(data_value, lv_pct(90));
        lv_label_set_long_mode(data_value, LV_LABEL_LONG_WRAP);
        // program address label
        lv_obj_t *programAddress_label = lv_label_create(container);
        lv_label_set_text(programAddress_label, "programAddress");
        lv_obj_set_style_text_color(programAddress_label, WHITE_COLOR, LV_PART_MAIN);
        lv_obj_set_style_text_opa(programAddress_label, 144, LV_PART_MAIN | LV_STATE_DEFAULT);


        lv_obj_align_to(programAddress_label, data_value, LV_ALIGN_OUT_BOTTOM_LEFT, -50, 10);
        // program address value
        PtrString program_address = overview_instructions->data[i].program_address;
        lv_obj_t *program_content_label = lv_label_create(container);
        lv_label_set_text(program_content_label, program_address);
        lv_obj_set_style_text_font(program_content_label, g_defIllustrateFont, LV_PART_MAIN);
        lv_obj_set_style_text_color(program_content_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_align_to(program_content_label, programAddress_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
        lv_obj_set_width(program_content_label, lv_pct(90));
        lv_label_set_long_mode(program_content_label, LV_LABEL_LONG_WRAP);
        // calculate the height of the container
        lv_obj_update_layout(container);
        containerYOffset += lv_obj_get_height(container) + 16; // set padding between containers
    }
}


void GuiShowSolTxOverview(lv_obj_t *parent, void *totalData)
{
    lv_obj_set_size(parent, 408, 444);
    lv_obj_add_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(parent, LV_OBJ_FLAG_CLICKABLE);

    DisplaySolanaTx *txData = (DisplaySolanaTx*)totalData;
    PtrT_DisplaySolanaTxOverview overviewData = txData->overview;
    if (0 == strcmp(overviewData->display_type, "Transfer")) {
        GuiShowSolTxTransferOverview(parent, overviewData);
    } else if (0 == strcmp(overviewData->display_type, "Vote")) {
        GuiShowSolTxVoteOverview(parent, overviewData);
    } else if (0 == strcmp(overviewData->display_type, "General")) {
        GuiShowSolTxGeneralOverview(parent, overviewData);
    } else {
        GuiShowSolTxInstructionsOverview(parent, overviewData);
    }
}

void GuiShowSolTxDetail(lv_obj_t *parent, void *totalData)
{
    lv_obj_set_size(parent, 408, 444);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 408, 444);
    lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(cont, 0, 0);
    lv_obj_set_style_radius(cont, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cont, WHITE_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cont, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cont, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(cont, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cont, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cont, 24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(cont, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    DisplaySolanaTx *txData = (DisplaySolanaTx*)totalData;
    PtrString txDetail = txData->detail;

    lv_obj_t *label = lv_label_create(cont);
    cJSON *root = cJSON_Parse((const char *)txDetail);
    char *retStr = cJSON_Print(root);
    lv_label_set_text(label, retStr);
    EXT_FREE(retStr);
    cJSON_Delete(root);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, 360);
    SetTitleLabelStyle(label);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
}
#endif
