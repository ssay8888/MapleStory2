#include "pch.h"
#include "mesh_static.h"

#include <iostream>

#include "src/utility/components/shader/shader.h"

MeshStatic::MeshStatic(const ComPtr<IDirect3DDevice9>& device):
	Mesh(device)
{
}

auto MeshStatic::NativeConstructPrototype(const std::wstring& filePath, const std::wstring& fileName) -> HRESULT
{
	wchar_t		szFullPath[MAX_PATH] = TEXT("");

	lstrcpy(szFullPath, filePath.c_str());
	lstrcat(szFullPath, fileName.c_str());
	LPD3DXMESH	mesh = nullptr;

	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, _graphic_device.Get(), &_adjacency, &_buffer_material, nullptr, &_num_materials, &mesh)))
		return E_FAIL;

	_mesh.Attach(mesh);

	/* ∏ﬁΩ√¿« µ«ª¡Ó ∏  + ≥Î∏÷∏  + Ω∫∆Â≈ß∑Ø∏  ¿ª ∑ŒµÂ«œ≥Æ. */
	for (uint32_t i = 0; i < _num_materials; ++i)
	{
		const D3DXMATERIAL* pMaterial = static_cast<D3DXMATERIAL*>(_buffer_material->GetBufferPointer()) + i;

		MeshMaterialTexture* pMeshMaterialTexture = new MeshMaterialTexture;
		ZeroMemory(pMeshMaterialTexture, sizeof(MeshMaterialTexture));

		if (pMaterial->pTextureFilename == nullptr)
		{
			std::wstring str = szFullPath;
			if (str.find(L"co_fluid_water_C01.X") != std::wstring::npos)
			{
				wchar_t		szTextureFileName[MAX_PATH] = TEXT("");
				/* char -> tchar */
				wchar_t	szFileName[MAX_PATH] = TEXT("");

				/* µ«ª¡Ó ∏  ∑ŒµÂ */
				wsprintf(szFileName, szTextureFileName, L"D");

				lstrcpy(szFullPath, filePath.c_str());
				lstrcat(szFullPath, L"co_fluid_water_c01.dds");
				auto result = D3DXCreateTextureFromFile(_graphic_device.Get(), szFullPath, &pMeshMaterialTexture->diffuse_map);

				_materials.push_back(pMeshMaterialTexture);

				continue;
			}
			std::wcout << szFullPath << std::endl;
			continue;
		}
		wchar_t		szTextureFileName[MAX_PATH] = TEXT("");
		/* char -> tchar */
		MultiByteToWideChar(CP_ACP, 
			0, pMaterial->pTextureFilename, 
			static_cast<int>(strlen(pMaterial->pTextureFilename)), 
			szTextureFileName,
			MAX_PATH);


		wchar_t	szFileName[MAX_PATH] = TEXT("");

		/* µ«ª¡Ó ∏  ∑ŒµÂ */
		wsprintf(szFileName, szTextureFileName, L"D");

		lstrcpy(szFullPath, filePath.c_str());
		lstrcat(szFullPath, szFileName);
		auto result = D3DXCreateTextureFromFile(_graphic_device.Get(), szFullPath, &pMeshMaterialTexture->diffuse_map);
		if (FAILED(result))
		{
			std::wcout << szFullPath << std::endl;
		}
		/* ≥Î∏÷ ∏  ∑ŒµÂ. */
		wsprintf(szFileName, szTextureFileName, L"N");

		lstrcpy(szFullPath, filePath.c_str());
		lstrcat(szFullPath, szFileName);
		result = D3DXCreateTextureFromFile(_graphic_device.Get(), szFullPath, &pMeshMaterialTexture->normal_map);


		/* Ω∫∆Â≈ß∑Ø ∏  ∑ŒµÂ. */
		wsprintf(szFileName, szTextureFileName, L"S");

		lstrcpy(szFullPath, filePath.c_str());
		lstrcat(szFullPath, szFileName);
		result = D3DXCreateTextureFromFile(_graphic_device.Get(), szFullPath, &pMeshMaterialTexture->specular_map);

		_materials.push_back(pMeshMaterialTexture);
	}

	return S_OK;
}

HRESULT MeshStatic::NativeConstruct(void* arg)
{
	return Mesh::NativeConstruct(arg);
}

std::shared_ptr<Component> MeshStatic::Clone(void* arg)
{
	auto pInstance = std::make_shared<MeshStatic>(*this);

	if (FAILED(pInstance->NativeConstruct(arg)))
	{
		MSGBOX("Failed to Clone MeshStatic");
		return nullptr;
	}
	return pInstance;
}

auto MeshStatic::SetUpTextureOnShader(const std::shared_ptr<Shader>& shader, const D3DXHANDLE parameter,
                                      const MeshMaterialTexture::kType type, const uint32_t materialIndex) -> HRESULT
{
	LPDIRECT3DTEXTURE9		pTexture = nullptr;
	if(_materials.empty())
	{
		return E_FAIL;
	}
	switch (type)
	{
	case MeshMaterialTexture::kType::kTypeDiffuse:
		pTexture = _materials[materialIndex]->diffuse_map.Get();
		break;
	case MeshMaterialTexture::kType::kTypeNormal:
		pTexture = _materials[materialIndex]->normal_map.Get();
		break;
	case MeshMaterialTexture::kType::kTypeSpecular:
		pTexture = _materials[materialIndex]->specular_map.Get();
		break;
	}

	if (FAILED(shader->SetUpTextureConstantTable(parameter, pTexture)))
		return E_FAIL;

	return S_OK;
}

auto MeshStatic::Render(const int32_t index) const -> HRESULT
{
	_mesh->DrawSubset(index);
	return S_OK;
}

auto MeshStatic::GetNumMaterials() const -> uint32_t
{
	return _num_materials;
}

auto MeshStatic::Create(const ComPtr<IDirect3DDevice9>& device, const std::wstring& filePath,
                        const std::wstring& fileName) -> std::shared_ptr<MeshStatic>
{
	auto pInstance = std::make_shared<MeshStatic>(device);

	if (FAILED(pInstance->NativeConstructPrototype(filePath, fileName)))
	{
		//MSGBOX("Failed to Creating MeshStatic");
		std::wcout << "Not Found File : " << fileName << std::endl;
		return nullptr;
	}
	return pInstance;
}
