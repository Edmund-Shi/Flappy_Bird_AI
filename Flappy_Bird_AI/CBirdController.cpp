#include "CBirdController.h"
#include <cmath>

const int NumBirdVerts = 4;
const SPoint bird[NumBirdVerts] = { SPoint(-1,1),SPoint(1,1),
									SPoint(1,-1),SPoint(-1,-1) };
const int NumObjectVerts = 8;
const SPoint objects[NumObjectVerts] = {SPoint(80, 60),
										SPoint(200,60),
										SPoint(200,60),
										SPoint(200,100),
										SPoint(200,100),
										SPoint(160,100),
										SPoint(160,100),
										SPoint(160,200) };


//-----------------Constructor---------------
CBirdController::CBirdController(HWND hwndMain,
	int cxClient,
	int cyClient) :m_NumBirds(CParams::iNumBirds),
	m_bFastRender(false), m_bRenderBest(false), m_iTicks(0),
	m_hwndMain(hwndMain), m_hwndInfo(NULL), m_iGenerations(0),
	m_cxClient(cxClient), m_cyClient(cyClient), m_iViewThisBird(0) {

	//Create Birds first 
	for (int i = 0; i < m_NumBirds; i++) {
		m_vecBirds.push_back(CBird());
	}

	//and the vector of Bird which will hold the best performing Bird
	for (int i = 0; i < CParams::iNumBestBirds; i++) {
		m_vecBestBirds.push_back(CBird());
	}
	m_pPop = new Cga(CParams::iNumBirds,
		CParams::iNumInputsforBirds,
		CParams::iNumOutputsforBirds,
		hwndMain,
		10, 10);
	vector<CNeuralNet*> pBrains = m_pPop->CreatePhenotypes();

	// assign the phenotypes 
	for (int i = 0; i < m_NumBirds; i++) {
		m_vecBirds[i].InsertNewBrain(pBrains[i]);
	}

	//create a pen for the graph drawing
	m_BluePen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_RedPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_GreenPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	m_GreyPenDotted = CreatePen(PS_DOT, 1, RGB(100, 100, 100));
	m_RedPenDotted = CreatePen(PS_DOT, 1, RGB(200, 0, 0));

	m_OldPen = NULL;

	//and the brushes
	m_BlueBrush = CreateSolidBrush(RGB(0, 0, 244));
	m_RedBrush = CreateSolidBrush(RGB(150, 0, 0));

	//fill the vertex buffers
	for (int i = 0; i < NumBirdVerts; i++) {
		m_BirdVB.push_back(bird[i]);
	}

	for (int i = 0; i < NumObjectVerts;i++) {
		m_ObjectsVB.push_back(objects[i]);
	}
}

// --------destructor------------
CBirdController::~CBirdController()
{
	if (m_pPop) {
		delete m_pPop;
	}

	DeleteObject(m_BluePen);
	DeleteObject(m_RedPen);
	DeleteObject(m_GreenPen);
	DeleteObject(m_OldPen);
	DeleteObject(m_GreyPenDotted);
	DeleteObject(m_RedPenDotted);
	DeleteObject(m_BlueBrush);
	DeleteObject(m_RedBrush);
}


// ------------update-------------
bool CBirdController::Update() {
	//first add up each Birds fitness scores. (remember for this task
	//there are many different sorts of penalties the Birds may incur
	//and each one has a coefficient)
	for (int swp = 0; swp < m_vecBirds.size(); ++swp)
	{
		m_vecBirds[swp].EndOfRunCalculations();
	}

	//increment the generation counter
	++m_iGenerations;

	//reset cycles
	m_iTicks = 0;

	//perform an epoch and grab the new brains
	vector<CNeuralNet*> pBrains = m_pPop->Epoch(GetFitnessScores());

	//insert the new  brains back into the Birds and reset their
	//positions
	for (int i = 0; i < m_NumBirds; ++i)
	{
		m_vecBirds[i].InsertNewBrain(pBrains[i]);

		m_vecBirds[i].Reset();
	}

	//grab the NNs of the best performers from the last generation
	vector<CNeuralNet*> pBestBrains = m_pPop->GetBestPhenotypesFromLastGeneration();

	//put them into our record of the best Birds
	for (int i = 0; i < m_vecBestBirds.size(); ++i)
	{
		m_vecBestBirds[i].InsertNewBrain(pBestBrains[i]);

		m_vecBestBirds[i].Reset();
	}

	//this will call WM_PAINT which will render our scene
	InvalidateRect(m_hwndInfo, NULL, TRUE);
	UpdateWindow(m_hwndInfo);

	return true;
}

vector<double> CBirdController::GetFitnessScores()const
{
	vector<double> scores;

	for (int i = 0; i < m_vecBirds.size(); ++i) {
		scores.push_back(m_vecBirds[i].Fitness());
	}
	return scores;
}

void CBirdController::Render(HDC &surface) {
	if (!m_bFastRender)
	{
		string s = "Generation: " + itos(m_iGenerations);
		TextOut(surface, 5, 0, s.c_str(), s.size());

		//select in the blue pen
		m_OldPen = (HPEN)SelectObject(surface, m_BluePen);

		if (m_bRenderBest)
		{
			//render the best Birds memory cells
			m_vecBestBirds[m_iViewThisBird].Render(surface);

			//render the best Birds from the last generation
			RenderBirds(surface, m_vecBestBirds);

			//render the best Birds sensors
			//RenderSensors(surface, m_vecBestBirds);
		}

		else
		{
			//render the Birds
			RenderBirds(surface, m_vecBirds);
		}

		SelectObject(surface, m_OldPen);

		//render the objects
		for (int i = 0; i < NumObjectVerts; i += 2)
		{
			MoveToEx(surface, m_ObjectsVB[i].x, m_ObjectsVB[i].y, NULL);

			LineTo(surface, m_ObjectsVB[i + 1].x, m_ObjectsVB[i + 1].y);
		}

	}//end if


	//advance function, update later
	//else
	//{
	//	PlotStats(surface);

	//	RECT sr;
	//	sr.top = m_cyClient - 50;
	//	sr.bottom = m_cyClient;
	//	sr.left = 0;
	//	sr.right = m_cxClient;

	//	//render the species chart
	//	m_pPop->RenderSpeciesInfo(surface, sr);

	//}
}

void CBirdController::RenderBirds(HDC &surface, vector<CBird> &birds) {
	for (int i = 0; i < birds.size();i++) {
		SelectObject(surface, m_RedPen);
		vector<SPoint> birdVB = m_BirdVB;
		
		//draw the birds
		MoveToEx(surface, (int)birdVB[0].x, (int)birdVB[0].y, NULL);
		for (int vert = 1; vert < 4;vert++) {
			LineTo(surface, (int)birdVB[vert].x, (int)birdVB[vert].y);
		}
		//next bird
	}
}

