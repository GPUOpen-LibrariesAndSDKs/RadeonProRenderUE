/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#pragma once
#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "IDetailsView.h"
#include "RPRMeshDataContainer.h"

DECLARE_DELEGATE_RetVal(FRPRMeshDataContainerPtr, FGetRPRMeshDatas)

class RPRSECTIONSMANAGER_API SRPRSectionsManager : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SRPRSectionsManager) {}
		SLATE_EVENT(FGetRPRMeshDatas, GetRPRMeshDatas)
		SLATE_EVENT(FSimpleDelegate, OnMaterialChanged)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

private:

	void	InitializeModeDetails();
	void	InitializeMeshesSectionsDetails();
	void	OnMaterialChanged(UStaticMesh* StaticMesh);
	FRPRMeshDataPtr GetRPRMeshData(UStaticMesh* StaticMesh);

	TSharedRef<IDetailCustomization> MakeStaticMeshDetails();

private:

	TSharedPtr<IDetailsView> RPRModeDetails;
	TSharedPtr<IDetailsView> MeshDetails;

	FGetRPRMeshDatas GetRPRMeshDatas;
	FSimpleDelegate MaterialChanged;

};
