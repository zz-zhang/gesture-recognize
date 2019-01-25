#include "gesturestudy.h"

vector<int> _defectsNum;
vector<float> _conCirRatio;
vector<int> _direction;
vector<float> _conRectRatio;
vector<float> _rectHWRatio;

int GestureStudyInit(const string filename){
	int defectsNumMin, defectsNumMax;
	float conCirRatioMin, conCirRatioMax;
	int directionMin, directionMax;
	float conRectRatioMin, conRectRatioMax;
	float rectHWRatioMin, rectHWRatioMax;


	string file = ".\\frame\\" + filename + "\\" + filename + ".txt";
	ifstream fin(file);
	fin >> defectsNumMin >> defectsNumMax >> conCirRatioMin >> 
		conCirRatioMax >> directionMin >> directionMax >> conRectRatioMin >>
		conRectRatioMax >> rectHWRatioMin >> rectHWRatioMax;
	fin.close();

	_defectsNum.push_back(defectsNumMin);
	_defectsNum.push_back(defectsNumMax);
	_conCirRatio.push_back(conCirRatioMin);
	_conCirRatio.push_back(conCirRatioMax);
	_direction.push_back(directionMin);
	_direction.push_back(directionMax);
	_conRectRatio.push_back(conRectRatioMin);
	_conRectRatio.push_back(conRectRatioMax);
	_rectHWRatio.push_back(rectHWRatioMin);
	_rectHWRatio.push_back(rectHWRatioMax);


	//if (hand->defectsNum < defectsNumMin)
	//	defectsNumMin = hand->defectsNum;
	//if (hand->defectsNum > defectsNumMax)
	//	defectsNumMax = hand->defectsNum;
	//if (hand->conCirRatio < conCirRatioMin)
	//	conCirRatioMin = hand->conCirRatio;
	//if (hand->conCirRatio > conCirRatioMax)
	//	conCirRatioMax = hand->conCirRatio;
	//if (hand->direction < directionMin)
	//	directionMin = hand->direction;
	//if (hand->direction > directionMax)
	//	directionMax = hand->direction;
	
	return 0;
}
void GestureStudy(Hand *hand){

	_defectsNum.push_back(hand->defectsNum);
	_conCirRatio.push_back(hand->conCirRatio);
	_direction.push_back(hand->direction);
	_conRectRatio.push_back(hand->conRecRatio);
	_rectHWRatio.push_back(hand->rectHWRatio);
	return;
}

void GestureStudyOutput(const string filename){
	string file = ".\\frame\\" + filename + "\\" + filename + ".txt";
	ofstream fout(file);

	sort(_defectsNum.begin(), _defectsNum.end());
	sort(_conCirRatio.begin(), _conCirRatio.end());
	sort(_direction.begin(), _direction.end());
	sort(_conRectRatio.begin(), _conRectRatio.end());
	sort(_rectHWRatio.begin(), _rectHWRatio.end());

	int i, j;
	float ratio = 0.99;
	i = _defectsNum.size() * (1 - ratio);
	j = _defectsNum.size() * ratio;

	fout << _defectsNum[i] << " " << _defectsNum[j] << " " << _conCirRatio[i] << " " <<
		_conCirRatio[j] << " " << _direction[i] << " " << _direction[j] << " " << 
		_conRectRatio[i] << " " << _conRectRatio[j] << " " << _rectHWRatio[i] << " " << 
		_rectHWRatio[j];

 	/*for (vector<int>::iterator ii = _direction.begin(); ii != _direction.end(); ++ii)
		cout << *ii << " ";*/
	//waitKey(0);
	fout.close();
}