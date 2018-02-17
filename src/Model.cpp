//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "PhongShader.h"
#include <list>
#include <float.h>
#include <algorithm>    // std::max
#include <sstream>
#include <string>
#include <fstream>

#include "Model.h"
#include "PhongShader.h"
#include <list>
#include <limits>
#include <sstream>
#include <float.h>

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0), scale(1.0)
{
	
}

Model::Model(const char* ModelFile, bool FitSize, float scale) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
	//Changed from load(ModelFile) to load(ModelFile, FitSize)
	this->scale = scale;
	bool ret = load(ModelFile, FitSize);
	if(!ret)
		throw std::exception();
}

Model::~Model()
{
    deleteNodes(&RootNode);
	
    delete [] this->pMeshes;
    delete [] this->pMaterials;
}

void Model::deleteNodes(Node* pNode)
{
    if(!pNode)
        return;
    for(unsigned int i=0; i<pNode->ChildCount; ++i)
        deleteNodes(&(pNode->Children[i]));
    if(pNode->ChildCount>0)
        delete [] pNode->Children;
    if(pNode->MeshCount>0)
        delete [] pNode->Meshes;
}

bool Model::load(const char* ModelFile, bool FitSize)
{
    const aiScene* pScene = aiImportFile( ModelFile,aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords | aiProcess_FlipUVs );

    if(pScene==NULL || pScene->mNumMeshes<=0)
        return false;

    Filepath = ModelFile;
    Path = Filepath;
    size_t pos = Filepath.rfind('/');
    if(pos == std::string::npos)
        pos = Filepath.rfind('\\');
    if(pos !=std::string::npos)
        Path.resize(pos+1);

    loadMeshes(pScene, FitSize);
    loadMaterials(pScene);
    loadNodes(pScene);

    return true;
}

void Model::loadMeshes(const aiScene* pScene, bool FitSize)
{
	//Meshes	(Polygonale	Modelle)	aus	aiScene::mMeshes in Model::pMeshes (und	MeshCount) laden.
	//Hierfür	müssen	passende Vertex- und Indexbuffer erzeugt werden	(Position,	Normale	&	UV-Texturkoordinaten soweit	vorhanden).

    this->MeshCount = pScene->mNumMeshes;
    this->pMeshes = new Mesh[pScene->mNumMeshes];

	// Bounding Box
	float factor = 1.0;

    if (FitSize) {
        //Wenn	FitSize auf	true gesetzt wurde,	soll das Mesh sinnvolle Skalierung auf z.b 5x5 Einheiten.
        Vector size = BoundingBox.size();
        float maxLength = fmax(size.X, fmax(size.Y, size.Z));
        factor = 4/maxLength;
    }
	
	factor = factor * scale;
	this->BoundingBox = AABB(Vector(FLT_MAX, FLT_MAX, FLT_MAX), Vector(FLT_MIN, FLT_MIN, FLT_MIN));
	calcBoundingBox(pScene, this->BoundingBox);
	
    //Mesh: Geometrie/Modell mit einem einzigem Material
    //Gesamtmodell bestehend aus vielen Meshes
    for (int i=0; i< MeshCount; ++i) {
        //Initialisieren
        aiMesh* mesh = pScene->mMeshes[i];

        Mesh& currentMesh = this->pMeshes[i];
        currentMesh.MaterialIdx = mesh->mMaterialIndex;
        currentMesh.VB.begin();

		//Jedes Mesh enthält meherere Vertices//
		for (int j=0; j< mesh->mNumVertices; ++j) {
			aiVector3D normal = mesh->mNormals[j];
			aiVector3D vertex = mesh->mVertices[j];

			if (mesh->HasNormals()) {
				currentMesh.VB.addNormal(normal.x, normal.y, normal.z);
			}

			if (mesh->HasTextureCoords(0)) {
				aiVector3D* textCordsChannels = mesh->mTextureCoords[0];

				aiVector3D textCords = textCordsChannels[j];

				currentMesh.VB.addTexcoord0(textCords.x, textCords.y, textCords.z);
			}
			
			//Added Tangents and Bitangents
			if(mesh->HasTangentsAndBitangents()) {
				aiVector3D tang = mesh->mTangents[j];
				aiVector3D bitang = mesh->mBitangents[j];

				currentMesh.VB.addTexcoord1(tang.x, tang.y, tang.z);
				currentMesh.VB.addTexcoord2(bitang.x, bitang.y, bitang.z);
			}

			//FitSize ggf. anpassen (falls FitSize false: factor = 1)
			vertex *= factor;

			currentMesh.VB.addVertex(vertex.x, vertex.y, vertex.z);
        }

        currentMesh.VB.end();
        currentMesh.IB.begin();

        //Index Buffer füllen
        for (int j = 0; j < mesh->mNumFaces; ++j) {
            aiFace currentFace = mesh->mFaces[j];

            for (int k = 0; k < currentFace.mNumIndices; ++k) {
                currentMesh.IB.addIndex(currentFace.mIndices[k]);
            }
        }

        currentMesh.IB.end();
    }
	
	
}

