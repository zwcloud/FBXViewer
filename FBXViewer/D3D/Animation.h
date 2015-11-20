#pragma once
#include <vector>
#include <d3dx9.h>

class Animation
{
private:
    Animation(Animation&);
    void operator = (Animation&);

    struct Frame
    {
        unsigned int Time;
        D3DXMATRIX TransformMat;
    };

    unsigned int mDuration;
    std::map<unsigned int, std::string> mBoneNames;
    std::map<unsigned int, std::vector<Frame> > mFrameMap;

public:
    Animation();
    ~Animation();

    void AddFrame(unsigned int boneIndex, const char* boneName, unsigned int time, const D3DXMATRIX& transformMat);
    D3DXMATRIX GetFrame(unsigned int boneIndex, unsigned int time);
    unsigned int GetDuration(){return mDuration+1;}
    //Debug
    void Dump(bool printT, bool printR);
};