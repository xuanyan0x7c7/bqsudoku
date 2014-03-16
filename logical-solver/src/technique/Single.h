#pragma once
#include "../Technique.h"


class Single: public Technique {
public:
	Single(const Single&);
	Single(Single&&);
	Single& operator =(const Single&);
	Single& operator =(Single&&);
	~Single();
public:
	Single(Candidate&);
public:
	virtual HintType GetHint();
private:
	HintType _Single();
	HintType HiddenSingle();
	HintType NakedSingle();
};
