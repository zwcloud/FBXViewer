#include "stdafx.h"
#include "Skeleton.h"
#include "FBX/FBXCommon.h"
#include <algorithm>

Skeleton::Skeleton(void) : mMeshType(RIGHTHANDED_ZUP)
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

	// If the parent of this bone is NULL so it is a root bone
	if (parentId < 0)
    {
		mRootBoneIds.push_back(pBone->mBoneId);
    }
	return pBone->mBoneId;
}

void Skeleton::Destroy()
{
	for (unsigned int i = 0; i < mBones.size(); ++i)
		delete mBones[i];
    mBones.resize(0);
    mRootBoneIds.resize(0);
}

unsigned int Skeleton::NumBones() const
{
	return (unsigned int)mBones.size();
}

Bone* Skeleton::GetBone(unsigned int boneId) const
{
	assert(boneId < mBones.size());

	return mBones[boneId];
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

unsigned int Skeleton::NumRootBones() const
{
	return (unsigned int)mRootBoneIds.size();
}

//获取第index个根骨骼的id
unsigned int Skeleton::GetRootBoneId(unsigned int index) const
{
	assert(index < mRootBoneIds.size());
	return mRootBoneIds[index];
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

void Skeleton::Convert( MeshType targetType /*= LEFTHANDED_YUP*/ )
{
    if (mMeshType == RIGHTHANDED_ZUP && targetType == RIGHTHANDED_ZUP)   //这种情况下无需转换
    {
        DebugPrintf("无需转换\n");
        return;
    }

}
