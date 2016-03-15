#pragma once
#include <fbxsdk.h>

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
void LoadFirstFbxMesh(FbxScene* pScene, FbxMesh** pMesh);
FbxPose* GetFirstPose(FbxScene* pScene);
FbxAMatrix GetGlobalPosition(FbxNode* pNode, 
    const FbxTime& pTime, 
    FbxPose* pPose = NULL,
    FbxAMatrix* pParentGlobalPosition = NULL);
FbxAMatrix GetPoseMatrix(FbxPose* pPose, 
    int pNodeIndex);
FbxAMatrix GetGeometryTransformation(FbxNode* pNode);
FbxAMatrix GetLocalTransformation(FbxNode* pNode);
D3DXMATRIX FbxAMatrix_to_D3DXMATRIX(const FbxAMatrix& lMatrix);
FbxAMatrix D3DXMATRIX_to_FbxAMatrix(const D3DXMATRIX& lMatrix);
