//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "phongshader.h"
#include "float.h"
#include <list>

using namespace std;

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{

}
Model::Model(const char* ModelFile, bool FitSize) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
	bool ret = load(ModelFile, FitSize);
	if (!ret)
		throw std::exception();
}
Model::~Model()
{
	delete this->pMeshes;
	delete this->pMaterials;
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
	///** Bounding Box */
	this->BoundingBox = AABB();
	this->calcBoundingBox(pScene, this->BoundingBox);

	///** Fit Size */
	float scale = 1.0f;
	if (FitSize) {
		// norm "dimension where length closest to 5" == "5"
		float legthToFitTo = 5;
		scale = this->fitSize(legthToFitTo);
		this->BoundingBox.Max = this->BoundingBox.Max * scale;
		this->BoundingBox.Min = this->BoundingBox.Min * scale;
	}

	/*** Loading Meshes */
	this->pMeshes = new Mesh[pScene->mNumMeshes]();
	for (int i = 0; i < pScene->mNumMeshes; i++) {
		this->pMeshes[i].VB.begin();
		this->pMeshes[i].IB.begin();
		for (int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {
			/** Material */
			//this->pMeshes[i].MaterialIdx = pScene->mMeshes[i]->mMaterialIndex;

			/** Normal */
			Vector normal(
				pScene->mMeshes[i]->mNormals[j].x,
				pScene->mMeshes[i]->mNormals[j].y,
				pScene->mMeshes[i]->mNormals[j].z
			);
			this->pMeshes[i].VB.addNormal(normal);

			/** Texture */
			if (pScene->mMeshes[i]->mTextureCoords[0]) {
				this->pMeshes[i].VB.addTexcoord0(
					pScene->mMeshes[i]->mTextureCoords[0][j].x,
					-pScene->mMeshes[i]->mTextureCoords[0][j].y
				);
			}
			else {
				this->pMeshes[i].VB.addTexcoord0(0, 0);
			}

			/** Position */
			Vector vertex(
				pScene->mMeshes[i]->mVertices[j].x * scale,
				pScene->mMeshes[i]->mVertices[j].y * scale,
				pScene->mMeshes[i]->mVertices[j].z * scale
			);
			this->pMeshes[i].VB.addVertex(vertex);
		}
		for (int j = 0; j < pScene->mMeshes[i]->mNumFaces; j++) {
			for (int k = 0; k < pScene->mMeshes[i]->mFaces[j].mNumIndices; k++) {
				this->pMeshes[i].IB.addIndex(pScene->mMeshes[i]->mFaces[j].mIndices[k]);
			}
		}
		this->pMeshes[i].VB.end();
		this->pMeshes[i].IB.end();
	}
}

void Model::loadMaterials(const aiScene* pScene)
{
	this->MaterialCount = pScene->mNumMaterials;
	this->pMaterials = new Material[pScene->mNumMaterials]();
	for (int i = 0; i < pScene->mNumMeshes; i++) {
		if (pScene->mMeshes[i]->mMaterialIndex >= 0) { // Falls Material existiert
			int mIndex = pScene->mMeshes[i]->mMaterialIndex;
			this->pMeshes[i].MaterialIdx = mIndex; //Index setzen
			aiMaterial* aiMaterial = pScene->mMaterials[mIndex];

			//Color laden
			aiColor4D aiColorDiffuse;
			aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_DIFFUSE, &aiColorDiffuse);
			this->pMaterials[mIndex].DiffColor.R = aiColorDiffuse.r;
			this->pMaterials[mIndex].DiffColor.G = aiColorDiffuse.g;
			this->pMaterials[mIndex].DiffColor.B = aiColorDiffuse.b;

			aiColor4D aiColorSpecular;
			float specularExponent;
			aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &specularExponent);
			aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_SPECULAR, &aiColorSpecular);
			this->pMaterials[mIndex].SpecColor.R = aiColorSpecular.r;
			this->pMaterials[mIndex].SpecColor.G = aiColorSpecular.g;
			this->pMaterials[mIndex].SpecColor.B = aiColorSpecular.b;
			this->pMaterials[mIndex].SpecExp = specularExponent;

			aiColor4D aiColorAmbient;
			aiGetMaterialColor(aiMaterial, AI_MATKEY_COLOR_AMBIENT, &aiColorAmbient);
			this->pMaterials[mIndex].AmbColor.R = aiColorAmbient.r;
			this->pMaterials[mIndex].AmbColor.G = aiColorAmbient.g;
			this->pMaterials[mIndex].AmbColor.B = aiColorAmbient.b;

			/*cout << "colors: " << aiColorAmbient.r << " - " << aiColorSpecular.r << " - " << aiColorDiffuse.r << endl;
			cout << "colors: " << aiColorAmbient.g << " - " << aiColorSpecular.g << " - " << aiColorDiffuse.g << endl;
			cout << "colors: " << aiColorAmbient.b << " - " << aiColorSpecular.b << " - " << aiColorDiffuse.b << endl;*/

			//String
			aiString texturename;
			aiGetMaterialString(aiMaterial, AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), &texturename);
			string str = texturename.C_Str();
			//cout << "(Path + str): " << (Path + str) << endl;
			const Texture* texture = Texture::LoadShared((Path + str).c_str());

			this->pMaterials[mIndex].DiffTex = texture;
		}
	}
}

void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{
	Vector min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector max(0, 0, 0);
	for (int i = 0; i < pScene->mNumMeshes; i++) {
		for (int j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {
			Vector p(
				pScene->mMeshes[i]->mVertices[j].x,
				pScene->mMeshes[i]->mVertices[j].y,
				pScene->mMeshes[i]->mVertices[j].z
			);
			min.X = p.X < min.X ? p.X : min.X;
			min.Y = p.Y < min.Y ? p.Y : min.Y;
			min.Z = p.Z < min.Z ? p.Z : min.Z;
			max.X = p.X > max.X ? p.X : max.X;
			max.Y = p.Y > max.Y ? p.Y : max.Y;
			max.Z = p.Z > max.Z ? p.Z : max.Z;
		}
	}
	Box.Min = min;
	Box.Max = max;
}

/**
* finds the dimension of the model which is closest to maxedgeLength
* calcs scale so that the closest to maxEdgeLenth dimension now equals maxEdgeLength
* the scale is for every dimesnion bc the model keeps its ratio
* @param float maxEdgeLength float size to fit
* @returns float scale
*/
float Model::fitSize(float maxEdgeLength = 5.0f) {
	float scale = 1.0f;
	float actualEdgeLengthX = (this->BoundingBox.Max - this->BoundingBox.Min).X;
	float actualEdgeLengthY = (this->BoundingBox.Max - this->BoundingBox.Min).Y;
	float actualEdgeLengthZ = (this->BoundingBox.Max - this->BoundingBox.Min).Z;
	float xDiff = abs(actualEdgeLengthX - maxEdgeLength);
	float yDiff = abs(actualEdgeLengthY - maxEdgeLength);
	float zDiff = abs(actualEdgeLengthZ - maxEdgeLength);
	float minDiff = min(min(xDiff, yDiff), zDiff);
	if (minDiff == xDiff) {
		scale = scale + ((maxEdgeLength - actualEdgeLengthX) / actualEdgeLengthX);
	}
	else if (minDiff == yDiff) {
		scale = scale + ((maxEdgeLength - actualEdgeLengthY) / actualEdgeLengthY);
	}
	else if (minDiff == zDiff) {
		scale = scale + ((maxEdgeLength - actualEdgeLengthZ) / actualEdgeLengthZ);
	}
	return scale;
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
	return Matrix(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4
	);
}



