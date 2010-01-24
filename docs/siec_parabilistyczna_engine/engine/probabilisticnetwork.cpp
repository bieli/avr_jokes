#include <stdlib.h>
#include "probabilisticnetwork.hpp"


CProbabilisticNetwork::CProbabilisticNetwork()
{
	mDirected = true;
	mMoral = -1;
	mTriangular = -1;
	mEliminationTree = false;
}

CProbabilisticNetwork::~CProbabilisticNetwork()
{
}


int
CProbabilisticNetwork::Load()
{
	mMode = 2;
	mDirected = 0;
	mMoral = 1;
	mTriangular = 1;
	mEliminationTree = 1;
	
	
	mNodeArray[0].Load(0, "Visit to Asia?", "NoVisit", "Visit", 8, 0);
	mNodeArray[1].Load(1, "Tuberculosis?", "Absent", "Present", 7, 0);
	mNodeArray[2].Load(2, "Smoking?", "NonSmoker", "Smoker", 6, 0);
	mNodeArray[3].Load(3, "Lung Cancer?", "Absent", "Present", 5, 0);
	mNodeArray[4].Load(4, "Bronchitis?", "Absent", "Present", 3, 0);
	mNodeArray[5].Load(5, "TuberculosisOrLungCancer?", "Nothing", "CancerOrTuberculosis", 2, 0);
	mNodeArray[6].Load(6, "X-RayResults", "Normal", "Abnormal", 4, 0);
	mNodeArray[7].Load(7, "Dyspnea?", "Absent", "Present", 1, 0);
	mNodeArray[8].Load(7, "Dys", "Normal", "Abnormal", 9, 0);
	
USART_Transmit_String("\n\r");
USART_Transmit_String("\n\r");
	int i;
	for ( i=0; i<9; i++)
	{
		USART_Transmit_String("\n\r");
		USART_Transmit_String(mNodeArray[i].mName);
		USART_Transmit_String(mNodeArray[i].mStateNameArray[0]);
		USART_Transmit_String(mNodeArray[i].mStateNameArray[1]);
	}

	return 1;

}

