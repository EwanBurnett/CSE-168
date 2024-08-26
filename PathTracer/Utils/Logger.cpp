#include "Logger.h"
#include <assert.h>
#if WIN32
#include <Windows.h>
#endif


/**
 * @brief Prints a formatted message in White.
*/
void EDX::Log::Print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::WHITE, stdout, fmt, args);
    va_end(args);
}

/**
  * @brief Only prints when __DEBUG is defined.
 */
void EDX::Log::Debug(const char* fmt, ...)
{
#if DEBUG | _DEBUG
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::LIGHTGREEN, stdout, "[Debug] ");
    _Output(ELogColour::LIGHTGREEN, stdout, fmt, args);
    va_end(args);
#endif
}

/**
 * @brief Prints a Status message.
*/
void EDX::Log::Status(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::LIGHTCYAN, stdout, fmt, args);
    va_end(args);
}

/**
 * @brief Prints a Success message.
 * @param fmt
 * @param
*/
void EDX::Log::Success(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::GREEN, stdout, fmt, args);
    va_end(args);
}

/**
 * @brief Prints a Failure message.
*/
void EDX::Log::Failure(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::BROWN, stdout, fmt, args);
    va_end(args);
}

/**
 * @brief Prints a Warning Message.
*/
void EDX::Log::Warning(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::YELLOW, stdout, "Warning\n");
    _Output(ELogColour::YELLOW, stdout, fmt, args);
    va_end(args);
}

/**
 * @brief Prints an Error message.
 * @param file The caller file. Pass `__FILE__`.
 * @param line The caller line. Pass `__LINE__`.
 * @param function The caller function. Pass `__PRETTY_FUNCTION__`.
*/
void EDX::Log::Error(const char* file, int line, const char* function, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::LIGHTRED, stderr, "Error\nFILE: %s\n\tLINE: %d\n\tFUNCTION: %s\n", file, line, function);
    _Output(ELogColour::LIGHTRED, stderr, fmt, args);
    va_end(args);
}

/**
 * @brief Prints a Fatal Error message. This also triggers a Breakpoint.
 * @param file The caller file. Pass `__FILE__`.
 * @param line The caller line. Pass `__LINE__`.
 * @param function The caller function. Pass `__PRETTY_FUNCTION__`.
*/
void EDX::Log::Fatal(const char* file, int line, const char* function, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(ELogColour::RED, stderr, "\nFATAL ERROR\nFILE: %s\n\tLINE: %d\n\tFUNCTION: %s\n", file, line, function);
    _Output(ELogColour::RED, stderr, fmt, args);
    va_end(args);
    
    assert(false); 
}


void EDX::Log::_Output(ELogColour colour, FILE* stream, const char* fmt, va_list args) {

     //Change the output colour 
#if WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)colour);
#else //Assume the program is running on linux
    switch (colour) {
    case ELogColour::BLACK:
        fprintf(stream, "\u001b[30m");
        break;
    case ELogColour::LIGHTBLUE:
    case ELogColour::BLUE:
        fprintf(stream, "\u001b[34m");
        break;
    case ELogColour::LIGHTGREEN:
    case ELogColour::GREEN:
        fprintf(stream, "\u001b[32m");
        break;
    case ELogColour::LIGHTCYAN:
    case ELogColour::CYAN:
        fprintf(stream, "\u001b[36m");
        break;
    case ELogColour::LIGHTRED:
    case ELogColour::RED:
        fprintf(stream, "\u001b[31m");
        break;
    case ELogColour::LIGHTMAGENTA:
    case ELogColour::MAGENTA:
        fprintf(stream, "\u001b[35m");
        break;
    case ELogColour::BROWN:
    case ELogColour::YELLOW:
        fprintf(stream, "\u001b[33m");
        break;
    case ELogColour::LIGHTGRAY:
    case ELogColour::DARKGRAY:
    case ELogColour::WHITE:
    default:
        fprintf(stream, "\u001b[0m");
        break;
    }
#endif
    va_list vargs;
    va_copy(vargs, args);
    vfprintf(stream, fmt, vargs);

    //Reset the output colour
#if WIN32
    SetConsoleTextAttribute(hConsole, (WORD)ELogColour::WHITE);
#else
    fprintf(stream, "\u001b[0m");
#endif
}

void EDX::Log::_Output(ELogColour colour, FILE* stream, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    _Output(colour, stream, fmt, args);
    va_end(args);
}

