#pragma once
class Light final : public std::enable_shared_from_this<Light>
{
public:
	explicit Light() = default;
	virtual ~Light() = default;
public:
	D3DLIGHT9 GetLightDesc() const;
public:
	HRESULT NativeConstruct(const D3DLIGHT9& lightDesc);
private:
	D3DLIGHT9			_light_desc;
public:
	static std::shared_ptr<Light> Create(const D3DLIGHT9& lightDesc);
};