//Lädt die	Materialeinstellungen von Assimp in	Model::pMaterials (und MaterialCount).
//Hierfür müssen die passenden Material-Keys von Assimp abgefragt werden (aiGetMaterialColor & aiGetMaterialString für Texturnamen).
//Zum Laden von	Texturen können	Sie auf	die	Member-Variable	Path zugreifen, die	den	Pfad zur Modell-Datei enthält. Texturen können Sie mit
//Hilfe	der	Funktion Texture::loadShared(...) laden.
void Model::loadMaterials(const aiScene* pScene)
{
    if(pScene->HasMaterials()) {
        this->MaterialCount = pScene->mNumMaterials;
        this->pMaterials = new Material[MaterialCount];

        for (int i=0; i<MaterialCount; ++i) {

            //Daten aus pScene laden
            aiMaterial* currentMat = pScene->mMaterials[i];

            aiString matname;
            currentMat->Get(AI_MATKEY_NAME, matname);

            aiColor3D dif(0.f,0.f,0.f);
            aiColor3D ambient(0.f,0.f,0.f);
            aiColor3D spec(0.f,0.f,0.f);
            float specExp = 0.0;

            currentMat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
            currentMat->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
            currentMat->Get(AI_MATKEY_COLOR_SPECULAR, spec);
            currentMat->Get(AI_MATKEY_SHININESS, specExp);

            //Geladene Daten in pMaterials speichern
            Material& mat = pMaterials[i];

            mat.DiffColor = Color(dif.r, dif.g, dif.b);
            mat.SpecColor = Color(spec.r, spec.g, spec.b);
            mat.SpecExp = specExp;
            mat.AmbColor = Color(ambient.r, ambient.g, ambient.b);

            //Texturen laden
            //std::string texturePath = this->Path;
            aiString texturePath;
            aiReturn textureFound = currentMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
            unsigned int numTextures= currentMat->GetTextureCount(aiTextureType_DIFFUSE);

            if(numTextures > 0 && textureFound == AI_SUCCESS) {
                aiString fullPath;
                //Pfad der Normal Map
                //Texturpfad kopieren
                std::string normalMapPath = texturePath.C_Str();
                //Index des letzten Punktes finden
                std::size_t dotIndex = normalMapPath.find_last_of(".");
                //_n vor dem letzten Einfuegen
                normalMapPath.insert(dotIndex, "_n");

                //Diffuse-Textur
                //Pfad erstellen
                fullPath.Set(this->Path);
                fullPath.Append(texturePath.C_Str());
                //Laden der Diffuse-Textur
                pMaterials[i].DiffTex = pMaterials[i].DiffTex->LoadShared(fullPath.C_Str());

                //Normal-Map
                //Pfad erstellen
                fullPath.Set(this->Path);
                fullPath.Append(normalMapPath.c_str());
                //Laden der Normal-Map
                //Pruefen, ob Datei existiert
                std::ifstream file(fullPath.C_Str());
                if (file.good()) {
                    // Falls Datei existiert, versuchen, diese zu laden
                    std::cout << "Loading NormalMap: " << normalMapPath << std::endl;
                    pMaterials[i].NormalMap = pMaterials[i].NormalMap->LoadShared(fullPath.C_Str());
                }
                file.close();
            }
        }
    }
}

