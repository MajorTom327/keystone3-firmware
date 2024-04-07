#include "gui_multisig_read_sdcard_widgets.h"
#include "gui.h"
#include "gui_page.h"
#include "stdlib.h"
#include "gui_button.h"
#include "gui_import_multisig_wallet_info_widgets.h"
#include "gui_btc.h"

#ifndef COMPILE_SIMULATOR
#include "drv_sdcard.h"
#include "user_fatfs.h"
#else
#include "simulator_mock_define.h"
#include "simulator_model.h"
#endif

static PageWidget_t *g_pageWidget;
static char g_fileList[10][64] = {0};

static void GuiContent(lv_obj_t *);
static void GuiSelectFileHandler(lv_event_t *e);
static int endsWith(const char *str, const char *suffix);
static FileFilterType g_fileFilterType = ALL;



void GuiMultisigReadSdcardWidgetsInit(uint8_t fileFilterType)
{
    g_fileFilterType = fileFilterType;
    g_pageWidget = CreatePageWidget();
    GuiContent(g_pageWidget->contentZone);
    SetNavBarLeftBtn(g_pageWidget->navBarWidget, NVS_BAR_RETURN, CloseCurrentViewHandler, NULL);
    SetMidBtnLabel(g_pageWidget->navBarWidget, NVS_BAR_MID_LABEL, _("import_multi_wallet_via_micro_title"));
    SetNavBarMidBtn(g_pageWidget->navBarWidget, NVS_MID_BUTTON_BUTT, NULL, NULL);
}
void GuiMultisigReadSdcardWidgetsDeInit()
{
    DestroyPageWidget(g_pageWidget);
    g_pageWidget = NULL;
}

static void GuiContent(lv_obj_t *parent)
{
    GuiAddObjFlag(parent, LV_OBJ_FLAG_SCROLLABLE);
    char *buffer = EXT_MALLOC(1024 * 5);
    uint32_t number = 0;
    int i = 0;
#ifdef COMPILE_SIMULATOR
    FatfsGetFileName("C:/assets/sd", buffer, &number, 1024 * 5, NULL);
#else
    FatfsGetFileName("0:", buffer, &number, 1024 * 5, NULL);
#endif
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        bool shouldGo = true;
        switch (g_fileFilterType) {
        case ALL:
            strncpy(g_fileList[i], token, sizeof(g_fileList[i]) - 1);
            break;
        case ONLY_TXT:
            shouldGo = endsWith(token, ".txt");
            if (shouldGo) {
                strncpy(g_fileList[i], token, sizeof(g_fileList[i]) - 1);
            }
            break;
        case ONLY_PSBT:
            shouldGo = endsWith(token, ".psbt");
            if (shouldGo) {
                strncpy(g_fileList[i], token, sizeof(g_fileList[i]) - 1);
            }
            break;
        case ONLY_JSON:
            shouldGo = endsWith(token, ".json");
            if (shouldGo) {
                strncpy(g_fileList[i], token, sizeof(g_fileList[i]) - 1);
            }
            break;
        default:
            break;
        }
        token = strtok(NULL, " ");
        if (!shouldGo) {
            continue;
        }
        lv_obj_t *btn = GuiCreateSelectButton(parent, g_fileList[i], &imgArrowRight, GuiSelectFileHandler, g_fileList[i], false);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 84 * i);
        i++;
        if (i == 10) {
            break;
        }
    }
    EXT_FREE(buffer);
}

static void GuiSelectFileHandler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    char *path = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {

        switch (g_fileFilterType) {
        case ALL:
            break;
        case ONLY_TXT: {
            char *walletConfig = FatfsFileRead(path);
            GuiSetMultisigImportWalletDataBySDCard(walletConfig);
            GuiFrameOpenView(&g_importMultisigWalletInfoView);
        }
        break;
        case ONLY_PSBT: {
            uint32_t readBytes = 0;
            uint8_t *psbtBytes = FatfsFileReadBytes(path, &readBytes);

            // for debug
            char *psbtStr = EXT_MALLOC(readBytes * 2 + 1);
            psbtStr[readBytes * 2] = 0;
            ByteArrayToHexStr(psbtBytes, readBytes, psbtStr);
            printf("psbt is %s\n", psbtStr);
            EXT_FREE(psbtStr);

            GuiSetPsbtStrData(psbtBytes, readBytes);
            static ViewType viewType = BtcTx;
            GuiFrameOpenViewWithParam(&g_transactionDetailView, &viewType, sizeof(viewType));
        }
        break;
        case ONLY_JSON:

            break;
        default:
            break;
        }


    }
}

static int endsWith(const char *str, const char *suffix)
{
    size_t strLen = strlen(str);
    size_t suffixLen = strlen(suffix);

    if (suffixLen > strLen) {
        return 0;
    }

    const char *strEnd = str + (strLen - suffixLen);
    if (strcmp(strEnd, suffix) == 0) {
        return 1;
    }

    return 0;
}