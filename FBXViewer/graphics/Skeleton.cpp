#include "stdafx.h"
#include "Skeleton.h"
#include "fbx/FBXCommon.h"
#include <algorithm>

Skeleton::Skeleton(void)
{
}

Skeleton::~Skeleton(void)
{
	Destroy();
}

unsigned int Skeleton::AddBone(const std::string& name, int parentId)
{
	for (unsigned int i = 0; i < mBones.size(); ++i)
		if (mBones[i]->mName == name)
			return i;

	Bone* pBone = new Bone();

	pBone->mName = name;
	pBone->mParentId = parentId;
	pBone->mBoneId = (unsigned int)mBones.size();
	mBones.push_back(pBone);
	return pBone->mBoneId;
}

void Skeleton::Destroy()
{
	for (unsigned int i = 0; i < mBones.size(); ++i)
		delete mBones[i];
    mBones.resize(0);
}

unsigned int Skeleton::NumBones() const
{
	return (unsigned int)mBones.size();
}

Bone* Skeleton::GetBone(unsigned int boneId) const
{
	assert(boneId < mBones.size());
    Bone* bone = mBones[boneId];
    assert(boneId == bone->mBoneId);
	return bone;
}

int Skeleton::GetBoneIndex(const std::string& name) const
{
    for (unsigned int i=0; i<mBones.size(); i++)
    {
        if (mBones[i]->mName == name)
        {
            return i;
        }
    }
    return -1;
}

void Skeleton::SetBoneMatrix( unsigned int index, D3DXMATRIX& mat )
{
    Bone* pBone = this->GetBone(index);
    pBone->matBone = mat;
}

D3DXMATRIX Skeleton::GetBoneMatrix( unsigned int index )
{
    Bone* pBone = this->GetBone(index);
    return pBone->matBone;
}

D3DXMATRIX Skeleton::GetParentBoneMatrix( unsigned int index )
{
    Bone* pBone = this->GetBone(index);
    int parentIndex = pBone->mParentId;
    if (parentIndex == -1)
    {
        D3DXMATRIX i;
        D3DXMatrixIdentity(&i);
        return i;
    }
    Bone* pParentBone = GetBone(parentIndex);
    return pParentBone->matBone;
}