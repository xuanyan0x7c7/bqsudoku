#pragma once
#include <array>
#include <vector>
#include "../Technique.h"


class Unique: public Technique {
public:
	Unique(const Unique&);
	Unique(Unique&&);
	Unique& operator =(const Unique&);
	Unique& operator =(Unique&&);
	~Unique();
public:
	Unique(Candidate&);
public:
	virtual HintType GetHint();
private:
	HintType UniqueLoop();
	HintType AvoidableRectangle(); // I don't know the algorithm of AvoidableLoop
	HintType BivalueUniversalGrave();
private:
	struct Loop {
		std::vector<std::size_t> cell;
		std::vector<bool> set;
		std::array<size_t, 2> var;
		int type;
		friend bool operator <(const Loop &l1, const Loop &l2) {
			return l1.cell.size() < l2.cell.size() || (l1.cell.size() == l2.cell.size() && l1.type < l2.type);
		}
	};
	int GetType(const Loop&) const;
private:
	const std::vector<std::size_t>& RegionIndex(size_t) const;
	const std::vector<std::size_t>& RegionCount(size_t) const;
	void FindLoop(std::vector<Loop>&, Loop&, std::size_t, const std::vector<bool>&, int) const;
};
