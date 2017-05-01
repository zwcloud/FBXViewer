#include "stdafx.h"
#include <d3dx9.h>
#include <stdio.h>
#include "FBXCommon.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    pManager = FbxManager::Create();
    if( !pManager )
    {
        DebugAssert(false, "Error: Unable to create FBX Manager!\n");
    }
	else DebugPrintf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    pScene = FbxScene::Create(pManager, "My Scene");
	if( !pScene )
    {
        DebugAssert(false, "Error: Unable to create FBX scene!\n");
    }
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    //if( pManager ) pManager->Destroy();
	if( pExitStatus ) DebugPrintf("Program Success!\n");
}

bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    int lMajor, lMinor, lRevision;
    bool lStatus = true;

    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(pManager, "");

    if( pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format in less no ASCII format found
        pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //Try to export in ASCII if possible
        int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

        for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
        {
            if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
            {
                FbxString lDesc =pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                const char *lASCII = "ascii";
                if (lDesc.Find(lASCII)>=0)
                {
                    pFileFormat = lFormatIndex;
                    break;
                }
            }
        } 
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.
    IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
    IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
    IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
    IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
    IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
    IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
    {
        DebugPrintf("Call to FbxExporter::Initialize() failed.\nError returned: %s\n\n",
            lExporter->GetStatus().GetErrorString());
        return false;
    }

    FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
    DebugPrintf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

    // Export the scene.
    lStatus = lExporter->Export(pScene); 

    // Destroy the exporter.
    lExporter->Destroy();
    return lStatus;
}

bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(pManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        DebugPrintf("Call to FbxImporter::Initialize() failed.\n");
        DebugPrintf("Error returned: %s file:%s \n\n", error.Buffer(), pFilename);

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            DebugPrintf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            DebugPrintf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
        }

        return false;
    }

    DebugPrintf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        DebugPrintf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        DebugPrintf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        DebugPrintf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        DebugPrintf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        DebugPrintf("\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            DebugPrintf("    Animation Stack %d\n", i);
            DebugPrintf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            DebugPrintf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            DebugPrintf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            DebugPrintf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            DebugPrintf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(pScene);

    if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
    {
        DebugPrintf("Please enter password: ");

        lPassword[0] = '\0';

        FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
        DebugAssert(false, "未实现密码输入\n");
        scanf("%s", lPassword); //TODO 从对话框获取输入
        FBXSDK_CRT_SECURE_NO_WARNING_END

        FbxString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(pScene);

        if(lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
        {
            DebugPrintf("\nPassword is wrong, import aborted.\n");
        }
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

void LoadFirstFbxMesh(FbxScene* pScene, FbxMesh** pMesh)
{
    FbxNode* lNode = pScene->GetRootNode();
    DebugAssert(NULL != lNode, "Null root node" );
    // search for the fist FbxMesh
    for(int i = 0; i < lNode->GetChildCount(); i++)
    {
        FbxNode* pChildNode = lNode->GetChild(i);
        FbxNodeAttribute::EType lAttributeType;
        if (NULL == pChildNode->GetNodeAttribute())
        {
            continue;
        }
        else
        {
            lAttributeType = (pChildNode->GetNodeAttribute()->GetAttributeType());
            if (lAttributeType == FbxNodeAttribute::eMesh)  // found
            {
                *pMesh = (FbxMesh*)pChildNode->GetNodeAttribute();
                return;
            }
        }
    }
}

FbxPose* GetFirstPose(FbxScene* pScene)
{
    unsigned int lPoseCount = pScene->GetPoseCount();

    for (unsigned int i = 0; i < lPoseCount; i++)
    {
        FbxPose* lPose = pScene->GetPose(i);
        return lPose;
    }

    return NULL;
}

// Get the global position of the node for the current pose.
// If the specified node is not part of the pose or no pose is specified, get its
// global position at the current time.
FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
    FbxAMatrix lGlobalPosition;
    bool        lPositionFound = false;

    if (pPose)
    {
        int lNodeIndex = pPose->Find(pNode);

        if (lNodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
            {
                lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                FbxAMatrix lParentGlobalPosition;

                if (pParentGlobalPosition)
                {
                    lParentGlobalPosition = *pParentGlobalPosition;
                }
                else
                {
                    if (pNode->GetParent())
                    {
                        lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
                    }
                }

                FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
                lGlobalPosition = lParentGlobalPosition * lLocalPosition;
            }

            lPositionFound = true;
        }
    }

    if (!lPositionFound)
    {
        // There is no pose entry for that node, get the current global position instead.

        // Ideally this would use parent global position and local position to compute the global position.
        // Unfortunately the equation 
        //    lGlobalPosition = pParentGlobalPosition * lLocalPosition
        // does not hold when inheritance type is other than "Parent" (RSrs).
        // To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
        lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
    }

    return lGlobalPosition;
}

// Get the matrix of the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
    FbxAMatrix lPoseMatrix;
    FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

    memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

    return lPoseMatrix;
}

FbxAMatrix GetGeometryTransformation(FbxNode* pNode)
{
    const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

    return FbxAMatrix(lT, lR, lS);
}

FbxAMatrix GetLocalTransformation(FbxNode* pNode)
{
    const FbxVector4 lT = FbxVector4(pNode->LclTranslation);
    FbxQuaternion lQ;
    lQ.ComposeSphericalXYZ( FbxVector4(pNode->LclRotation.Get()) ); //欧拉角转换为四元数
    const FbxVector4 lS = FbxVector4(pNode->LclScaling);

    return FbxAMatrix(lT, lQ, lS);
}

D3DXMATRIX FbxAMatrix_to_D3DXMATRIX(const FbxAMatrix& lMatrix)
{
	D3DXMATRIX mat(
		(float)lMatrix.Get(0, 0), (float)lMatrix.Get(0, 1), (float)lMatrix.Get(0, 2), (float)lMatrix.Get(0, 3),
		(float)lMatrix.Get(1, 0), (float)lMatrix.Get(1, 1), (float)lMatrix.Get(1, 2), (float)lMatrix.Get(1, 3),
		(float)lMatrix.Get(2, 0), (float)lMatrix.Get(2, 1), (float)lMatrix.Get(2, 2), (float)lMatrix.Get(2, 3),
		(float)lMatrix.Get(3, 0), (float)lMatrix.Get(3, 1), (float)lMatrix.Get(3, 2), (float)lMatrix.Get(3, 3));
    return mat;
}

FbxAMatrix D3DXMATRIX_to_FbxAMatrix(const D3DXMATRIX& lMatrix)
{
    FbxAMatrix mat;
    mat.SetRow(0, FbxVector4(lMatrix._11,lMatrix._12,lMatrix._13,lMatrix._14));
    mat.SetRow(1, FbxVector4(lMatrix._21,lMatrix._22,lMatrix._23,lMatrix._24));
    mat.SetRow(2, FbxVector4(lMatrix._31,lMatrix._32,lMatrix._33,lMatrix._34));
    mat.SetRow(3, FbxVector4(lMatrix._41,lMatrix._42,lMatrix._43,lMatrix._44));
    return mat;
}
