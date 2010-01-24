#include "node.hpp"

CNode::CNode()
{
	mEvidence = NULL;
	mCardinalNumber = 0;
	mEliminationSet = false;
}

CNode::~CNode()
{
}


int 
CNode::Load(uint Number, string Name, string State1, string State2, uint Cardinal, bool ElSet)
{
	mNumber = Number;
	strcpy(mName, Name);

	strcpy(mStateNameArray[0], State1);
	strcpy(mStateNameArray[1], State2); 

	mCardinalNumber = Cardinal;
	mEliminationSet = ElSet;
	
	return 1;
}

