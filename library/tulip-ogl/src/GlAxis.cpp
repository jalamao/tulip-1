/*
 * GlAxis.cpp
 *
 *  Created on: 5 févr. 2009
 *      Author: antoine
 */

#include <sstream>
#include <cmath>
#include <list>
#include <map>

#include <tulip/GlLine.h>
#include <tulip/GlLabel.h>
#include <tulip/GlRect.h>

#include "tulip/GlAxis.h"



using namespace std;

namespace tlp {

GlAxis::GlAxis(const std::string &axisName, const Coord &axisBaseCoord, const float axisLength,
			   const AxisOrientation &axisOrientation, const Color &axisColor)  :
			   axisName(axisName), axisBaseCoord(axisBaseCoord), axisLength(axisLength),
			   axisOrientation(axisOrientation), axisColor(axisColor),
			   captionText(axisName), captionOffset(axisLength / 15.),
			   captionComposite(new GlComposite()), gradsComposite(new GlComposite()), captionSet(false), maxCaptionWidth(axisLength / 4.) {
	buildAxisLine();
	addGlEntity(captionComposite, "caption composite");
	addGlEntity(gradsComposite, "grads composite");
	axisGradsWidth = (DEFAULT_GRAD_WIDTH * axisLength) / 200.;
}

GlAxis::~GlAxis() {
	reset(true);
}

void GlAxis::setAxisGraduations(const std::vector<std::string> &axisGradsLabels, const LabelPosition &axisGradsPosition) {
	spaceBetweenAxisGrads = axisLength / (axisGradsLabels.size() - 1);
	gradsComposite->reset(true);
	ostringstream oss;
	unsigned int gradsCpt = 0;
	for (unsigned int i = 0 ; i < axisGradsLabels.size() ; ++i) {
		GlLine *axisGraduation = new GlLine();
		axisGraduation->setStencil(1);
		GlLabel *graduationLabel = NULL;
		float labelWidth = spaceBetweenAxisGrads;

		if (axisOrientation == HORIZONTAL_AXIS) {
			labelHeight = axisGradsWidth;
			axisGraduation->addPoint(Coord(axisBaseCoord.getX() + i * spaceBetweenAxisGrads, axisBaseCoord.getY() + axisGradsWidth / 2), axisColor);
			axisGraduation->addPoint(Coord(axisBaseCoord.getX() + i * spaceBetweenAxisGrads, axisBaseCoord.getY() - axisGradsWidth / 2), axisColor);
			if (axisGradsPosition == LEFT_OR_BELOW) {
				graduationLabel = new GlLabel(Coord(axisBaseCoord.getX() + i * spaceBetweenAxisGrads, axisBaseCoord.getY() - axisGradsWidth / 2 - labelHeight / 2),
											  Coord(labelWidth, labelHeight), axisColor);
			} else if (axisGradsPosition == RIGHT_OR_ABOVE) {
				graduationLabel = new GlLabel(Coord(axisBaseCoord.getX() + i * spaceBetweenAxisGrads, axisBaseCoord.getY() + axisGradsWidth / 2 + labelHeight / 2),
											  Coord(labelWidth, labelHeight), axisColor);
			}
		} else if (axisOrientation == VERTICAL_AXIS) {
			labelHeight = spaceBetweenAxisGrads * MAGIG_FACTOR;
			labelWidth = axisGradsLabels[i].length() * (labelHeight / 2.);

			if (labelWidth > (axisLength / 8.)) {
				labelWidth = (axisLength / 8.);
			}

			if (axisGradsLabels[i].length() == 1) {
				labelWidth *= 2.;
			}

			axisGraduation->addPoint(Coord(axisBaseCoord.getX() - axisGradsWidth / 2., axisBaseCoord.getY() + i * spaceBetweenAxisGrads), axisColor);
			axisGraduation->addPoint(Coord(axisBaseCoord.getX() + axisGradsWidth / 2., axisBaseCoord.getY() + i * spaceBetweenAxisGrads), axisColor);
			if (axisGradsPosition == LEFT_OR_BELOW) {
				graduationLabel = new GlLabel(Coord(axisBaseCoord.getX() - axisGradsWidth / 2. - labelWidth / 2. , axisBaseCoord.getY() + i * spaceBetweenAxisGrads),
											  Coord(labelWidth, labelHeight), axisColor);
			} else if (axisGradsPosition == RIGHT_OR_ABOVE) {
				graduationLabel = new GlLabel(Coord(axisBaseCoord.getX() + axisGradsWidth / 2. + labelWidth / 2. , axisBaseCoord.getY() + i * spaceBetweenAxisGrads),
											  Coord(labelWidth, labelHeight), axisColor);
			}
		}
		oss.str("");
		oss << axisName << " axis grad " << ++gradsCpt;
		gradsComposite->addGlEntity(axisGraduation, oss.str());
		oss.str("");
		oss << axisName << " axis grad label" << ++gradsCpt;
		graduationLabel->setText(axisGradsLabels[i]);
		graduationLabel->setStencil(1);
		gradsComposite->addGlEntity(graduationLabel, oss.str());
	}
}

void GlAxis::buildAxisLine() {
	GlLine *axisLine = new GlLine();
	axisLine->addPoint(axisBaseCoord, axisColor);
	if (axisOrientation == VERTICAL_AXIS) {
		axisLine->addPoint(Coord(axisBaseCoord.getX(), axisBaseCoord.getY() + axisLength), axisColor);
	} else if (axisOrientation == HORIZONTAL_AXIS) {
		axisLine->addPoint(Coord(axisBaseCoord.getX() + axisLength, axisBaseCoord.getY()), axisColor);
	}
	axisLine->setStencil(1);
	addGlEntity(axisLine, axisName + " axis");
}

Coord GlAxis::computeCaptionCenter() {
	Coord captionCenter;
	if (axisOrientation == VERTICAL_AXIS) {
		if (captionPosition == RIGHT_OR_ABOVE) {
			captionCenter = Coord(axisBaseCoord.getX(), axisBaseCoord.getY() + axisLength + captionOffset + captionHeight / 2);
		} else {
			captionCenter = Coord(axisBaseCoord.getX(), axisBaseCoord.getY() - captionOffset - captionHeight / 2);
		}
	} else if (axisOrientation == HORIZONTAL_AXIS) {
		if (captionPosition == RIGHT_OR_ABOVE) {
			captionCenter = Coord(axisBaseCoord.getX() + axisLength + captionOffset + (captionWidth / 2.), axisBaseCoord.getY());
		} else {
			captionCenter = Coord(axisBaseCoord.getX() - captionOffset - (captionWidth / 2.) , axisBaseCoord.getY());
		}
	}
	return captionCenter;
}

void GlAxis::addCaption(const LabelPosition &captionPos, const bool frame,
						const float sizeFactor, const float maxCapWidth, const std::string caption) {
	if (caption != "")
		captionText = caption;
	captionPosition = captionPos;
	captionFrame = frame;
	captionSizeFactor = sizeFactor;
	if (maxCapWidth != 0)
		maxCaptionWidth = maxCapWidth;
	computeCaptionSize(captionSizeFactor);
	captionOffset = axisLength / 15.;
	Coord captionCenter = computeCaptionCenter();
	addAxisCaption(captionCenter, captionFrame);
}

void GlAxis::addAxisCaption(const Coord &captionLabelCenter, const bool frame) {

	captionComposite->reset(true);
	captionSet = true;

	GlLabel *captionLabel = new GlLabel(captionLabelCenter, Coord(captionWidth, captionHeight), axisColor);
	captionLabel->setText(captionText);
	captionComposite->addGlEntity(captionLabel, axisName + " axis caption");

	if (frame) {

		GlRect *captionLabelInnerFrame = new GlRect(Coord(captionLabel->getBoundingBox().first.getX() - 1, captionLabel->getBoundingBox().second.getY() + 1),
							          			    Coord(captionLabel->getBoundingBox().second.getX() + 1, captionLabel->getBoundingBox().first.getY() - 1),
													axisColor, axisColor, false, true);
		for (unsigned int i = 0 ; i < 4 ; ++i) {
			captionLabelInnerFrame->ocolor(i) = axisColor;
		}
		captionComposite->addGlEntity(captionLabelInnerFrame, "caption inner frame" + captionText);

		GlRect *captionLabelOuterFrame = new GlRect(Coord(captionLabel->getBoundingBox().first.getX() - 2, captionLabel->getBoundingBox().second.getY() + 2),
												    Coord(captionLabel->getBoundingBox().second.getX() + 2, captionLabel->getBoundingBox().first.getY() - 2),
												    axisColor, axisColor, false, true);
		for (unsigned int i = 0 ; i < 4 ; ++i) {
			captionLabelOuterFrame->ocolor(i) = axisColor;
		}
		captionComposite->addGlEntity(captionLabelOuterFrame, "caption outer frame" + captionText);
	}
}

void GlAxis::computeCaptionSize(float sizeFactor) {
	captionHeight = sizeFactor * axisGradsWidth;
	captionWidth = sizeFactor * captionText.size() * (captionHeight / 2.);
	if (captionWidth > maxCaptionWidth) {
		captionWidth = maxCaptionWidth;
	}
}

void GlAxis::translate(const Coord &c) {
	axisBaseCoord += c;
	GlComposite::translate(c);
}

void GlAxis::updateAxis() {
	reset(true);
	gradsComposite = new GlComposite();
	addGlEntity(gradsComposite, "grads composite");
	buildAxisLine();
	if (captionSet) {
		captionComposite = new GlComposite();
		addGlEntity(captionComposite, "caption composite");
		addCaption(captionPosition, captionFrame, captionSizeFactor, maxCaptionWidth, captionText);
	}
}

}

