#ifndef __BLINNPHONG_H
#define __BLINNPHONG_H
/**
 * @file BlinnPhong.h
 * @brief Blinn-Phong Material
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-29
*/
#include "../Colour.h"
namespace EDX {
    struct BlinnPhong {
        Colour ambient; 
        Colour diffuse; 
        Colour specular; 
        Colour emission; 
        float shininess; 
    };
}
#endif