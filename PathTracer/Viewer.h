#ifndef __VIEWER_H
#define __VIEWER_H

#include "Viewer/Window.h"
#include "Viewer/vkengine.h"

namespace EDX {
    class Viewer {
    public:
        void Init(); 
        void Shutdown(); 

        bool PollEvents(); 
        void Update();
    private:
        Window m_Window; 
        VkEngine m_Engine; 
    };
}

#endif