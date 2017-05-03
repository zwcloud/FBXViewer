#include "stdafx.h"
#include "CppUnitTest.h"
#include "fbx\FBXCommon.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <algorithm>
#include <iterator>
#include "common\util.h"
#include "Log.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(MakeSureFbxAMatrixIsTheSameAsD3DXMATRIX)
		{
            float translation[3] = { 1,2,3 };
            float scale[3] = { 1,1,1 };

            FbxAMatrix fbxMat(
                FbxVector4(translation[0], translation[1], translation[2]),
                FbxVector4(0, 0, 0),
                FbxVector4(scale[0], scale[1], scale[2]));

            D3DXMATRIX translationMat;
            D3DXMatrixTranslation(&translationMat, translation[0], translation[1], translation[2]);
            D3DXMATRIX scalingMat;
            D3DXMatrixScaling(&scalingMat, scale[0], scale[1], scale[2]);
            D3DXMATRIX d3dMat = translationMat * scalingMat;

            LogFormat(
                "FbxAMatrix:\n"
                "%.2f, %.2f, %.2f, %.2f,\n"
                "%.2f, %.2f, %.2f, %.2f,\n"
                "%.2f, %.2f, %.2f, %.2f,\n"
                "%.2f, %.2f, %.2f, %.2f,\n",
                fbxMat[0][0], fbxMat[0][1], fbxMat[0][2], fbxMat[0][3],
                fbxMat[1][0], fbxMat[1][1], fbxMat[1][2], fbxMat[1][3],
                fbxMat[2][0], fbxMat[2][1], fbxMat[2][2], fbxMat[2][3],
                fbxMat[3][0], fbxMat[3][1], fbxMat[3][2], fbxMat[3][3]);

            LogFormat(
                "D3DXMATRIX:\n"
                "%.2f, %.2f, %.2f, %.2f,\n"
                "%.2f, %.2f, %.2f, %.2f,\n"
                "%.2f, %.2f, %.2f, %.2f,\n"
                "%.2f, %.2f, %.2f, %.2f,\n",
                d3dMat.m[0][0], d3dMat.m[0][1], d3dMat.m[0][2], d3dMat.m[0][3],
                d3dMat.m[1][0], d3dMat.m[1][1], d3dMat.m[1][2], d3dMat.m[1][3],
                d3dMat.m[2][0], d3dMat.m[2][1], d3dMat.m[2][2], d3dMat.m[2][3],
                d3dMat.m[3][0], d3dMat.m[3][1], d3dMat.m[3][2], d3dMat.m[3][3]);

            bool result = false;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    result = floatEqual(fbxMat[i][j], d3dMat.m[i][j], 10e-6);
                }
            }
            Assert::IsTrue(result, L"FbxAMatrix is not the same as D3DXMATRIX");
		}

	};
}