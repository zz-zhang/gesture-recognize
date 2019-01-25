#include "gesturematch.h"

void TemplateInput(HandTemplate *palm, HandTemplate *fist, HandTemplate *left, HandTemplate *right, HandTemplate *up){
	ifstream fin(".\\template.txt");
	fin >> palm->typeStr >> palm->defectsNumMin >> palm->defectsNumMax >> palm->conCirRatioMin >> palm->conCirRatioMax >>
		palm->directionMin >> palm->directionMax >> palm->conRecRatioMin >> palm->conRecRatioMax >> palm->rectHWRatioMin >>
		palm->rectHWRatioMax;
	fin >> fist->typeStr >> fist->defectsNumMin >> fist->defectsNumMax >> fist->conCirRatioMin >> fist->conCirRatioMax >>
		fist->directionMin >> fist->directionMax >> fist->conRecRatioMin >> fist->conRecRatioMax >> fist->rectHWRatioMin >>
		fist->rectHWRatioMax;
	fin >> left->typeStr >> left->defectsNumMin >> left->defectsNumMax >> left->conCirRatioMin >> left->conCirRatioMax >>
		left->directionMin >> left->directionMax >> left->conRecRatioMin >> left->conRecRatioMax >> left->rectHWRatioMin >>
		left->rectHWRatioMax;
	fin >> right->typeStr >> right->defectsNumMin >> right->defectsNumMax >> right->conCirRatioMin >> right->conCirRatioMax >>
		right->directionMin >> right->directionMax >> right->conRecRatioMin >> right->conRecRatioMax >> right->rectHWRatioMin >>
		right->rectHWRatioMax;
	fin >> up->typeStr >> up->defectsNumMin >> up->defectsNumMax >> up->conCirRatioMin >> up->conCirRatioMax >>
		up->directionMin >> up->directionMax >> up->conRecRatioMin >> up->conRecRatioMax >> up->rectHWRatioMin >>
		up->rectHWRatioMax;

	palm->type = 1;
	fist->type = 2;
	left->type = 3;
	right->type = 4;
	up->type = 5;
	fin.close();
}

void TemplateMatch(HandTemplate temp, Hand *hand){
	if (hand->conCirRatio >= temp.conCirRatioMin && hand->conCirRatio <= temp.conCirRatioMax &&
		hand->defectsNum >= temp.defectsNumMin && hand->defectsNum <= temp.defectsNumMax &&
		hand->direction >= temp.directionMin && hand->direction <= temp.directionMax &&
		hand->conRecRatio >= temp.conRecRatioMin && hand->conRecRatio <= temp.conRecRatioMax &&
		hand->rectHWRatio >= temp.rectHWRatioMin && hand->rectHWRatio <= temp.rectHWRatioMax)
		hand->type = temp.type;
}
