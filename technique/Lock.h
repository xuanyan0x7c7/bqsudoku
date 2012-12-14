#pragma once
#include "../Technique.h"


class Lock: public Technique {
public:
	Lock(const Lock&);
	Lock(Lock&&);
	Lock& operator =(const Lock&);
	Lock& operator =(Lock&&);
	~Lock();
public:
	Lock(Candidate&);
public:
	virtual HintType GetHint();
private:
	HintType LockedCandidates();
	HintType HiddenPair(size_t);
	HintType NakedPair(size_t);
};
