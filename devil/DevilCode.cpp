//
// Created by ewan decima on 11/16/25.
//

#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <fstream>

#pragma comment(lib, "winmm.lib")

// Écrit un buffer dans un fichier
bool writeFile(const std::string& path, const void* data, size_t size) {
    std::ofstream f(path.c_str(), std::ios::binary);
    if (!f) return false;
    f.write((const char*)data, size);
    return f.good();
}

std::string extractMP3FromResource() {
    HRSRC res = FindResource(NULL, MAKEINTRESOURCE(101), RT_RCDATA);
    if (!res) return "";

    HGLOBAL hRes = LoadResource(NULL, res);
    if (!hRes) return "";

    DWORD size = SizeofResource(NULL, res);
    void* data = LockResource(hRes);

    // Fichier temporaire
    char tempPath[MAX_PATH];
    GetTempPathA(MAX_PATH, tempPath);

    std::string out = std::string(tempPath) + "temp_music.mp3";

    if (!writeFile(out, data, size)) return "";

    return out;
}

bool playExtractedMP3(const std::string& path) {
    std::string cmdOpen = "open \"" + path + "\" type mpegvideo alias music";
    if (mciSendString(cmdOpen.c_str(), NULL, 0, NULL))
        return false;

    if (mciSendString("play music", NULL, 0, NULL))
        return false;

    return true;
}

void stopMP3() {
    mciSendString("stop music", NULL, 0, NULL);
    mciSendString("close music", NULL, 0, NULL);
}

int main() {
    std::string mp3Path = extractMP3FromResource();
    if (mp3Path.empty()) {
        MessageBoxA(NULL, "Failed to extract MP3", "Error", MB_OK);
        return 1;
    }

    if (!playExtractedMP3(mp3Path)) {
        MessageBoxA(NULL, "Failed to play MP3", "Error", MB_OK);
        return 1;
    }

    MessageBoxA(NULL, "Music playing.\nClick OK to stop.", "Info", MB_OK);

    stopMP3();
    return 0;
}
