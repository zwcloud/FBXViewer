#include "stdafx.h"
#include "RenderSettings.h"

RenderSettings::RenderSettings()
{
    ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
    diffuse = D3DXCOLOR(0.5f, 0.7f, 0.0f, 1.0f);
    specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    specularPower = 5.0f;

    lightPos = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
    attenuation012 = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
}
