#include "gui.h"
#include "gui_obj.h"
#include "gui_views.h"
#include "gui_multi_path_coin_receive_widgets.h"

static int32_t GuiMultiPathCoinReceiveViewInit(uint8_t chain)
{
    GuiMultiPathCoinReceiveInit(chain);
    return SUCCESS_CODE;
}

static int32_t GuiMultiPathCoinReceiveViewDeInit(void)
{
    printf("GuiEthereumReceiveViewDeInit\r\n");
    GuiMultiPathCoinReceiveDeInit();
    return SUCCESS_CODE;
}

int32_t GuiMultiPathCoinReceiveViewEventProcess(void *self, uint16_t usEvent, void *param, uint16_t usLen)
{
    uint8_t chain = 0;
    switch (usEvent) {
    case GUI_EVENT_OBJ_INIT:
        if (param != NULL) {
            chain = *(uint8_t *)param;
        } else {
            return ERR_GUI_ERROR;
        }
        return GuiMultiPathCoinReceiveViewInit(chain);
    case GUI_EVENT_OBJ_DEINIT:
        return GuiMultiPathCoinReceiveViewDeInit();
    case GUI_EVENT_DISACTIVE:
        break;
    case GUI_EVENT_REFRESH:
        GuiMultiPathCoinReceiveRefresh();
        break;
    case SIG_SETUP_VIEW_TILE_PREV:
        GuiMultiPathCoinReceivePrevTile();
        break;
    default:
        return ERR_GUI_UNHANDLED;
    }
    return SUCCESS_CODE;
}

GUI_VIEW g_multiPathCoinReceiveView = {
    .id = SCREEN_ETHEREUM_RECEIVE,
    .previous = NULL,
    .isActive = false,
    .optimization = false,
    .pEvtHandler = GuiMultiPathCoinReceiveViewEventProcess,
};
