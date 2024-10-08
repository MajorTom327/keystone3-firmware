#ifndef _GUI_LOCK_WIDGETS_H
#define _GUI_LOCK_WIDGETS_H

typedef enum {
    LOCK_SCREEN_PURPOSE_UNLOCK,
    LOCK_SCREEN_PURPOSE_VERIFY,
    LOCK_SCREEN_PURPOSE_SET,

    LOCK_SCREEN_PURPOSE_BUTT,
} LOCK_SCREEN_PURPOSE_ENUM;

void GuiLockScreenUpdatePurpose(LOCK_SCREEN_PURPOSE_ENUM);

void GuiLockScreenInit(void *param);
void GuilockScreenRefresh(void);
void GuiLockScreenPassCode(bool result);
void GuiLockScreenTurnOnHandler(lv_event_t *e);
void GuiLockScreenTurnOffHandler(lv_event_t *e);
void GuiLockScreenTurnOff(void);
void GuiLockScreenTurnOn(void *param);
bool GuiLockScreenIsTop(void);
void GuiLockScreenUpdatePassCode(void);
void OpenForgetPasswordHandler(lv_event_t *e);
void GuiLockScreenHidden(void);
void GuiUpdateOldAccountIndex(void);
void GuiLockScreenClearFirstUnlock(void);
bool GuiLockScreenIsFirstUnlock(void);
void GuiFpRecognizeResult(bool en);
void GuiLockScreenPasscodeSwitch(bool isPin);
void GuiLockScreenSetFirstUnlock(void);
void GuiLockViewRefreshLanguage(void);

void GuiLockScreenErrorCount(void *param);
void GuiLockScreenToHome(void);
void GuiJumpToLockDevicePage(void);
void GuiLockScreenFpRecognize(void);
bool GuiNeedFpRecognize(void);

void GuiShowGenerateXPubLoading(void);
void GuiHideGenerateXPubLoading(void);
void GuiLockViewRefreshLanguage(void);
#endif /* _GUI_LOCK_WIDGETS_H */


