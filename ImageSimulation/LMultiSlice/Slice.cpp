#include "stdafx.h"
#include "Slice.h"

Slice::Slice(void) {

}

Slice::Slice(size_t id, float lowerBound, float upperBound) {
	this->id = id;
	this->theUppestBound = this->upperBound = upperBound;
	this->theLowestBound = this->lowerBound = lowerBound;
}

Slice::~Slice(void) {

}

AModel::Cortege Slice::operator[](size_t i) {
	return *(composition[i]);
}

std::vector<AModel::Cortege*>::iterator Slice::begin() {
	return composition.begin();
}

std::vector<AModel::Cortege*>::iterator Slice::end() {
	return composition.end();
}

bool Slice::empty() {
	return composition.empty();
}

void Slice::push_back(AModel::Cortege *atom) {
	if(upperBound < atom->element.xsCoordinate.z)
		theUppestBound = atom->element.xsCoordinate.z;
	if(lowerBound > atom->element.xsCoordinate.z)
		theLowestBound = atom->element.xsCoordinate.z;
	composition.push_back(atom);
}

void Slice::pop_back() {
	composition.pop_back();
}

void Slice::clear() {
	composition.clear();
}

size_t Slice::size() {
	return composition.size();
}

size_t Slice::getId() {
	return id;
}

float Slice::getUpperBound() {
	return upperBound;
}

float Slice::getTheUppestBound() {
	return theUppestBound;
}

float Slice::getLowerBound() {
	return lowerBound;
}
float Slice::getTheLowestBound() {
	return theLowestBound;
}