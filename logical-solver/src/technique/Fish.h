#pragma once
#include "../Technique.h"


class Fish: public Technique {
public:
	Fish(const Fish&);
	Fish(Fish&&);
	Fish& operator =(const Fish&);
	Fish& operator =(Fish&&);
	~Fish();
public:
	Fish(Candidate&);
public:
	virtual HintType GetHint();
private:
	HintType _Fish(std::size_t);
};
