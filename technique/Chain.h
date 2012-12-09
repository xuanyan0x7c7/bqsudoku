#pragma once
#include "../Technique.h"


class Chain: public Technique {
public:
	Chain(const Chain&);
	Chain(Chain&&);
	Chain& operator =(const Chain&);
	Chain& operator =(Chain&&);
	~Chain();
public:
	Chain(Candidate&);
private:
	enum struct ChainType {
		XChain, NiceXLoop, XYChain, AIC, NiceLoop, GroupedXChain, GroupedNiceXLoop, GroupedAIC, GroupedNiceLoop
	};
	typedef std::vector<std::size_t> Group;
	inline std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(std::size_t, std::size_t, std::size_t, std::size_t, std::size_t, std::size_t) const;
	std::vector<std::size_t> CommonEffectCell(const Group&, const Group&) const;
	bool IsWeakChain(const Group&, const Group&) const;
	ChainType GetChainType(const std::vector<Group>&) const;
	std::string Chain2String(const std::vector<Group>&);
public:
	virtual HintType GetHint();
private:
	HintType Skyscraper();
	HintType _2StringKite();
	HintType TurbotFish();
	HintType ForcingChain();
};

inline std::vector<size_t> Chain::CommonEffectCell(size_t n1, size_t n2) const {
	return CommonEffectCell(n1 / (size * size), n1 / size % size, n1 % size + 1, n2 / (size * size), n2 / size % size, n2 % size + 1);
}
