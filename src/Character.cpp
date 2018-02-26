//
//  Character.cpp
//	Sandra Tieben
//	2018

//

#include "Character.h"

Character::Character():palette(NULL)
{
	
}

Character::~Character()
{

}

//Character::Character(const char* ModelFile, bool FitSize, float scale) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
//{
//	//Changed from load(ModelFile) to load(ModelFile, FitSize)
//	this->scale = scale;
//	bool ret = load(ModelFile, FitSize);
//	if(!ret)
//		throw std::exception();
//}


//Steuerung
void Character::steer( float ForwardBackward, float LeftRight)
{
	this->forwardBackward = ForwardBackward;
	this->leftRight = LeftRight;
}

//Steuerung mit Sprung
void Character::steer3d( float ForwardBackward, float LeftRight, float Jump)
{
	this->forwardBackward = ForwardBackward;
	this->leftRight = LeftRight;
	this->jump = Jump;
}

void Character::update(float dtime)
{
	Matrix characMat = this->transform();
	Matrix forwardMat, steeringMat, hoverMat;
	
	//Panzerbewegung berechnen
	float fbFactor = this->forwardBackward * dtime;
	float lrFactor = this->leftRight * dtime;
	float jpFactor = this->jump * dtime;
	float translatZ = this->posZ * dtime;
	
	
	if(palette != NULL) {
		//Höhe abhängig von der Palette
		int additionalJumpFactor = 5;
		jpFactor *= additionalJumpFactor;
		hoverMat = palette->getLocalTransform();
		steeringMat.rotationY(characMat.left().Z+lrFactor);
		forwardMat = forwardMat.translation(characMat.translation().X+fbFactor, hoverMat.translation().Y+jpFactor, characMat.translation().Z+translatZ);
		characMat = forwardMat * steeringMat;
	}
	else {
		forwardMat.translation(fbFactor, jpFactor, translatZ);
		steeringMat.rotationY(lrFactor);
		characMat = characMat * forwardMat * steeringMat;
	}
	
	//Aktuelle Position in Vektor speichern
	this->position.X = characMat.m[12];
	this->position.Y = std::max(characMat.m[13], TERRAIN_HEIGHT);
	this->position.Z = characMat.m[14];
	
	this->transform(characMat);
}

//void Character::draw(const BaseCamera& Cam)
//{
//	
//}

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

////(Kleinste) achsenausgerichtete Box berechnen, die das Modell komplett umschließt
//void Character::calcBoundingBox(AABB& Box)
//{
//
//}

void Character::setHovering(bool hover) {
	this->isHovering = hover;
}

bool Character::getHovering() const
{
	return this->isHovering;
}

void Character::setForwardBackward(float fb) {
	forwardBackward = fb;
}

void Character::setLeftRight(float lr) {
	leftRight = lr;
}

void Character::setJump(float jmp) {
	jump = jmp;
}

void Character::setPosZ(float z) {
	posZ = z;
}

//void Character::calcBoundingBox(AABB& box){
//	const AABB& modelBox = this->model->boundingBox();
//
//	//box.min.X = std::min(modelBox.min.X);
//	//box.min.Y = std::min(modelBox.min.Y);
//	//box.min.Z = std::min(modelBox.min.Z);
//
//	//box.max.X = std::max(modelBox.max.X);
//	//box.max.Y = std::max(modelBox.max.Y);
//	//box.max.Z = std::max(modelBox.max.Z);
//
//	// Verschiebung der BoundingBox
//	//Box.Min.X = std::min(this->position.X - chassisBox.size().X/2, this->position.X - cannonBox.size().X/2);
//	//Box.Min.Y = std::min(this->position.Y - chassisBox.size().Y/2, this->position.Y - cannonBox.size().Y/2);
//	//Box.Min.Z = std::min(this->position.Z - chassisBox.size().Z/2, this->position.Z - cannonBox.size().Z/2);
//
//	//Box.Min.X = std::max(this->position.X + chassisBox.size().X/2, this->position.X + cannonBox.size().X/2);
//	//Box.Min.Y = std::max(this->position.Y + chassisBox.size().Y/2, this->position.Y + cannonBox.size().Y/2);
//	//Box.Min.Z = std::max(this->position.Z + chassisBox.size().Z/2, this->position.Z + cannonBox.size().Z/2);
//}


