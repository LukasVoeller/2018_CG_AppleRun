#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include "BaseModel.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Aabb.h"
#include <string>
#include <list>

class Model : public BaseModel
{
public:
    Model();
	//Model(const char* ModelFile, bool FitSize=true);
	Model(const char* ModelFile, bool FitSize=false, float scale=1.0);
    virtual ~Model();

    bool load(const char* ModelFile, bool FitSize=true);
    virtual void draw(const BaseCamera& Cam);
    const AABB& getBoundingBox() const { return BoundingBox; }
	void setBoundingBox(const AABB& box) {BoundingBox = box; }

	const AABB& getScaledBoundingBox() const { return scaledBoundingBox; }
	const AABB& getMovedScaledBoundingBox() const { return movedScaledBoundingBox; }
	
	const AABB& scaleBoundingBox(Vector scale);
	const AABB& moveScaledBoundingBox();

protected:
    struct Mesh
    {
        Mesh()  {}
        VertexBuffer VB;
        IndexBuffer IB;
        int MaterialIdx;
    };

    struct Material
    {
        Material() : DiffTex(NULL), NormalMap(NULL), DiffColor(1,1,1),SpecColor(0.3f,0.3f,0.3f), AmbColor(0,0,0), SpecExp(10) {}
        Color DiffColor;
        Color SpecColor;
        Color AmbColor;
        float SpecExp;
        const Texture* DiffTex;
        const Texture* NormalMap;
    };

    struct Node
    {
        Node() : Parent(NULL), Children(NULL), ChildCount(0), MeshCount(0), Meshes(NULL) {}
        Matrix Trans;
        Matrix GlobalTrans;
        int* Meshes;
        unsigned int MeshCount;
        Node* Parent;
        Node* Children;
        unsigned int ChildCount;
        std::string Name;
    };

protected:
    void loadMeshes(const aiScene* pScene, bool FitSize);
    void loadMaterials(const aiScene* pScene);
    void calcBoundingBox( const aiScene* pScene, AABB& Box);
    void loadNodes(const aiScene* pScene);
    void copyNodesRecursive(const aiNode* paiNode, Node* pNode);
    void applyMaterial( unsigned int index);
    void deleteNodes(Node* pNode);
	Matrix convert(const aiMatrix4x4& m);

protected:
    Mesh* pMeshes;
    unsigned int MeshCount;
    Material* pMaterials;
    unsigned int MaterialCount;
    AABB BoundingBox;
	AABB scaledBoundingBox;
	AABB movedScaledBoundingBox;

    std::string Filepath;
	std::string Path;
    Node RootNode;
	float scale;
};

#endif /* Model_hpp */
