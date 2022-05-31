//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "phongshader.h"
#include <list>

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{

}
Model::Model(const char* ModelFile, bool FitSize) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
	bool ret = load(ModelFile);
	if (!ret)
		throw std::exception();
}
Model::~Model()
{
	// TODO: Add your code (Exercise 3)
	deleteNodes(&RootNode);
}

void Model::deleteNodes(Node* pNode)
{
	if (!pNode)
		return;
	for (unsigned int i = 0; i < pNode->ChildCount; ++i)
		deleteNodes(&(pNode->Children[i]));
	if (pNode->ChildCount > 0)
		delete[] pNode->Children;
	if (pNode->MeshCount > 0)
		delete[] pNode->Meshes;
}

bool Model::load(const char* ModelFile, bool FitSize)
{
	const aiScene* pScene = aiImportFile(ModelFile, aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords);

	if (pScene == NULL || pScene->mNumMeshes <= 0)
		return false;

	Filepath = ModelFile;
	Path = Filepath;
	size_t pos = Filepath.rfind('/');
	if (pos == std::string::npos)
		pos = Filepath.rfind('\\');
	if (pos != std::string::npos)
		Path.resize(pos + 1);

	loadMeshes(pScene, FitSize);
	loadMaterials(pScene);
	loadNodes(pScene);

	return true;
}

void Model::loadMeshes(const aiScene* pScene, bool FitSize)
{	
	if (FitSize) {
		Matrix matrix;
		matrix.scale(1);
		transform(matrix);
	}
	calcBoundingBox(pScene, BoundingBox);
	MeshCount = pScene->mNumMeshes;
	pMeshes = new Mesh[MeshCount];
	for (size_t i = 0; i < MeshCount; i++) {
		Mesh& pMesh = pMeshes[i];
		aiMesh* aimesh = pScene->mMeshes[i];
		pMesh.MaterialIdx = aimesh->mMaterialIndex;
		pMesh.VB.begin();
		for (size_t v = 0; v < aimesh->mNumVertices; v++) {
			if (aimesh->HasNormals()) {
				aiVector3D& normal = aimesh->mNormals[v];
				pMesh.VB.addNormal(normal.x, normal.y, normal.z);
			}
			if (aimesh->HasTextureCoords(0)) {
				aiVector3D& texture = aimesh->mTextureCoords[0][v];
				pMesh.VB.addTexcoord0(texture.x, -texture.y);
			}
			aiVector3D& pos = aimesh->mVertices[v];
			pMesh.VB.addVertex(pos.x, pos.y, pos.z);
		}
		pMesh.VB.end();
		pMesh.IB.begin();
		for (size_t f = 0; f < aimesh->mNumFaces; f++) {
			aiFace& aiface = aimesh->mFaces[f];
			for (size_t j = 0; j < aiface.mNumIndices; j++) {
				pMesh.IB.addIndex(aiface.mIndices[j]);
			}
		}
		pMesh.IB.end();
	}
}
void Model::loadMaterials(const aiScene* pScene)
{
	MaterialCount = pScene->mNumMaterials;
	pMaterials = new Material[MaterialCount];
	for (size_t i = 0; i < MaterialCount; i++) {
		Material material;
		aiMaterial* aimaterials = pScene->mMaterials[i];
		aiColor3D color;
		aimaterials->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material.AmbColor = Color(color.r, color.g, color.b);
		aimaterials->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material.DiffColor = Color(color.r, color.g, color.b);
		aimaterials->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material.SpecColor = Color(color.r, color.g, color.b);
		float exp;
		aimaterials->Get(AI_MATKEY_SHININESS, exp);
		material.SpecExp = exp;

		aiString tmp;
		aimaterials->GetTexture(aiTextureType_DIFFUSE, 0, &tmp);
		string fullPath = Path + tmp.C_Str();
		material.DiffTex = Texture().LoadShared(fullPath.c_str());
		pMaterials[i] = material;
	}
}
void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{
	float minX = FLT_MIN;
	float minY = FLT_MIN;
	float minZ = FLT_MIN;
	float maxX = FLT_MAX;
	float maxY = FLT_MAX;
	float maxZ = FLT_MAX;
	for (size_t i = 0; i < pScene->mNumMeshes; i++) {
		for (size_t j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {
			aiVector3D& vertice = pScene->mMeshes[i]->mVertices[j];
			if (vertice.x < minX)
				minX = vertice.x;
			if (vertice.y < minY)
				minY = vertice.y;
			if (vertice.z < minZ)
				minZ = vertice.z;
			if (vertice.x > maxX)
				maxX = vertice.x;
			if (vertice.y > maxY)
				maxY = vertice.y;
			if (vertice.z > maxZ)
				maxZ = vertice.z;
		}
	}
	Box = { minX, minY, minZ, maxX, maxY, maxZ };
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

	if (paiNode->mNumMeshes > 0)
	{
		pNode->MeshCount = paiNode->mNumMeshes;
		pNode->Meshes = new int[pNode->MeshCount];
		for (unsigned int i = 0; i < pNode->MeshCount; ++i)
			pNode->Meshes[i] = (int)paiNode->mMeshes[i];
	}

	if (paiNode->mNumChildren <= 0)
		return;

	pNode->ChildCount = paiNode->mNumChildren;
	pNode->Children = new Node[pNode->ChildCount];
	for (unsigned int i = 0; i < paiNode->mNumChildren; ++i)
	{
		copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
		pNode->Children[i].Parent = pNode;
	}
}

void Model::applyMaterial(unsigned int index)
{
	if (index >= MaterialCount)
		return;

	PhongShader* pPhong = dynamic_cast<PhongShader*>(shader());
	if (!pPhong) {
		std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
		return;
	}

	Material* pMat = &pMaterials[index];
	pPhong->ambientColor(pMat->AmbColor);
	pPhong->diffuseColor(pMat->DiffColor);
	pPhong->specularExp(pMat->SpecExp);
	pPhong->specularColor(pMat->SpecColor);
	pPhong->diffuseTexture(pMat->DiffTex);
}

void Model::draw(const BaseCamera& Cam)
{
	if (!pShader) {
		std::cout << "BaseModel::draw() no shader found" << std::endl;
		return;
	}
	pShader->modelTransform(transform());

	std::list<Node*> DrawNodes;
	DrawNodes.push_back(&RootNode);

	while (!DrawNodes.empty())
	{
		Node* pNode = DrawNodes.front();
		Matrix GlobalTransform;

		if (pNode->Parent != NULL)
			pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
		else
			pNode->GlobalTrans = transform() * pNode->Trans;

		pShader->modelTransform(pNode->GlobalTrans);

		for (unsigned int i = 0; i < pNode->MeshCount; ++i)
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
		for (unsigned int i = 0; i < pNode->ChildCount; ++i)
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



