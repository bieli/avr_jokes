#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "probabilisticnetwork.hpp"

class CNode
{
public:
	CNode();
	~CNode();

	int Load(uint Number, string Name, string State1, string State2, uint Cardinal, bool ElSet);


	char mStateNameArray[2][24];
	char mName[24];	
	
protected:

	bool* mEvidence;
	ulong mNumber;
	bool mEliminationSet;
	uint mCardinalNumber;
	
};

#endif /*_NODE_HPP_*/