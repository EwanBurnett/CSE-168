#ifndef __PROGRESSBAR_H
#define __PROGRESSBAR_H
/**
 * @file ProgressBar.h
 * @brief Progress Bar Utility
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-26
*/
#include <cstdint>
#include <cstdio> 
#include <string>

#include "Timer.h"

namespace EDX {
    class ProgressBar {
    public:
        /**
         * @brief Constructs a Progress Bar.
         * @param width Number of characters wide. 40 by default.
         * @param progressMarker Character used to represent progress. '|' by default.
        */
        explicit ProgressBar(uint8_t width = 40, char progressMarker = '|') {
            m_Width = width;
            m_Progress = 0.0;

            //For sprintf, we have to allocate buffers...
            m_ProgressMarkerBuffer = new char[m_Width];
            memset(m_ProgressMarkerBuffer, progressMarker, m_Width);
            m_ProgressTodoBuffer = new char[m_Width];
            memset(m_ProgressTodoBuffer, ' ', m_Width);
        }

        ~ProgressBar() {
            delete[] m_ProgressMarkerBuffer;
            delete[] m_ProgressTodoBuffer;
        }

        /**
         * @brief Updates the progress bar
         * @param progress Progress %, bounded [0, 1].
        */
        void Update(const float progress) {
            //Start the timer if necessary
            if (m_Progress <= 0.0 + FLT_EPSILON) {
                m_Timer.Start();
            }

            if (progress > 1.0f) {
                m_Progress = 1.0f;
            }
            else if (progress < 0.0f) {
                m_Progress = 0.0f;
            }
            else {
                m_Progress = progress;
            }

            //Write the progress string to an internal buffer before outputting to the console. 
            char buffer[0xff];  //Yes, this is unsafe.  
            sprintf(buffer, "%03.2f \%\t[", m_Progress);
            const size_t len = strlen(buffer);

            const uint8_t progressBarWidth = m_Width * m_Progress;
            sprintf(buffer + len, "%.*s", progressBarWidth, m_ProgressMarkerBuffer);
            sprintf(buffer + len + progressBarWidth, "%.*s", m_Width - progressBarWidth, m_ProgressTodoBuffer);
            sprintf(buffer + len + m_Width, "] %03.3fs Elapsed", m_Timer.Duration());

            printf("%s\r", buffer);
            m_Timer.Tick();
        }

        const Timer& GetProgressTimer() {
            return m_Timer;
        }

    private:
        Timer m_Timer;
        float m_Progress;
        uint8_t m_Width;
        char* m_ProgressMarkerBuffer;
        char* m_ProgressTodoBuffer;
    };
}
#endif