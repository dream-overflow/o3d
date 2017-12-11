/**
 * @file ms3d.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/utils/ms3d.h"

#include "o3d/core/vector2.h"
#include "o3d/core/debug.h"
#include "o3d/core/diskfileinfo.h"
#include "o3d/core/filemanager.h"
#include "o3d/engine/scene/scene.h"
#include "o3d/engine/object/bones.h"
#include "o3d/engine/object/mtransform.h"
#include "o3d/engine/object/meshdatamanager.h"
#include "o3d/engine/object/pointgizmo.h"
#include "o3d/engine/object/cubegizmo.h"
#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/engine/texture/texturemanager.h"
#include "o3d/engine/material/ambientmaterial.h"
#include "o3d/engine/material/lambertmaterial.h"
#include "o3d/engine/material/pickingmaterial.h"
#include "o3d/engine/scene//sceneobjectmanager.h"

#include "o3d/engine/animation/animationmanager.h"
#include "o3d/engine/animation/animationtrack.h"
#include "o3d/engine/animation/animationnode.h"
#include "o3d/engine/animation/animationplayermanager.h"

using namespace o3d;

Bool Ms3d::import(Scene *scene,
        InStream &is,
        const String &path,
        const String &baseName,
        const Ms3dSettings &settings)
{
	Ms3d ms3d(scene);
	ms3d.settings(settings);

    return ms3d.load(is, baseName, path);
}

Bool Ms3d::import(Scene *scene, const String &filename, const Ms3dSettings &settings)
{
	Ms3d ms3d(scene);
	ms3d.settings(settings);

	return ms3d.load(filename);
}

// Constructor
Ms3d::Ms3d(Scene *scene) :
	m_scene(scene),
	m_objectId(0),
	m_loaded(False),
    m_rootNode(nullptr),
    m_mesh(nullptr),
    m_skin(nullptr),
	m_numGroups(0)
{
}

// Destructor
Ms3d::~Ms3d()
{
}

// Load
Bool Ms3d::load(InStream &is, const String& baseName, const String &path)
{
	if (m_loaded)
		O3D_ERROR(E_InvalidOperation("This object already contain some loaded data"));

	if (!m_settings.m_parentNode)
		m_settings.m_parentNode = m_scene->getHierarchyTree()->getRootNode();

	Char id[10];		// "MS3D000000"
	UInt32 vers;        // version ('04'00'00'00')
	UInt16 word;		// word to int
	Char verif_id[10] = { 'M','S','3','D','0','0','0','0','0','0' };
	UInt8 flag;			// useless
	Char name[32];		// names

    is.read(id, 10);    // id MS3D000000

	if (memcmp(verif_id,id,10) != 0)
        O3D_ERROR(E_InvalidFormat("Invalid MS3D token"));

    is >> vers;

	if (vers > 4)
        O3D_ERROR(E_InvalidFormat("Invalid MS3D version (0x04000000 max)"));

	// n vertex
	UInt32 nVertex;
    is >> word;
	nVertex = word;

	ArrayFloat vertices(nVertex*3,768);
	Char* boneId = new Char[nVertex];
	Bool hasBones = False;

	// vertex informations
	UInt8 ref_count;	// see later

	for (UInt32 i = 0; i < nVertex; ++i)
	{
        is >> flag;
        is.read(&vertices[i*3], 3);
        is >> boneId[i]
			 >> ref_count;

		if (boneId[i] != -1)
			hasBones = True;
	}

	// n faces
	UInt32 nFaces;
    is >> word;
	nFaces = word;

	UInt16* faces = new UInt16[nFaces*3];

	// faces informations
	UInt8 smoothing;	// smoothing value
	UInt8 groupeId;		// groupe id

	Float *normals = new Float[nFaces*3*3];
	Float *texCoordS = new Float[nFaces*3];
	Float *texCoordT = new Float[nFaces*3];

	// lecture et transformation des donnees de face en donnees normals/uv
	for (UInt32 j = 0; j < nFaces; ++j)
	{
		// stupid flag
        is >> word;

		// read 3 indices
        is.read(&faces[j*3], 3);

		// read 3 normals
        is.read(&normals[j*3*3], 9);

		// read 3 uv
        is.read(&texCoordS[j*3], 3);
        is.read(&texCoordT[j*3], 3);

		// others
        is >> smoothing;
        is >> groupeId;
	}


	// groups informations
	UInt32 numFaces;
    UInt16* facesIndices = nullptr;
	UInt8* materialId;
    is >> word;
	m_numGroups = word;

	materialId = new UInt8[m_numGroups];
	m_mesh = new Mesh(m_scene);

    String rootName = baseName;
    m_mesh->setName(rootName);

	// a group match to a mesh, but the data are globals. So we use
	// one mesh data object.
	ArrayFloat meshVertices(nVertex*3,768);
	ArrayFloat meshNormals(nVertex*3,768);
	ArrayFloat meshUV(nVertex*2,512);
	ArrayUInt8 boneIdList(nVertex,256);

	Bool createMeshData = False;

    MeshData *meshData = nullptr;
    String meshDataName = rootName;

	// find the mesh data
    if (m_scene->getMeshDataManager()->isMeshData(meshDataName + ".o3dms")) {
		meshData = m_scene->getMeshDataManager()->addMeshData(meshDataName + ".o3dms");
    } else {
		createMeshData = True;
		meshData = new MeshData(m_scene);
		meshData->setName(meshDataName);
		meshData->setResourceName(meshDataName + ".o3dms");

		meshData->setGeometry(new GeometryData(meshData));

		m_scene->getMeshDataManager()->addMeshData(meshData);
    }

	UInt32 vertexLimit = 0;

	// result
    if (m_settings.m_result) {
			m_settings.m_result->m_numFaces = 0;
    }

	// for each group element (material+faceArray)
    for (UInt32 k = 0; k < m_numGroups; ++k) {
        is >> flag;
        is.read(name, 32);

        is >> word;
		numFaces = word;

		facesIndices = new UInt16[numFaces];
        is.read(facesIndices, numFaces);
        is >> materialId[k];

		if (createMeshData)
		{
			SmartArrayUInt16 facesArray(numFaces*3);

			for (UInt32 m = 0 ; m < numFaces ; ++m)
			{
				UInt32 vertexId,id,id2,id3,new_id2,new_id3;

				for (UInt32 n = 0 ; n < 3 ; ++n)
				{
					vertexId = faces[facesIndices[m]*3+n];

					new_id2 = vertexLimit * 2;
					new_id3 = vertexLimit * 3;

					id  = vertexId * 3;
					id2 = facesIndices[m]*9+n*3;
					id3 = facesIndices[m]*3+n;

					UInt32 g;

					for (g = 0 ; g < vertexLimit ; ++g)
					{
						UInt32 pid = g*3;

						if (meshVertices[pid]   == vertices[id]   &&
							meshVertices[pid+1] == vertices[id+1] &&
							meshVertices[pid+2] == vertices[id+2] &&
							meshNormals[pid]    == normals[id2]   &&
							meshNormals[pid+1]  == normals[id2+1] &&
							meshNormals[pid+2]  == normals[id2+2] &&
							meshUV[g*2  ]       == texCoordS[id3] &&
							meshUV[g*2+1]       == texCoordT[id3])
						{
							facesArray[m*3+n] = (UInt16)g;
							break;
						}
					}
					if (g == vertexLimit)
					{
						Float a = vertices[id  ];
						Float b = vertices[id+1];
						Float c = vertices[id+2];

						meshVertices[new_id3  ] = a;
						meshVertices[new_id3+1] = b;
						meshVertices[new_id3+2] = c;

						meshNormals[new_id3  ] = normals[id2  ];
						meshNormals[new_id3+1] = normals[id2+1];
						meshNormals[new_id3+2] = normals[id2+2];

						meshUV[new_id2  ] = texCoordS[id3];
						meshUV[new_id2+1] = texCoordT[id3];

						boneIdList[vertexLimit] = boneId[vertexId];

						facesArray[m*3+n] = (UInt16)vertexLimit;
						++vertexLimit;
					}
				}
			}

			// and create the faceArray
            FaceArrayUInt16 *faceArrayUS = new FaceArrayUInt16(m_scene->getContext(), P_TRIANGLES);

			faceArrayUS->setFaces(facesArray);
			//faceArray->setName(name);

			meshData->getGeometry()->addFaceArray(k, faceArrayUS);
		}

		// result
		if (m_settings.m_result)
			m_settings.m_result->m_numFaces += numFaces;

		// delete the face list
		deleteArray(facesIndices);
	}

	// now delete ms3d mesh data
	deleteArray(faces);
	deleteArray(texCoordS);
	deleteArray(texCoordT);
	deleteArray(normals);

	nVertex = vertexLimit;

	if (createMeshData)
	{
		// result
		if (m_settings.m_result)
			m_settings.m_result->m_numVertices = meshVertices.getNumElt() / 3;

		meshData->getGeometry()->createElement(
				V_VERTICES_ARRAY,
				SmartArrayFloat(meshVertices.getData(),meshVertices.getNumElt()));
		vertices.destroy();

		meshData->getGeometry()->createElement(
				V_NORMALS_ARRAY,
				SmartArrayFloat(meshNormals.getData(),meshNormals.getNumElt()));
		meshNormals.destroy();

		meshData->getGeometry()->createElement(
				V_TEXCOORDS_2D_1_ARRAY,
				SmartArrayFloat(meshUV.getData(),meshUV.getNumElt()));
		meshUV.destroy();

		// if rigging data
		if (hasBones)
		{
			SmartArrayFloat riggingArray(nVertex);

			// convert char to float
			for (UInt32 i = 0 ; i < nVertex ; ++i)
				riggingArray[i] = boneIdList[i] != 255 ? boneIdList[i] : -1.0f;

			meshData->getGeometry()->createElement(V_RIGGING_ARRAY, riggingArray);
		}

		meshData->computeBounding(m_settings.getBoundingVolumeGen());
		meshData->createGeometry();
	}
	else
	{
		// result
		if (m_settings.m_result)
			m_settings.m_result->m_numVertices = meshData->getNumVertices();

		if (m_settings.m_result)
			m_settings.m_result->m_numFaces = meshData->getNumFaces();
	}

	m_mesh->setMeshData(meshData);

	// material
	Color ambient, diffuse, specular, emissive;
	Float shininess;
	Float transparency;
	Char diffuseMap[128];
	Char alphaMap[128];

    is >> word;
	UInt32 numMaterials = word;

	m_mesh->setNumMaterialProfiles(m_numGroups);
	for (UInt32 i = 0; i < m_numGroups; ++i)
	{
		m_mesh->getMaterialProfile(i).setNumTechniques(1);
		m_mesh->getMaterialProfile(i).getTechnique(0).setNumPass(1);
	}

	for (UInt32 o = 0; o < numMaterials; ++o)
	{
        is.read(name, 32);

        is >> ambient
           >> diffuse
           >> specular
           >> emissive
           >> shininess
           >> transparency
           >> flag;

        is.read(diffuseMap, 128);
        is.read(alphaMap, 128);

		// set parameters to each group that use this material
		for (UInt32 s = 0; s < m_numGroups; ++s)
		{
			if (materialId[s] == o)
			{
				MaterialPass &pass = m_mesh->getMaterialProfile(s).getTechnique(0).getPass(0);

                pass.setAmbient(ambient);
				pass.setDiffuse(diffuse);
				pass.setSpecular(specular);
				pass.setSelfIllumination(emissive);
				pass.setShine(shininess);
				pass.setTransparency(transparency);

				if (diffuseMap[0] != 0)
				{
					// Adapt the path of the texture to a relative path from the scene abs-file-path
                    String textureName = FileManager::instance()->getFullFileName(path + "/" + diffuseMap);
                    textureName = m_scene->getTextureManager()->getResourceName(textureName);

					Texture2D *diffuseMap = m_scene->getTextureManager()->addTexture2D(textureName, True);

					pass.setDiffuseMap(diffuseMap);
					pass.setMapFiltering(MaterialPass::DIFFUSE_MAP, m_settings.m_filtering);
					pass.setMapWarp(MaterialPass::DIFFUSE_MAP, Texture::CLAMP);
					pass.setMapAnisotropy(MaterialPass::DIFFUSE_MAP, m_settings.m_anisotropy);
				}

				if (alphaMap[0] != 0)
				{
					// Adapt the path of the texture to a relative path from the scene abs-file-path
                    String textureName = FileManager::instance()->getFullFileName(path + "/" + alphaMap);
                    textureName = m_scene->getTextureManager()->getResourceName(textureName);

					Texture2D *alphaMap = m_scene->getTextureManager()->addTexture2D(textureName, True);

					pass.setOpacityMap(alphaMap);
					pass.setMapFiltering(MaterialPass::OPACITY_MAP, m_settings.m_filtering);
					pass.setMapWarp(MaterialPass::OPACITY_MAP, Texture::CLAMP);
					pass.setMapAnisotropy(MaterialPass::OPACITY_MAP, m_settings.m_anisotropy);
				}

                pass.setMaterial(Material::AMBIENT, new AmbientMaterial(m_scene));
                pass.setMaterial(Material::LIGHTING, new LambertMaterial(m_scene));
				pass.setMaterial(Material::PICKING, new PickingMaterial(m_scene));
                pass.setMaterial(Material::DEFERRED, new LambertMaterial(m_scene)); // TODO BumpMaterial
			}
		}
	}

	// joins (hierarchy) + animation
	UInt32 nJoin;
	Float animationFPS, currentTime;
	UInt32 totalFrames;

    is >> animationFPS
		 >> currentTime
		 >> totalFrames;

    is >> word;
	nJoin = word;

	// bones parent names
	String* parentNames = new String[nJoin];

	// result
	if (m_settings.m_result)
	{
		m_settings.m_result->m_numBones = nJoin;
		m_settings.m_result->m_isRigging = nJoin != 0 ? True : False;
	}

	Bool createAnimation = False;
    String animationName = rootName;

	// do we create the animation ?
	if (!m_scene->getAnimationManager()->isAnimation(animationName + ".o3dan"))
		createAnimation = True;

	if (nJoin != 0)
	{
		Vector3 rot, pos;

		m_skin = new Rigging(*m_mesh);
		deletePtr(m_mesh);

		m_objectId = m_skin->getId();

		m_skin->setNumBones(nJoin);

		// root node
		Node *rootNode = new Node(m_settings.m_parentNode);
		m_rootNode = rootNode;
		m_rootNode->setName(rootName);

		m_settings.m_parentNode->addSonLast(rootNode);

        // skeleton and root bone
        Skeleton *skeleton = m_skin->getSkeleton();
        Bones *skeletonRoot = skeleton->createBones();

		// root animation node
        AnimationNode *rootTrackNode = nullptr;

		if (createAnimation)
			rootTrackNode = new AnimationNode;

		Bool keepAnim = False;

		// rotation/translation keyframe numbers
		UInt16 numKeyFrameRot;
		UInt16 numKeyFramePos;

		// rotation/translation keyframe times
		Float keyFrameRotTime;
		Float keyFramePosTime;

		Vector3 keyFrameRot;
		Vector3 keyFramePos;

		// a constructed skeleton tree
        AnimationNode **animationTree = nullptr;
		String *joinName = new String[nJoin];

		if (createAnimation)
			animationTree = new AnimationNode*[nJoin];

		// first pass read all joins
		for (UInt8 q = 0; q < nJoin; ++q)
		{
            Bones *father = nullptr;
            AnimationNode *trackNode = nullptr;

            is >> flag;

            is.read(name, 32);
			joinName[q] = rootName + "." + name;

            is.read(name, 32);
			parentNames[q] = rootName + "." + name;

            is   >> rot
				 >> pos
				 >> numKeyFrameRot
				 >> numKeyFramePos;

			MTransform *localTransform = new MTransform;
			// initial position
			localTransform->setPosition(pos);

			// initial rotation
			Quaternion quat;
			quat.fromEuler(rot);
			localTransform->rotate(quat);

			// not a root join
			if (name[0] != 0)
			{
                father = (Bones*)skeletonRoot->findSon(parentNames[q]);
				O3D_ASSERT(father);

				// compute the reference matrix, depend on the parent absolute matrix
				m_skin->setRefMatrix(q, father->getAbsoluteMatrix() * localTransform->getMatrix());

				// search its father animation node and add to it
				if (createAnimation)
				{
					for (UInt8 currentFather = 0; currentFather < q; ++currentFather)
					{
						if (joinName[currentFather] == parentNames[q])
						{
							trackNode = new AnimationNode(animationTree[currentFather]);
							animationTree[currentFather]->addSonLast(*trackNode);

							break;
						}
					}

					O3D_ASSERT(trackNode);
				}
			}
			else
			{
                father = skeletonRoot;

				// compute the reference matrix, depend on the parent absolute matrix
                m_skin->setRefMatrix(q, skeletonRoot->getAbsoluteMatrix() * localTransform->getMatrix());

				if (createAnimation)
				{
                    trackNode = new AnimationNode(rootTrackNode);
                    rootTrackNode->addSonLast(*trackNode);
				}
			}

			if (createAnimation)
				animationTree[q] = trackNode;

            Bones *join = skeleton->createBones(father);

			join->setName(joinName[q]);
			join->addTransform(localTransform);

			m_skin->setBone(q,join);

			// we update the subtree part for computes absolutes matrices
			if (join)
				join->update();

			// only if key frames...
			if (numKeyFrameRot || numKeyFramePos)
			{
				keepAnim = True;

				// animations track
                AnimationTrack_SmoothQuaternion *trackQ = nullptr;
                AnimationTrack_LinearVector *track = nullptr;

				if (numKeyFrameRot && createAnimation)
				{
					trackQ = new AnimationTrack_SmoothQuaternion(
							AnimationTrack::TARGET_OBJECT_ROT,
							0,
							AnimationTrack::TRACK_MODE_CONSTANT,
							AnimationTrack::TRACK_MODE_CONSTANT);

					trackNode->addTrack(*(AnimationTrack*)trackQ);
				}

				if (numKeyFramePos && createAnimation)
				{
					track = new AnimationTrack_LinearVector(
							AnimationTrack::TARGET_OBJECT_POS,
							0,
							AnimationTrack::TRACK_MODE_CONSTANT,
							AnimationTrack::TRACK_MODE_CONSTANT);

					trackNode->addTrack(*(AnimationTrack*)track);
				}

				// Animation tracks
				for (UInt32 a = 0; a < numKeyFrameRot; ++a)
				{
                    is >> keyFrameRotTime
                       >> keyFrameRot;

					if (createAnimation)
					{
						keyFrameRotTime /= (Float)totalFrames / (Float)animationFPS;

						KeyFrameSmooth<Quaternion> *feyFrameQ = new KeyFrameSmooth<Quaternion>;

						feyFrameQ->setTime(keyFrameRotTime);
						feyFrameQ->Data.fromEuler(keyFrameRot);
						trackQ->addKeyFrame(*(KeyFrame*)feyFrameQ);
					}
				}

				for (UInt32 a = 0; a < numKeyFramePos; ++a)
				{
                    is >> keyFramePosTime
                       >> keyFramePos;

					if (createAnimation)
					{
						keyFramePosTime /= (Float)totalFrames / (Float)animationFPS;

						KeyFrameLinear<Vector3> *keyFrame = new KeyFrameLinear<Vector3>;

						keyFrame->setTime(keyFramePosTime);
						keyFrame->Data = keyFramePos;
						track->addKeyFrame(*(KeyFrame*)keyFrame);
					}
				}
			}
		}

		// add the skin
		rootNode->addSonLast(m_skin);

		if (keepAnim)
		{
			// Animation
            Animation *animation = nullptr;

			// find the animation
			if (!createAnimation)
				animation = m_scene->getAnimationManager()->addAnimation(animationName + ".o3dan");
			else
			{
				// otherwise create a new one
				animation =	new Animation(m_scene);
				animation->setName(animationName);
				animation->setResourceName(animationName + ".o3dan");

				animation->setNumFrames(totalFrames);
				animation->setDuration((Float)totalFrames / (Float)animationFPS);
				animation->setNumObject(nJoin);
				animation->setFatherNode(rootTrackNode);

				m_scene->getAnimationManager()->addAnimation(animation);
			}

            // Animation player of the skeleton
			AnimationPlayer *animationPlayer = m_scene->getAnimationPlayerManager()->createAnimationPlayer(animation);
			animationPlayer->setPlayerMode(AnimationPlayer::MODE_LOOP);
			animationPlayer->play();
			animationPlayer->setFramePerSec(animationFPS);
            animationPlayer->setAnimatable(skeletonRoot);

			// Add an animation queue
			m_scene->getAnimationPlayerManager()->add(*animationPlayer);

			// result
			if (m_settings.m_result)
			{
				m_settings.m_result->m_animation = animation;
				m_settings.m_result->m_animationPlayer = animationPlayer;
			}
		}

		m_skin->initialize();

		deleteArray(animationTree);
		deleteArray(joinName);

        // result
        if (m_settings.m_result)
        {
            m_settings.m_result->m_meshOrSkin = m_skin;
            m_settings.m_result->m_rootNode = rootNode;
        }
	}
	else
	{
		// root node
		Node *rootNode = new Node(m_settings.m_parentNode);
        rootNode->setName(rootName);

		m_settings.m_parentNode->addSonLast(rootNode);

		// add the mesh to the hierarchy
		rootNode->addSonLast(m_mesh);
		m_objectId = m_mesh->getId();

		// result
		if (m_settings.m_result)
		{
			m_settings.m_result->m_meshOrSkin = m_mesh;
			m_settings.m_result->m_rootNode = rootNode;
		}
	}

	// initialize material profile
	if (m_mesh)
		m_mesh->initMaterialProfiles();
	else if (m_skin)
		m_skin->initMaterialProfiles();

	deleteArray(materialId);
	deleteArray(parentNames);
	deleteArray(boneId);

	m_loaded = True;

	return True;
}

// Load MS3D file.
Bool Ms3d::load(const String& filename)
{
    DiskFileInfo file(filename);

    InStream *is = FileManager::instance()->openInStream(filename);
    if (is)
	{
        String baseName = file.getFileName();
        if (baseName.endsWith(".ms3d"))
            baseName.truncate(baseName.length() - 5);

        Bool result = load(*is, baseName, file.getFilePath());

        deletePtr(is);
		return result;
	}
	return False;
}

// Unload MS3D created content.
void Ms3d::destroy()
{
	if (m_loaded)
	{
		deletePtr(m_rootNode);

        m_mesh = nullptr;
        m_skin = nullptr;

		m_objectId = -1;
		m_numGroups = 0;

		m_loaded = False;
	}
}

