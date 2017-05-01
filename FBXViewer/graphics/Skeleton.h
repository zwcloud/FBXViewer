#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include "fbxsdk.h"

struct Bone
{
    std::string				    mName;      // Name
    unsigned int				mBoneId;    // ID  //!!Bone的ID即其在Skeleton的索引值
    int                         mParentId;  // Parent bone ID
    std::vector< unsigned int > mChildIDs;  // Children bone ID

    D3DXMATRIX matBone;     //bone matrix at bind pose
    D3DXMATRIX matOffset;   //bone offset when animated

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
    Skeleton( const Skeleton& );
    Skeleton& operator = (const Skeleton& );
public:
    Skeleton();
    ~Skeleton();

    std::vector<Bone*>      mBones; //!!Bone的ID即其在Skeleton的索引值
    std::vector<FbxNode*>   mFbxNodes;
    std::vector<int>        mRootBoneIds;

    unsigned int AddBone(const std::string& name, int parentId);
    void Destroy();

    unsigned int NumBones() const;
    Bone* GetBone(unsigned int boneId) const;
    int GetBoneIndex(const std::string& name) const;
    unsigned int GetRootBoneId(size_t index) const;
    unsigned int NumRootBones() const;
    D3DXMATRIX GetBoneMatrix(unsigned int index);
    D3DXMATRIX GetParentBoneMatrix(unsigned int index);
    void SetBoneMatrix( unsigned int index, D3DXMATRIX& mat );

    enum MeshType{RIGHTHANDED_ZUP, LEFTHANDED_YUP} mMeshType;
    void Convert(MeshType targetType = LEFTHANDED_YUP);
};
