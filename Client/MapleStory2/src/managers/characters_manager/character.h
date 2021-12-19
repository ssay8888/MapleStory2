#pragma once
class Equipped;

class Character : public std::enable_shared_from_this<Character>
{
public:
	Character();
	~Character();
	auto NativeConstruct()->HRESULT;


	static auto Create()->std::shared_ptr<Character>;
public:
	auto SetInfo(Protocol::ViewCharacter info)->void;
	auto GetInfo()const->Protocol::ViewCharacter;

	auto GetEqpList() const ->std::shared_ptr<Equipped>;

private:
	std::shared_ptr<Equipped> _eqp_list;
	Protocol::ViewCharacter _info;
};