//(Kleinste) achsenausgerichtete Box berechnen, die das Modell komplett umschließt
void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{
    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        aiMesh* mesh = pScene->mMeshes[i];

        for (int j = 0; j < mesh->mNumVertices; ++j) {
            aiVector3D vertex = mesh->mVertices[j];

			Box.Min.X = std::min(Box.Min.X, vertex.x*scale);
			Box.Min.Y = std::min(Box.Min.Y, vertex.y*scale);
			Box.Min.Z = std::min(Box.Min.Z, vertex.z*scale);

            Box.Max.X = std::max(Box.Max.X, vertex.x*scale);
            Box.Max.Y = std::max(Box.Max.Y, vertex.y*scale);
            Box.Max.Z = std::max(Box.Max.Z, vertex.z*scale);
        }
    }
}

void Model::loadNodes(const aiScene* pScene)
{
    deleteNodes(&RootNode);
    copyNodesRecursive(pScene->mRootNode, &RootNode);
}

void Model::copyNodesRecursive(const aiNode* paiNode, Node* pNode)
{
    pNode->Name = paiNode->mName.C_Str();
    pNode->Trans = convert(paiNode->mTransformation);

    if(paiNode->mNumMeshes > 0)
    {
        pNode->MeshCount = paiNode->mNumMeshes;
        pNode->Meshes = new int[pNode->MeshCount];
        for(unsigned int i=0; i<pNode->MeshCount; ++i)
            pNode->Meshes[i] = (int)paiNode->mMeshes[i];
    }

    if(paiNode->mNumChildren <=0)
        return;

    pNode->ChildCount = paiNode->mNumChildren;
    pNode->Children = new Node[pNode->ChildCount];
    for(unsigned int i=0; i<paiNode->mNumChildren; ++i)
    {
        copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
        pNode->Children[i].Parent = pNode;
    }
}

void Model::applyMaterial( unsigned int index)
{
	if(index>=MaterialCount)
		return;
	
	PhongShader* pPhong = dynamic_cast<PhongShader*>(shader());
	if(!pPhong) {
		std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
		return;
	}
	
	Material* pMat = &pMaterials[index];
	pPhong->ambientColor(pMat->AmbColor);
	pPhong->diffuseColor(pMat->DiffColor);
	pPhong->specularExp(pMat->SpecExp);
	pPhong->specularColor(pMat->SpecColor);
	pPhong->diffuseTexture(pMat->DiffTex);
	pPhong->normalTexture(pMat->NormalMap);
}

void Model::draw(const BaseCamera& Cam)
{
    if(!pShader) {
        std::cout << "BaseModel::draw() no shader found" << std::endl;
        return;
    }
    pShader->modelTransform(transform());

    std::list<Node*> DrawNodes;
    DrawNodes.push_back(&RootNode);

    while(!DrawNodes.empty())
    {
        Node* pNode = DrawNodes.front();
        Matrix GlobalTransform;

        if(pNode->Parent != NULL)
            pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
        else
            pNode->GlobalTrans = transform() * pNode->Trans;

        pShader->modelTransform(pNode->GlobalTrans);

        for(unsigned int i = 0; i<pNode->MeshCount; ++i )
        {
            Mesh& mesh = pMeshes[pNode->Meshes[i]];
            mesh.VB.activate();
            mesh.IB.activate();
            applyMaterial(mesh.MaterialIdx);
            pShader->activate(Cam);
            glDrawElements(GL_TRIANGLES, mesh.IB.indexCount(), mesh.IB.indexFormat(), 0);
            mesh.IB.deactivate();
            mesh.VB.deactivate();
        }
        for(unsigned int i = 0; i<pNode->ChildCount; ++i )
            DrawNodes.push_back(&(pNode->Children[i]));

        DrawNodes.pop_front();
    }
}

Matrix Model::convert(const aiMatrix4x4& m)
{
    return Matrix(m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
}

