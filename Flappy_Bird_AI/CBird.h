#ifndef CBIRD_H
#define CBIRD_H

#include <vector>
#include <math.h>
#include "phenotype.h"
#include "utils.h"
#include "CParams.h"
#include "SVector2D.h"



using namespace std;
class CBird
{
public:
	CBird();
	~CBird() {}

	//updates the ANN with information from the sweepers enviroment
	bool Update(vector<SPoint> &objects);

	//used to transform the sweepers vertices prior to rendering
	void WorldTransform(vector<SPoint> &sweeper, double scale);
		 
	void Reset();
		 
	void EndOfRunCalculations();

	void RenderStats(HDC surface);

	void Render(HDC surface);

	void DrawNet(HDC &surface, int cxLeft, int cxRight, int cyTop, int cyBot)
	{
		m_pItsBrain->DrawNet(surface, cxLeft, cxRight, cyTop, cyBot);
	}


	//-------------------accessor functions
	SVector2D	Position()const { return m_vPosition; }

	//double            Rotation()const { return m_dRotation; }

	float Fitness()const { return m_dFitness; }

	double Scale()const { return m_dScale; }

	double Length()const { return m_dLength; }

	//vector<SPoint>&   Sensors() { return m_tranSensors; }

	//vector<double>&   SensorReadings() { return m_vecdSensors; }

	bool Collided()const { return m_bCollided; }

	//vector<double>    MemoryReadings() { return m_vecFeelers; }

	//int               NumCellsVisited() { return m_MemoryMap.NumCellsVisited(); }

	void InsertNewBrain(CNeuralNet* brain) { m_pItsBrain = brain; }

	// get the sensor result
	void TestSensors(vector<SPoint> &objects);

private:
	CNeuralNet *m_pItsBrain;
	SVector2D m_vPosition; // the position of the bird
	double m_dSpeed;	//the speed of the bird
	
	//to store the output from ANN
	double m_dTap;

	//the bird fitness score
	double m_dFitness;
	
	//the scale of bird when draw 
	double m_dScale;

	//the total length the bird goes 
	double m_dLength;
	
	// if a collision has been detected this flag is set 
	bool m_bCollided;

	//vertical Velocity
	double m_dVelocity;

	//vertical accelerate default as -10
	double m_dAccelerate;

	//current height of the bird used to convert to m_vPosition
	double m_dHeight;
	
	vector<double> m_vecdSensors;
	vector<double> m_vecFeelers;

	//the threshold for dTap
	const double m_dcTapThreshold = 0.8;
	
	//the veclocity acquired for tap
	const double m_dcVecBurst = 20;

	//TODO initiallize the following vars in construction
	vector<double> m_Sensors;

	int CheckForHit(vector<SPoint> &sensor,
		int				NumSensors,
		double			range);

};

#endif