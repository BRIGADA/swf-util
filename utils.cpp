#include "utils.h"

#include <stdarg.h>
#include <stdio.h>

std::string stringf(std::string format, ...) {

    va_list ap;
    va_start(ap, format);
    int buf_length = vsnprintf(NULL, 0, format.data(), ap);
    va_end(ap);

    char * buf = new char[buf_length + 1];
    va_start(ap, format);
    vsnprintf(buf, buf_length + 1, format.data(), ap);
    va_end(ap);
    std::string result(buf, buf_length);
    delete [] buf;
    return result;
}

void DEBUG(std::string format, ...) {
    va_list ap;
    va_start(ap, format);
    vprintf(format.data(), ap);
    printf("\n");
    va_end(ap);
}

std::string TagTypeAsString(uint8_t type) {
    switch (type) {
        case 0: return "End";
        case 1: return "ShowFrame";
        case 2: return "DefineShape";
        case 4: return "PlaceObject";
        case 5: return "RemoveObject";
        case 6: return "DefineBits";
        case 7: return "DefineButton";
        case 8: return "JPEGTables";
        case 9: return "SetBackgroundColor";
        case 10: return "DefineFont";
        case 11: return "DefineText";
        case 12: return "DoAction";
        case 13: return "DefineFontInfo";
        case 14: return "DefineSound";
        case 15: return "StartSound";
        case 17: return "DefineButtonSound";
        case 18: return "SoundStreamHead";
        case 19: return "SoundStreamBlock";
        case 20: return "DefineBitsLossless";
        case 21: return "DefineBitsJPEG2";
        case 22: return "DefineShape2";
        case 23: return "DefineButtonCxform";
        case 24: return "Protect";
        case 26: return "PlaceObject2";
        case 28: return "RemoveObject2";
        case 32: return "DefineShape3";
        case 33: return "DefineText2";
        case 34: return "DefineButton2";
        case 35: return "DefineBitsJPEG3";
        case 36: return "DefineBitsLossless2";
        case 37: return "DefineEditText";
        case 39: return "DefineSprite";
        case 43: return "FrameLabel";
        case 45: return "SoundStreamHead2";
        case 46: return "DefineMorphShape";
        case 48: return "DefineFont2";
        case 56: return "ExportAssets";
        case 57: return "ImportAssets";
        case 58: return "EnableDebugger";
        case 59: return "DoInitAction";
        case 60: return "DefineVideoStream";
        case 61: return "VideoFrame";
        case 62: return "DefineFontInfo2";
        case 64: return "EnableDebugger2";
        case 65: return "ScriptLimits";
        case 66: return "SetTabIndex";
        case 69: return "FileAttributes";
        case 70: return "PlaceObject3";
        case 71: return "ImportAssets2";
        case 73: return "DefineFontAlignZones";
        case 74: return "CSMTextSettings";
        case 75: return "DefineFont3";
        case 76: return "SymbolClass";
        case 77: return "Metadata";
        case 78: return "DefineScalingGrid";
        case 82: return "DoABC";
        case 83: return "DefineShape4";
        case 84: return "DefineMorphShape2";
        case 86: return "DefineSceneAndFrameLabelData";
        case 87: return "DefineBinaryData";
        case 88: return "DefineFontName";
        case 89: return "StartSound2";
        case 90: return "DefineBitsJPEG4";
        case 91: return "DefineFont4";
        default: return stringf("UNKNOWN TAG TYPE (%u)", type);
    }
}

std::string implode(const std::string &delim, const std::vector<std::string> &items) {
    std::string result;
    for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
        if (!result.empty()) result += delim;
        result += *it;
    }
    return result;
}

std::string string_escape(std::string str)
{
    str = string_replace("\\", "\\\\", str);
    str = string_replace("\'", "\\'", str);
    str = string_replace("\"", "\\\"", str);
    str = string_replace("\n", "\\n", str);
    str = string_replace("\r", "\\r", str);
    str = string_replace("\t", "\\t", str);
    return str;
}

std::string string_replace(std::string from, std::string to, std::string subject)
{
//    DEBUG(__FUNCTION__);
    std::string::size_type pos = 0;
    while((pos = subject.find(from, pos)) != std::string::npos) {
        subject.replace(pos, from.size(), to);
        pos += to.size();
    }
    return subject;
}