#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include "fbxsdk.h"

struct Bone
{
    std::string mName;      // Name
    unsigned int mBoneId;    // ID  //!!Bone的ID即其在Skeleton的索引值
    int mParentId;  // Parent bone ID
    std::vector< unsigned int > mChildIDs;  // Children bone ID

    D3DXMATRIX matBone;     //[world space] bone matrix at bind pose
    D3DXMATRIX matOffset;   //bone offset when animated, used when rendering

    bool IsRoot() { return this->mParentId < 0; }

    Bone() : mName("<unnamed>"), mBoneId(0), mParentId(0)
    {
        mName.resize(0);
        mChildIDs.resize(0);
        D3DXMatrixIdentity(&matBone);
        D3DXMatrixIdentity(&matOffset);
    }
};

class Skeleton
{
    Skeleton( const Skeleton& ) = delete;
    Skeleton& operator = (const Skeleton& ) = delete;
public:
    Skeleton();
    ~Skeleton();

    std::vector<Bone*>      mBones; //!!Bone的ID即其在Skeleton的索引值

    unsigned int NumBones() const;
    unsigned int AddBone(const std::string& name, int parentId);
    Bone* GetBone(unsigned int boneId) const;
    int GetBoneIndex(const std::string& name) const;
    D3DXMATRIX GetBoneMatrix(unsigned int index);
    D3DXMATRIX GetParentBoneMatrix(unsigned int index);
    void SetBoneMatrix( unsigned int index, D3DXMATRIX& mat );
    void Destroy();
};
