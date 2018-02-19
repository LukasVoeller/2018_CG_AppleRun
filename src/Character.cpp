//
//  Character.cpp
//	Lukas Voeller and Sandra Tieben
//	2018
//

#include "Character.h"

Character::Character(){
	
}

Character::~Character(){
	delete this->model;
}

bool Character::loadModels(const char* modelFile){
	this->model = new Model(modelFile, false);
	
	this->model->shader(this->pShader, false);
	
	/* unitBox() ist eine sehr schlechte Lösung für die BoundingBox
	 * Bessere wäre diese zu berechnen (aus Gehäuse und Kanone)
	 * Aber: Werte für Gehäuse und Kanone falsch (?), zumindest riesig
	 * daher erstmal die schlechte Lösung */
	BoundingBox = AABB::unitBox();
	//calcBoundingBox(this->BoundingBox);
	
	return true;
}

// Steuerung
void Character::steer(float forwardBackward, float leftRight){
	this->forwardBackward = forwardBackward;
	this->leftRight = leftRight;
}

// Steuerung mit Sprung
void Character::steer3d(float forwardBackward, float leftRight, float jump){
	this->forwardBackward = forwardBackward;
	this->leftRight = leftRight;
	this->jump = jump;
}

void Character::update(float dtime){
	Matrix characterMat = this->transform();
	Matrix forwardMat, steeringMat;
	
	// Characterbewegung berechnen
	float fbFactor = this->forwardBackward * dtime;
	float lrFactor = this->leftRight * dtime;
	float jpFactor = this->jump * dtime;
	
	//std::cout << fbFactor << " " << jpFactor <<std::endl;
	forwardMat.translation(fbFactor, jpFactor, 0);
	
	steeringMat.rotationY(lrFactor);
	characterMat = characterMat * forwardMat * steeringMat;
	
	// Aktuelle Position in Vektor speichern
	this->position.X = characterMat.m[12];
	this->position.Y = characterMat.m[13];
	this->position.Z = characterMat.m[14];
	
	// Alles zusammen bauen
	this->model->transform(characterMat);
	this->transform(characterMat);
}

void Character::draw(const BaseCamera& cam){
	this->model->draw(cam);
}

Vector Character::getLatestPosition() {
	return this->position;
}

void Character::printLatestPosition() {
	std::cout << "X:" << this->position.X << " ";
	std::cout << "Y:" << this->position.Y << " ";
	std::cout << "Z:" << this->position.Z << std::endl;
}

bool Character::getIsInAir(){
	return this->isInAir;
}

void Character::setIsInAir(bool newIsInAir){
	this->isInAir = newIsInAir;
}

float Character::getJumpPower(){
	return this->jumpPower;
}

// (Kleinste) achsenausgerichtete Box berechnen, die das Modell komplett umschließt
void Character::calcBoundingBox(AABB& box){
	const AABB& modelBox = this->model->boundingBox();
	
	//box.min.X = std::min(modelBox.min.X);
	//box.min.Y = std::min(modelBox.min.Y);
	//box.min.Z = std::min(modelBox.min.Z);
	
	//box.max.X = std::max(modelBox.max.X);
	//box.max.Y = std::max(modelBox.max.Y);
	//box.max.Z = std::max(modelBox.max.Z);
	
	// Verschiebung der BoundingBox
	//Box.Min.X = std::min(this->position.X - chassisBox.size().X/2, this->position.X - cannonBox.size().X/2);
	//Box.Min.Y = std::min(this->position.Y - chassisBox.size().Y/2, this->position.Y - cannonBox.size().Y/2);
	//Box.Min.Z = std::min(this->position.Z - chassisBox.size().Z/2, this->position.Z - cannonBox.size().Z/2);
	
	//Box.Min.X = std::max(this->position.X + chassisBox.size().X/2, this->position.X + cannonBox.size().X/2);
	//Box.Min.Y = std::max(this->position.Y + chassisBox.size().Y/2, this->position.Y + cannonBox.size().Y/2);
	//Box.Min.Z = std::max(this->position.Z + chassisBox.size().Z/2, this->position.Z + cannonBox.size().Z/2);
}

