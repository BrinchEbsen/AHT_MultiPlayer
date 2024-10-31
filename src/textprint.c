#include <common.h>
#include <colors.h>
#include <symbols.h>
#include <hashcodes.h>

void textPrint(char* pText, wchar_t* pTextW, u16 x, u16 y, TextAlign Align, XRGBA* textCol, float Scale) {
    //Drop shadow
    XWnd_SetText(gpPanelWnd, HT_File_Panel, HT_Font_Test, &COLOR_BLACK, Scale, Align);
    if (pTextW == 0) {
        XWnd_FontPrint(gpPanelWnd, x+2, y+3, pText, Scale, Align, true);
    } else {
        XWnd_FontPrintW(gpPanelWnd, x+2, y+3, pTextW, Scale, Align, true);
    }

    //Actual text
    XWnd_SetText(gpPanelWnd, HT_File_Panel, HT_Font_Test, textCol, Scale, Align);
    if (pTextW == 0) {
        XWnd_FontPrint(gpPanelWnd, x, y, pText, Scale, Align, true);
    } else {
        XWnd_FontPrintW(gpPanelWnd, x, y, pTextW, Scale, Align, true);
    }
}

void textSmpPrint(char* pText, wchar_t* pTextW, u16 x, u16 y) {
    textPrint(pText, pTextW, x, y, TopLeft, &COLOR_TEXT, 1.0f);
}

void textPrintF(u16 x, u16 y, TextAlign Align, XRGBA* textCol, float Scale, char* pText, ...) {
    va_list args;
    va_start(args, pText);
    
    char c[32];
    ig_vsprintf(c, pText, args);
    textPrint(c, 0, x, y, Align, textCol, Scale);

    va_end(args);
}

void textSmpPrintF(u16 x, u16 y, char* pText, ...) {
    va_list args;
    va_start(args, pText);
    
    char c[32];
    ig_vsprintf(c, pText, args);
    textSmpPrint(c, 0, x, y);

    va_end(args);
}