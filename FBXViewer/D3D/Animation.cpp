#include "stdafx.h"
#include "Animation.h"
#include "FBX/FBXCommon.h"

static D3DXMATRIX IdentityMatrix;

Animation::Animation() : mDuration(0)
{
    D3DXMatrixIdentity(&IdentityMatrix);
}

Animation::~Animation()
{

}

void Animation::AddFrame( unsigned int boneIndex, const char* boneName, unsigned int time, const D3DXMATRIX& transformMat )
{
    Frame lFrame;
    lFrame.Time = time;
    mDuration = (mDuration > time ? mDuration : time);
    lFrame.TransformMat = transformMat;
    mFrameMap[boneIndex].push_back(lFrame);
    mBoneNames[boneIndex] = boneName;
}

D3DXMATRIX Animation::GetFrame( unsigned int boneIndex, unsigned int time )
{
    const std::vector<Frame>& boneFrames = mFrameMap.at(boneIndex);
    unsigned int nboneFrames = boneFrames.size();
    for (unsigned int i=0; i<nboneFrames; i++)
    {
        const Frame& lFrame = boneFrames.at(i);
        if (lFrame.Time == time)
        {
#if 1
            {
                DebugPrintf("Getting frame for bone <%s>:\n", mBoneNames.at(boneIndex).c_str());
                DebugPrintf("    frame time: %d\n", i, lFrame.Time);
                const D3DXMATRIX& mat = lFrame.TransformMat;
                DebugPrintf("    T(%.3f, %.3f, %.3f)\n", mat._41, mat._42, mat._43);
                FbxAMatrix fbxmat = D3DXMATRIX_to_FbxAMatrix(mat);
                FbxVector4 v = fbxmat.GetR();
                DebugPrintf("    R(%.3f, %.3f, %.3f)\n", v[0], v[1], v[2]);
                DebugPrintf("---------------------------\n");
            }
#endif
            return lFrame.TransformMat;
        }
    }
    DebugPrintf("没有找到ID为%d的骨骼在%d时的变换矩阵\n", boneIndex, time);
    return IdentityMatrix;
}

void Animation::Dump(bool printT, bool printR)
{
    if (!(printT || printR))
    {
        return;
    }
    std::map<unsigned int, std::vector<Frame>>::const_iterator it;
    std::map<unsigned int, std::vector<Frame>>::const_iterator it_begin = mFrameMap.begin();
    std::map<unsigned int, std::vector<Frame>>::const_iterator it_end = mFrameMap.end();
    for (it = it_begin; it!=it_end; it++)
    {
        unsigned int lBoneIndex = it->first;
        DebugPrintf("Bone \"%s\"(%d):\n", mBoneNames.at(lBoneIndex).c_str(), lBoneIndex);
        const std::vector<Frame>& lBoneFrames = it->second;
        unsigned int nboneFrames = lBoneFrames.size();
        for (unsigned int i=0; i<nboneFrames; i++)
        {
            const Frame& lFrame = lBoneFrames.at(i);
            DebugPrintf("    frame(%d) time: %d\n", i, lFrame.Time);
            const D3DXMATRIX& mat = lFrame.TransformMat;
            if (printT)
            {
                DebugPrintf("    T(%.3f, %.3f, %.3f)\n", mat._41, mat._42, mat._43);
            }
            if (printR)
            {
                FbxAMatrix fbxmat = D3DXMATRIX_to_FbxAMatrix(mat);
                FbxVector4 v = fbxmat.GetR();
                DebugPrintf("    R(%.3f, %.3f, %.3f)\n", v[0], v[1], v[2]);
            }
            DebugPrintf("---------------------------\n");
        }
    }
}
