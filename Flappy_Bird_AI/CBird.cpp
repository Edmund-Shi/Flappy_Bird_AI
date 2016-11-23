#include "CBird.h"
#include <cmath>

//the time interval for update
const double g_dcTiemInterval = 0.01;

CBird::CBird() :
	m_bCollided(false),
	m_dLength(0),
	m_dFitness(0),
	m_dScale(CParams::iSweeperScale),
	m_dSpeed(0),
	m_dTap(0),
	m_dVelocity(0),
	m_dAccelerate(-10){
	// create a static start position 
	m_dHeight = 200;
	m_vPosition = SVector2D(180, m_dHeight);
	

}
void CBird::Reset() {
	m_dHeight = 200;
	m_vPosition = SVector2D(180, m_dHeight);

	m_dFitness = 0;

	m_dLength = 0;

}

void CBird::EndOfRunCalculations()
{
	m_dFitness += m_dLength;
}

bool CBird::Update(vector<SPoint> &objects) {
	vector<double> inputs;

	// get the positon of the path
	TestSensors(objects);

	//input sensor into net
	for (int sr = 0; sr < m_vecdSensors.size(); sr++) {
		inputs.push_back(m_vecdSensors[sr]);
		inputs.push_back(m_vecFeelers[sr]);
	}
	inputs.push_back(m_bCollided);

	vector<double> output = m_pItsBrain->Update(inputs, CNeuralNet::active);

	if (output.size()<CParams::iNumOutputs){
		return false;
	}

	//Control the bird
	m_dTap = output[0];
	
	if (m_dTap > m_dcTapThreshold){
		m_dVelocity = m_dcVecBurst;
	}

	//Updata the position and the velocity of bird
	m_dHeight += m_dVelocity * g_dcTiemInterval;
	m_dAccelerate += m_dAccelerate * g_dcTiemInterval;
	m_dLength += 1;
	m_vPosition = SVector2D(180, m_dHeight);
	
	return true;
}

void CBird::TestSensors(vector<SPoint> &objects) {
	m_bCollided = false;

	//flush the sensor
	m_vecdSensors.clear();
	m_vecFeelers.clear();

	for (int sr = 0; sr < m_Sensors.size();sr++) {
		bool bHit = false;
		double dist = 0;
		double res = 0;
		res = atan2(objects[sr].y - m_dHeight, objects[sr].x - 180); //assume the default x-postion is 180
		m_vecdSensors.push_back(res);
		dist = sqrt(pow((objects[sr].y - m_dHeight), 2) + pow(objects[sr].x - 180, 2));
		m_vecFeelers.push_back(dist);
	}
	return;
}

