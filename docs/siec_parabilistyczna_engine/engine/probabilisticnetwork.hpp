#ifndef _PROBABILISTICETWORK_HPP_
#define _PROBABILISTICETWORK_HPP_

#include "config.hpp"
#include "node.hpp"

class CProbabilisticNetwork
{
public:

	CProbabilisticNetwork();
	~CProbabilisticNetwork();

	int Load();

public: 
	CNode mNodeArray[9];

	ulong mNumber;
	string mName;
	uint mMode;
	bool mDirected;
	int mMoral;
	int mTriangular;
	bool mEliminationTree;
	static ulong mCounter;
};

#endif /*_PROBABILISTICNETWORK_HPP_*/