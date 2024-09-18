#ifndef __LOGGER_H
#define __LOGGER_H
/**
 * @file Logger.h 
 * @brief Console Logging Utility
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-26
*/
#include <cstdio>
#include <cstdarg>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
namespace EDX {
    /**
     * @brief Class wrapper for Logging Functions
    */
    class Log {
    public:
        enum class ELogColour {
            BLACK = 0,
            BLUE,
            GREEN,
            CYAN,
            RED,
            MAGENTA,
            BROWN,
            LIGHTGRAY,
            DARKGRAY,
            LIGHTBLUE,
            LIGHTGREEN,
            LIGHTCYAN,
            LIGHTRED,
            LIGHTMAGENTA,
            YELLOW,
            WHITE,
        };

        static void Print(const char* fmt, ...);
        static void Debug(const char* fmt, ...);
        static void Status(const char* fmt, ...);
        static void Success(const char* fmt, ...);
        static void Failure(const char* fmt, ...);
        static void Warning(const char* fmt, ...);
        static void Error(const char* file, int line, const char* function, const char* fmt, ...);
        static void Fatal(const char* file, int line, const char* function, const char* fmt, ...);

    private:
        static void _Output(ELogColour colour, FILE* stream, const char* fmt, va_list args);
        static void _Output(ELogColour colour, FILE* stream, const char* fmt, ...);
    };
}

#endif