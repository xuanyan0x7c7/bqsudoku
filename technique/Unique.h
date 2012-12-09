#pragma once
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
	HintType BivalueUniversalGrave();
};
