#ifndef CBIRDCONTROLLER_H
#define  CBIRDCONTROLLER_H

#include <vector>
#include <sstream>
#include <string>
#include <windows.h>

#include "CBird.h"
#include "utils.h"
#include "Cga.h"
#include "CParams.h"
#include "SVector2D.h"

using namespace std;

class CBirdController
{
public:
	CBirdController(HWND hwndMain, int cxClient, int cyClient);
	~CBirdController();
	bool Update();
	vector<double> GetFitnessScores() const;
	void Render(HDC &surface);
private:
	
	//the entire population of chromosomes
	Cga* m_pPop;

	//array of birds
	vector<CBird> m_vecBirds;

	vector<CBird> m_vecBestBirds;

	int m_NumBirds;
	
	// vertex buffer for the sweeper shapes vertices
	vector<SPoint> m_BirdVB;

	// vertex buffer for objects
	vector<SPoint> m_ObjectsVB;

	//the acerage fitness per generation
	vector<double> m_vecAvFitness;

	//the best fitness per generation
	vector<double> m_vecBestFitness;

	// the best fitness forever
	double m_dBestFitness;

	//pens we use for the stats
	HPEN				m_RedPen;
	HPEN				m_BluePen;
	HPEN				m_GreenPen;
	HPEN        m_GreyPenDotted;
	HPEN        m_RedPenDotted;
	HPEN				m_OldPen;

	HBRUSH      m_RedBrush;
	HBRUSH      m_BlueBrush;

	//local copy of the handle to the application window
	HWND				m_hwndMain;

	//local copy of the  handle to the info window
	HWND        m_hwndInfo;

	//toggles the speed at which the simulation runs
	bool				m_bFastRender;

	//when set, renders the best performers from the 
	//previous generaion.
	bool        m_bRenderBest;

	//cycles per generation
	int					m_iTicks;

	//generation counter
	int					m_iGenerations;

	//local copy of the client window dimensions
	int         m_cxClient, m_cyClient;

	int m_iViewThisBird;

	void RenderBirds(HDC &surface, vector<CBird> &birds);

	//void RenderSensors(HDC &surface, vector<CBird> &birds);
	
	void WroldTransform(vector<SPoint> &bird, double scale);

};

#endif // CBIRDCONTROLLER_H
