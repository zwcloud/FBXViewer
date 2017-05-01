#pragma once
class RenderSettings
{
public:
    static RenderSettings& getInstance()
    {
        static RenderSettings instance;
        return instance;
    }
private:
    RenderSettings();
public:
    RenderSettings(RenderSettings const&) = delete;
    void operator=(RenderSettings const&) = delete;

private:
    //lighting
    D3DXCOLOR ambient;
    D3DXCOLOR diffuse;
    D3DXCOLOR specular;
    float specularPower;
    D3DXVECTOR3 lightPos;
    D3DXVECTOR3 attenuation012;

public:
    D3DXCOLOR& Ambient() { return ambient; }
    D3DXCOLOR& Diffuse() { return diffuse; }
    D3DXCOLOR& Specular() { return diffuse; }
    float& SpecularPower() { return specularPower; }
    D3DXVECTOR3& LightPos() { return lightPos; } //point light position
    D3DXVECTOR3& Attenuation012() { return attenuation012; } //light attenuation
};
