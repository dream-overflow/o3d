/**
 * @file pclodtexturemanager.cpp
 * @brief 
 * @author Emmanuel RUFFIO (emmanuel.ruffio@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/engine/precompiled.h"
#include "o3d/engine/landscape/pclod/pclodtexturemanager.h"

#include "o3d/engine/landscape/pclod/terrain.h"
#include "o3d/engine/landscape/pclod/configs.h"
#include "o3d/engine/landscape/pclod/object.h"
#include "o3d/engine/landscape/pclod/zonemanager.h"
#include "o3d/engine/landscape/pclod/pclodrenderer.h"

#include "o3d/engine/landscape/pclod/pclodmaterial.h"
#include "o3d/engine/landscape/pclod/colormap.h"
#include "o3d/engine/landscape/pclod/lightmap.h"
#include "o3d/engine/landscape/pclod/debuglabel.h"

#include "o3d/engine/context.h"
#include "o3d/engine/framebuffer.h"
#include "o3d/engine/hierarchy/hierarchytree.h"
#include "o3d/engine/texture/texture2d.h"
#include "o3d/engine/scene/scene.h"

#include "o3d/engine/object/mtransform.h"

#include "o3d/core/objects.h"
#include "o3d/core/filemanager.h"

#include "o3d/image/image.h"

#include <memory>

using namespace o3d;

O3D_IMPLEMENT_DYNAMIC_CLASS1(PCLODTextureManager, ENGINE_PCLOD_TEXTURE_MANAGER, SceneEntity)

//---------------------------------------------------------------------------------------
// class PCLODTextureManager
//---------------------------------------------------------------------------------------
PCLODTextureManager::PCLODTextureManager(BaseObject *parent) :
    SceneEntity(parent),
    m_semaphore(),
    m_materialMap(),
    m_colormapMap(),
    m_lightmapMap(),
    m_lightArray(),
    m_labelMap(),
    m_textureFile(),
    m_colormapDirectory(),
    m_materialDirectory(),
    m_pZoneManager(nullptr),
    m_text(this),
    m_pFrameBuffer(nullptr),
    m_staticNoise(),
    m_colormapLodCurve(),
    m_lightmapLodCurve()
{

}

PCLODTextureManager::PCLODTextureManager(PCLODZoneManager * _pZoneManager) :
    SceneEntity(_pZoneManager),
    m_semaphore(),
    m_materialMap(),
    m_colormapMap(),
    m_lightmapMap(),
    m_lightArray(),
    m_labelMap(),
    m_textureFile(),
    m_colormapDirectory(),
    m_materialDirectory(),
    m_pZoneManager(_pZoneManager),
    m_text(this),
    m_pFrameBuffer(nullptr),
    m_staticNoise(),
    m_colormapLodCurve(),
    m_lightmapLodCurve()
{
    m_pFrameBuffer = new FrameBuffer(getScene()->getContext());

    m_pZoneManager->onQuadtreeMoved.connect(this, &PCLODTextureManager::rteOnQuadTreeMoved, EvtHandler::CONNECTION_SYNCH);

    onLightmapDeletion.connect(this, &PCLODTextureManager::rteOnLightmapDeletion, EvtHandler::CONNECTION_ASYNCH);
    onMaterialDeletion.connect(this, &PCLODTextureManager::rteOnMaterialDeletion, EvtHandler::CONNECTION_ASYNCH);
    onColormapDeletion.connect(this, &PCLODTextureManager::rteOnColormapDeletion, EvtHandler::CONNECTION_ASYNCH);

    onZoneLoaded.connect(this, &PCLODTextureManager::rteOnZoneLoaded, EvtHandler::CONNECTION_ASYNCH);
    onZoneRelease.connect(this, &PCLODTextureManager::rteOnZoneRelease, EvtHandler::CONNECTION_ASYNCH);
}

PCLODTextureManager::~PCLODTextureManager()
{
    destroy();
}

const PCLODTerrain* PCLODTextureManager::getTerrain() const
{
    return m_pZoneManager->getTerrain();
}

PCLODTerrain* PCLODTextureManager::getTerrain()
{
    return m_pZoneManager->getTerrain();
}

const PCLODZoneManager * PCLODTextureManager::getZoneManager() const
{
    return m_pZoneManager;
}

PCLODZoneManager * PCLODTextureManager::getZoneManager()
{
    return m_pZoneManager;
}

/* Release all textures and materials */
void PCLODTextureManager::destroy()
{
    PCLOD_MESSAGE(String("TextureManager : Termination requested"));

    while ((EvtManager::instance()->processEvent(this) > 0) ||
            (EvtManager::instance()->processEvent(this, m_pZoneManager->getRefreshThread()) > 0)) {}

    UInt32 matLeft = 0;

    for (CIT_ColormapMap it = m_colormapMap.begin() ; it != m_colormapMap.end() ; it++)
    {
        if (it->second->loaded())
            matLeft++;
    }

    if ((m_colormapMap.size() > 0) || (matLeft > 0) || (m_lightmapMap.size() > 0))
    {
        PCLOD_WARNING(String("TextureManager : Texture manager termination : ") <<
                UInt32(m_colormapMap.size()) << " colormaps, " <<
                UInt32(m_lightmapMap.size()) << " lightmaps, and " <<
                matLeft << " materials still used. Clean forced");
    }
    else
        PCLOD_MESSAGE(String("TextureManager : All texture was cleanly removed"));

    for (IT_MaterialMap it = m_materialMap.begin() ; it != m_materialMap.end() ; it++)
    {
        if (it->second->loaded())
        {
            // Si le TM n'est pas le seul à posséder une instance...
            if (it->second->getReferenceCounter() > 0)
            {
                PCLOD_WARNING(String("TextureManager : Material (") << it->second->getMatId() <<
                        " still used and owned by " << it->second->getReferenceCounter() <<
                        " objects. Destroy forced");
            }

            Texture2D * lpTexture = it->second->getTexture();
            it->second->rtUnload();
            deletePtr(lpTexture);
        }

        deletePtr(it->second);
    }

    m_materialMap.clear();

    // On libère les colormaps
    for (IT_ColormapMap it = m_colormapMap.begin() ; it != m_colormapMap.end() ; it++)
    {
        if (it->second->loaded())
        {
            // Si le TM n'est pas le seul à posséder une instance...
            if (it->second->getReferenceCounter() > 0)
            {
                PCLOD_WARNING(String("TextureManager : Colormap ") << it->second->getZoneId() <<
                        " still used and owned by " << it->second->getReferenceCounter() <<
                        " objects. Destroy forced");
            }

            it->second->mtUnload();
        }

        deletePtr(it->second);
    }

    m_colormapMap.clear();

    // On libère les lightmaps
    for (IT_LightmapMap it = m_lightmapMap.begin() ; it != m_lightmapMap.end() ; it++)
    {
        if (it->second->generated())
        {
            // Si le TM n'est pas le seul à posséder une instance...
            if (it->second->getReferenceCounter() > 0)
            {
                PCLOD_WARNING(String("TextureManager : Lightmap ") << it->second->getZoneId() <<
                        " still used and owned by " << it->second->getReferenceCounter() <<
                        " objects. Destroy forced");
            }

            it->second->mtUnload();
        }

        deletePtr(it->second);
    }

    m_lightmapMap.clear();

    // On libère les lumières
    for (IT_LightArray it = m_lightArray.begin() ; it != m_lightArray.end() ; it++)
    {
        deletePtr(*it);
    }

    m_lightArray.clear();

    // On libère les labels
    for (IT_LabelMap it = m_labelMap.begin() ; it != m_labelMap.end() ; it++)
    {
        deletePtr(it->second);
    }

    m_labelMap.clear();

    if (!m_staticNoise.noLongerUsed() && (m_staticNoise.getReferenceCounter() > 1))
    {
        PCLOD_WARNING(String("TextureManager : The static noise buffer is still used by ") <<
                m_staticNoise.getReferenceCounter() - 1 << " objects");
    }

    m_staticNoise.releaseCheckAndDelete();

    m_textureFile.destroy();
    m_colormapDirectory.destroy();
    m_materialDirectory.destroy();

    m_text.releaseCheckAndDelete();
    deletePtr(m_pFrameBuffer);

    m_pZoneManager = nullptr;
}

/* Load/Unload texture.
 * Functions protected by mutexes */
void PCLODTextureManager::rtLoadColormap(PCLODTopZone * _pTopZone)
{
    String colormapPath(m_colormapDirectory);
    colormapPath << String("Zone") << _pTopZone->getId() << String(".cmap");

    PCLODColormap * pNewColormap = new PCLODColormap(
            (BaseObject*)this,
            _pTopZone,
            _pTopZone->getId(),
            colormapPath,
            0);

    PCLODConfigs::PCLODColormapPolicy colormapPolicy = getTerrain()->getCurrentConfigs().colormapPolicy();

    if (getTerrain()->getCurrentConfigs().useColormapStaticNoise())
    {
        Vector2ui baseSize;
        m_pZoneManager->getBaseZoneSize(baseSize);

        pNewColormap->useStaticNoise(m_staticNoise, baseSize[X]-1, baseSize[Y]-1);
    }

    if ((colormapPolicy == PCLODConfigs::COLORMAP_GENERATE_ALWAYS) ||
            (!pNewColormap->rtLoad() && (colormapPolicy != PCLODConfigs::COLORMAP_GENERATE_NEVER)))
    {
        pNewColormap->rtGenerate(m_materialMap, getTerrain()->getCurrentConfigs().colormapPrecision());
    }

    // La zone qui demande la colormap prend l'instance
    pNewColormap->useIt();

    pNewColormap->onTextureUnused.connect(this, &PCLODTextureManager::rteOnColormapUnused, EvtHandler::CONNECTION_ASYNCH);

    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);
    O3D_ASSERT(m_colormapMap.find(_pTopZone->getId()) == m_colormapMap.end());
    m_colormapMap[_pTopZone->getId()] = pNewColormap;
}

void PCLODTextureManager::rtRemoveColormap(UInt32 _colormapId)
{
    // L'événement est synchrone donc aucune modification n'a peu avoir lieu.
    // SI bien qu'on est sur que la colormap est toujours inutilisée.
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    IT_ColormapMap itColormap = m_colormapMap.find(_colormapId);
    O3D_ASSERT(itColormap != m_colormapMap.end());
    O3D_ASSERT(itColormap->second->noLongerUsed());

    // Une fois enlevé de la liste des colormaps, on envoit un signal de demande de
    // destruction. Les derniers nettoyages pourront être fait dans le thread
    // principal.
    onColormapDeletion(itColormap->second);

    m_colormapMap.erase(itColormap);
}

void PCLODTextureManager::rtLoadLightmap(PCLODTopZone * _pTopZone)
{
    PCLODLightmap * pNewLightmap = new PCLODLightmap((BaseObject*)this, _pTopZone);

    // La zone qui demande la colormap prend l'instance
    pNewLightmap->useIt();

    // Plutot que d'aller interroger la grosse std::map, on recalcule l'extension de la zone
    Vector2ui baseSize;
    _pTopZone->getZoneManager()->getBaseZoneSize(baseSize);

    Vector2ui extension;
    extension[X] = (_pTopZone->getZoneSize()[X]-1) / (baseSize[X]-1);
    extension[Y] = (_pTopZone->getZoneSize()[Y]-1) / (baseSize[Y]-1);

    std::set<UInt32> zoneNeighbors[PCLODLightmap::DIR_SIZE];

    // On détermine les voisins dans chaque direction
    Vector2i zoneArrayIndex;
    zoneArrayIndex[X] = _pTopZone->getZoneOrigin()[X] / Int32(baseSize[X]-1);
    zoneArrayIndex[Y] = _pTopZone->getZoneOrigin()[Y] / Int32(baseSize[Y]-1);

    // Voisins de gauche
    Vector2i position = zoneArrayIndex;
    position[X]--;

    for (UInt32 y = 0 ; y < extension[Y] ; ++y, position[Y]++)
    {
        UInt32 id = m_pZoneManager->getZoneIdFromPosition(position);
        zoneNeighbors[PCLODLightmap::DIR_W].insert(id);
    }

    // Voisins de droite
    position = zoneArrayIndex;
    position[X] += extension[X];

    for (UInt32 y = 0 ; y < extension[Y] ; ++y, position[Y]++)
    {
        zoneNeighbors[PCLODLightmap::DIR_E].insert(
                m_pZoneManager->getZoneIdFromPosition(position));
    }

    // Voisins du dessus
    position = zoneArrayIndex;
    position[Y] += extension[Y];

    for (UInt32 y = 0 ; y < extension[Y] ; ++y, position[X]++)
    {
        zoneNeighbors[PCLODLightmap::DIR_N].insert(
                m_pZoneManager->getZoneIdFromPosition(position));
    }

    // Voisins du dessous
    position = zoneArrayIndex;
    position[Y]--;

    for (UInt32 y = 0 ; y < extension[Y] ; ++y, position[X]++)
    {
        zoneNeighbors[PCLODLightmap::DIR_S].insert(
                m_pZoneManager->getZoneIdFromPosition(position));
    }

    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    O3D_ASSERT(m_lightmapMap.find(_pTopZone->getId()) == m_lightmapMap.end());
    m_lightmapMap[_pTopZone->getId()] = pNewLightmap;

    pNewLightmap->onTextureUnused.connect(this, &PCLODTextureManager::rteOnLightmapUnused, EvtHandler::CONNECTION_ASYNCH);//m_pZoneManager->getRefreshThread());

    for (UInt32 k = 0 ; k < UInt32(PCLODLightmap::DIR_SIZE) ; ++k)
    {
        zoneNeighbors[k].erase(0);

        for (std::set<UInt32>::iterator it = zoneNeighbors[k].begin(); it != zoneNeighbors[k].end(); it++)
        {
            IT_LightmapMap itLightmap = m_lightmapMap.find(*it);

            if (itLightmap != m_lightmapMap.end())
            {
                pNewLightmap->setNeighbor(
                        static_cast<PCLODLightmap::ZoneDirection>(k),
                            itLightmap->second);
                }
            }
        }

        // On ajoute à la lightmap l'ensemble des lumières qui agissent sur elle
        for (IT_LightArray it = m_lightArray.begin() ; it != m_lightArray.end() ; it++)
        {
            if ((*it)->mtAffectLightmap(pNewLightmap))
            {
                (*it)->mtAddLightmap(pNewLightmap);
                (*it)->invalidate();
            }
        }
}

void PCLODTextureManager::rtRemoveLightmap(UInt32 _lightmapId)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    IT_LightmapMap itLightmap = m_lightmapMap.find(_lightmapId);

    // Comme les insertions des lightmaps (RT_LoadLightmap) a lieu dans le thread
    // de refresh. Il n'y a aucune raison qu'une zone est réutilisé la lightmap
    O3D_ASSERT(itLightmap != m_lightmapMap.end());
    O3D_ASSERT(itLightmap->second->noLongerUsed());

    // Une fois enlevé de la liste des lightmaps, on envoit un signal de demande de
    // destruction de la lightmap. Les derniers nettoyages pourront être fait
    // dans le thread principal tel que la libération des ressources openGL.
    onLightmapDeletion(itLightmap->second);

    m_lightmapMap.erase(itLightmap);
}

void PCLODTextureManager::rtLoadMaterial(PCLODMaterial * _pMaterial)
{
    // Ici, on est sur que UnloadMaterial n'est pas en cours d'éxécution
    // Aucune modification d'état Loaded/Unloaded ne peut entrer en conflit

    // L'appel a loadMaterial résulte tjs d'une requete d'une zone, donc on incrémente
    // le compteur de référence du matériau

    if (!_pMaterial->loaded())
        _pMaterial->rtLoad();

    _pMaterial->useIt();
}

void PCLODTextureManager::rtUnloadMaterial(UInt32 _materialId)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

    // L'événement est synchrone donc aucune modification n'a peu avoir lieu.
    // SI bien qu'on est sur que la colormap est toujours inutilisée.
    IT_MaterialMap itMat = m_materialMap.find(_materialId);
    O3D_ASSERT(itMat != m_materialMap.end());
    O3D_ASSERT(itMat->second->noLongerUsed());

    // On envoie un signal de demande de destruction. Les derniers nettoyages
    // pourront être fait dans le thread principal. Les matériaux sont un peu
    // particulier puisqu'ils ne sont jamais détruis donc on libère juste la
    // mémoire vidéo occupée.
    Texture2D * lpTexture = itMat->second->getTexture();

    itMat->second->rtUnload();
    // On attend d'avoir laissé le matériau libéré la texture pour envoyer la
    // requete de suppression.
    onMaterialDeletion(lpTexture);
}

/* Functions */

/* Process gui events (MAIN THREAD) */
void PCLODTextureManager::mtUpdate()
{
    if (getTerrain()->getCurrentConfigs().lightningEnabled())
    {
        ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

        // If some lights was destroyed, we must remove them from the light list.
        // It would be better to be warned by an event to avoid this check.
        IT_LightArray itLight = m_lightArray.begin();
        while (itLight != m_lightArray.end())
        {
            if (!(*itLight)->isValid())
            {
                deletePtr(*itLight);
                itLight = m_lightArray.erase(itLight);
            }
            else
            {
                (*itLight)->mtUpdate();
                itLight++;
            }
        }

        for (IT_LightmapMap it = m_lightmapMap.begin() ; it != m_lightmapMap.end() ; it++)
            it->second->mtUpdate();
    }

    if (getTerrain()->getCurrentConfigs().debugLabelEnabled())
    {
        ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

        for (IT_LabelMap it = m_labelMap.begin() ; it != m_labelMap.end() ; it ++)
        {
            IT_ColormapMap itColormap = m_colormapMap.find(it->first);
            IT_LightmapMap itLightmap = m_lightmapMap.find(it->first);

            it->second->mtUpdateContent(
                    m_text.get(),
                    m_pFrameBuffer,
                    itColormap != m_colormapMap.end() ? itColormap->second : nullptr,
                    itLightmap != m_lightmapMap.end() ? itLightmap->second : nullptr);
        }
    }
}

/* Process thread events (REFRESH THREAD) */
void PCLODTextureManager::rtUpdate()
{
    if (getTerrain()->getCurrentConfigs().lightningEnabled())
    {
        ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

        for (IT_LightArray it = m_lightArray.begin() ; it != m_lightArray.end() ; it++)
        {
            (*it)->rtUpdate();
        }

        for (IT_LightmapMap it = m_lightmapMap.begin() ; it != m_lightmapMap.end() ; it++)
        {
            it->second->rtUpdate();
        }
    }
}

/* Initialize the manager */
void PCLODTextureManager::load(
        const String & _texFilePath,
        const String & _materialDir,
        const String & _colormapDir)
{
    m_textureFile = _texFilePath;
    m_materialDirectory = _materialDir;
    m_colormapDirectory = _colormapDir;

    if (m_materialDirectory.length() > 0)
    {
        WChar lastChar = m_materialDirectory[m_materialDirectory.length()-1];

        if ((lastChar != '/') && (lastChar != '\\'))
            m_materialDirectory += String("/");
    }

    if (m_colormapDirectory.length() > 0)
    {
        WChar lastChar = m_colormapDirectory[m_colormapDirectory.length()-1];

        if ((lastChar != '/') && (lastChar != '\\'))
            m_colormapDirectory += String("/");
    }

    AutoPtr<InStream> pTexIs(FileManager::instance()->openInStream(_texFilePath));

    if (pTexIs.get() == nullptr)
    {
        PCLOD_ERROR(String("TextureManager : File <") << _texFilePath << "> introuvable.");
        return;
    }

    InStream & texIs = *pTexIs;

    Char header[16] = { 0 };
    if (texIs.read(header, 16) != 16)
    {
        PCLOD_ERROR("TextureManager : Empty file");
        return;
    }

    if (strncmp((const Char*)header, "O3DCLM TEXFILE  ", 16) != 0)
    {
        PCLOD_ERROR("TextureManager : Format invalid (wrong header)");
        return;
    }

    UInt32 materialNumber;
    texIs >> materialNumber;
    PCLOD_MESSAGE(String("TextureManager : ") << materialNumber << " materials declared");

    for (UInt32 k = 0 ; k < materialNumber ; ++k)
    {
        UInt32 matId;
        String matPath;

        texIs >> matId;
        texIs >> matPath;

        registerMaterial(matId, m_materialDirectory + matPath, 0);
    }

    // In any case, we register the null material
    O3D_ASSERT(m_materialMap.find(0) == m_materialMap.end());
    registerMaterial(0, String(""), 0);
}

/* Initialize the manager */
void PCLODTextureManager::init()
{
    PCLOD_MESSAGE("TextureManager : Initialisation");

    // Next, if a statisNoise is specified in terrain configs, we must scale it to the zone dimension
    const Image * pPicture = nullptr;

    if ((getTerrain()->getCurrentConfigs().useColormapStaticNoise()) &&
            ((pPicture = getTerrain()->getCurrentConfigs().colormapStaticNoisePicture()) != nullptr))
    {
        PCLOD_MESSAGE("TextureManager : Colormap static noise defined in terrain configs.");
        Vector2ui baseSize;
        m_pZoneManager->getBaseZoneSize(baseSize);

        Image resizedNoise(*pPicture);
        resizedNoise.resize(baseSize[X]-1, baseSize[Y]-1, FILTER_NEAREST);

        const Float factor = 2.0f * getTerrain()->getCurrentConfigs().colormapStaticNoiseFactor();

        const UInt32 size = resizedNoise.getWidth() * resizedNoise.getHeight();

        m_staticNoise = SmartArrayInt8(size);

        Int8 * pPtr = m_staticNoise.getData();
        const UInt8 * pNoise = resizedNoise.getData();
        const UInt32 bpp = resizedNoise.getBpp() >> 3;

        // On ne considère que la première composante de l'image
        for (UInt32 k = size; k > 0 ; --k, ++pPtr, pNoise += bpp)
        {
            *pPtr = (Int8)o3d::max<Int16>(
                        o3d::min<Int16>(Int16(factor * (Float(*pNoise) - 128.0f)), 127),
                        -128);
        }
    }

    // Si le LOD des colormaps est activés
    if (getTerrain()->getCurrentConfigs().colormapLod() &&
            (getTerrain()->getCurrentConfigs().colormapCurvePointCount() > 0))
    {
        // La courbe contenu par la config est une courbe fonction de la distance.
        Float maxDistance = 0.0;
        Int8 maxValue = 0;

        getTerrain()->getCurrentConfigs().getColormapPoint(
                getTerrain()->getCurrentConfigs().colormapCurvePointCount()-1,
                maxDistance,
                maxValue);

        Vector2f zoneUnits;
        m_pZoneManager->getZoneUnits(zoneUnits);
        Float units = o3d::min<Float>(zoneUnits[X], zoneUnits[Y]);

        Vector2ui zoneSize;
        m_pZoneManager->getBaseZoneSize(zoneSize);
        UInt32 minSize = o3d::min<UInt32>(zoneSize[X], zoneSize[Y]) - 1;

        UInt32 maxUnits = UInt32(maxDistance/units);
        UInt32 pointCount = maxUnits / minSize + ((maxUnits % minSize) == 0 ? 0 : 1);

        m_colormapLodCurve.resize(pointCount);

        String pointList;
        for (UInt32 k = 0 ; k < pointCount ; ++k)
        {
            getTerrain()->getCurrentConfigs().getColormapPoint(
                    Float(k * minSize),
                    m_colormapLodCurve[k]);

            pointList << "(" << k << String(":") << Int32(m_colormapLodCurve[k]) << ") ";
        }

        PCLOD_MESSAGE(String("TextureManager : Colormap Lod Curve : ") << pointList);
    }
    else if (getTerrain()->getCurrentConfigs().colormapLod())
        PCLOD_WARNING("TextureManager : Colormap LOD used but no curve is defined");

    // Si le LOD des colormaps est activés
    if (getTerrain()->getCurrentConfigs().lightmapLod() &&
            (getTerrain()->getCurrentConfigs().lightmapCurvePointCount() > 0))
    {
        // La courbe contenu par la config est une courbe fonction de la distance.
        Float maxDistance = 0.0;
        Int8 maxValue = 0;

        getTerrain()->getCurrentConfigs().getLightmapPoint(
                getTerrain()->getCurrentConfigs().lightmapCurvePointCount()-1,
                maxDistance,
                maxValue);

        Vector2f zoneUnits;
        m_pZoneManager->getZoneUnits(zoneUnits);
        Float units = o3d::min<Float>(zoneUnits[X], zoneUnits[Y]);

        Vector2ui zoneSize;
        m_pZoneManager->getBaseZoneSize(zoneSize);
        UInt32 minSize = o3d::min<UInt32>(zoneSize[X], zoneSize[Y]) - 1;

        UInt32 maxUnits = UInt32(maxDistance/units);
        UInt32 pointCount = maxUnits / minSize + ((maxUnits % minSize) == 0 ? 0 : 1);

        m_lightmapLodCurve.resize(pointCount);

        String pointList;
        for (UInt32 k = 0 ; k < pointCount ; ++k)
        {
            getTerrain()->getCurrentConfigs().getLightmapPoint(
                    Float(k * minSize),
                    m_lightmapLodCurve[k]);

            pointList << "(" << k << String(":") << Int32(m_lightmapLodCurve[k]) << ") ";
        }

        PCLOD_MESSAGE(String("TextureManager : Lightmap Lod Curve : ") << pointList);
    }
    else if (getTerrain()->getCurrentConfigs().lightmapLod())
    {
        PCLOD_WARNING("TextureManager : Lightmap LOD used but no curve is defined");
    }

    if (getTerrain()->getCurrentConfigs().debugLabelEnabled())
    {
        m_text = getTerrain()->getCurrentConfigs().getText2D();

        m_text->setTextHeight(12);
        m_pFrameBuffer->create(128, 64, PF_RGB_U8);
    }
}

/* Manually add a material */
void PCLODTextureManager::registerMaterial(
        UInt32 _matId,
        const String _filePath,
        UInt32 _position)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    if (m_materialMap.find(_matId) != m_materialMap.end())
    {
        PCLOD_WARNING(String("TextureManager : Attempt to add a material which is already defined : ") <<
                _matId << " Path = " << _filePath);
    }
    else if (_matId == 0)
    {
        PCLODMaterial * pNewMat = new PCLODMaterial(
                (BaseObject*)this,
                _matId,
                String(""),
                _position);

        m_materialMap[_matId] = pNewMat;
        //listen(pNewMat);

        pNewMat->onTextureUnused.connect(this, &PCLODTextureManager::rteOnMaterialUnused, EvtHandler::CONNECTION_ASYNCH);
    }
    else
    {
        String fullPath = _filePath;
        AutoPtr<InStream> is(FileManager::instance()->openInStream(fullPath));

        if (!is)
        {
            PCLOD_ERROR(String(
                        "TextureManager : Attempt to add a material. Invalid path <") <<_filePath << ">");
        }

        // The material is added, but it will be a black texture
        PCLODMaterial * newMat = new PCLODMaterial(
                (BaseObject*)this,
                _matId,
                fullPath,
                _position);

        m_materialMap[_matId] = newMat;
        //listen(newMat);

        newMat->onTextureUnused.connect(this, &PCLODTextureManager::rteOnMaterialUnused, EvtHandler::CONNECTION_ASYNCH);

        if (is)
        {
            PCLOD_MESSAGE(String("TextureManager : Material : ") << " Id = " <<
                    _matId << " Path = " << fullPath << " Added");
        }
    }
}

// Add a new light
void PCLODTextureManager::mtAddLight(const TerrainBase::LightInfos & _infos)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    for (IT_LightArray itLight = m_lightArray.begin() ; itLight != m_lightArray.end() ; itLight++)
    {
        if ((*itLight)->getLight() == _infos.pLight)
            O3D_ERROR(E_InvalidOperation("Attempt to add the light that was already inserted in the terrain"));
    }

    PCLODLight * lpLightContainer = new PCLODLight((BaseObject*)this, _infos);

    m_lightArray.push_back(lpLightContainer);

    for (IT_LightmapMap it = m_lightmapMap.begin() ; it != m_lightmapMap.end() ; it++)
    {
        if (lpLightContainer->mtAffectLightmap(it->second))
            lpLightContainer->mtAddLightmap(it->second);
    }
}

// Remove a light
void PCLODTextureManager::mtRemoveLight(Light * _pLight)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    for (IT_LightArray itLight = m_lightArray.begin() ; itLight != m_lightArray.end() ; itLight++)
    {
        if ((*itLight)->getLight() == _pLight)
        {
            deletePtr(*itLight);
            m_lightArray.erase(itLight);
            return;
        }
    }

    O3D_ERROR(E_InvalidOperation("Attempt to remove a light that does not exist in the terrain"));
}

// Update a light
void PCLODTextureManager::mtUpdateLight(Light * _pLight)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

    for (IT_LightArray itLight = m_lightArray.begin() ; itLight != m_lightArray.end() ; itLight++)
    {
        if ((*itLight)->getLight() == _pLight)
        {
            (*itLight)->invalidate();
            return;
        }
    }

    O3D_ERROR(E_InvalidOperation("Attempt to remove a light that does not exist in the terrain"));
}

// Load and release all used materials and colormap
void PCLODTextureManager::rtLoadZone(PCLODTopZone * _pZone)
{
    O3D_ASSERT(_pZone != nullptr);

    // First, we load all materials used by this zone
    T_MaterialSet matSet;
    _pZone->getMaterialSet(matSet);

    // On construit la liste des matériaux à part pour éviter de bloquer
    // les mutex trop longtemps.
    std::vector<PCLODMaterial*> matArray(matSet.size());

    {
        ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

        UInt32 k = 0;

        O3D_ASSERT(m_materialMap.find(0) != m_materialMap.end());

        for (IT_MaterialSet it = matSet.begin() ; it != matSet.end() ; it++, ++k)
        {
            IT_MaterialMap itMat = m_materialMap.find(*it);

            if (itMat == m_materialMap.end())
            {
                PCLOD_ERROR(String("TextureManager : Texture referenced but not declared by the terrain file. Id = ") <<
                        *it << String(" Black texture will be used"));

                matArray[k] = m_materialMap[0];
            }
            else
                matArray[k] = itMat->second;
        }
    }

    // We create a label to display debug informations
    PCLODDebugLabel * lpLabel = nullptr;

    if (getTerrain()->getCurrentConfigs().debugLabelEnabled())
    {
        Vector3 lLabelPosition = _pZone->getWorldCenter();
        SmartArrayFloat lHeightmap;
        Vector2ui lHeightmapSize;

        _pZone->getHeightmap(lHeightmap, lHeightmapSize);
        lLabelPosition[Y] = 4.0f + lHeightmap[(lHeightmapSize[Y]/2+1)*lHeightmapSize[X] + (lHeightmapSize[X]/2+1)];

        O3D_ASSERT(m_labelMap.find(_pZone->getId()) == m_labelMap.end());
        lpLabel = new PCLODDebugLabel(this);
        lpLabel->setPosition(lLabelPosition);

        ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

        m_labelMap[_pZone->getId()] = lpLabel;
    }

    for (std::vector<PCLODMaterial*>::iterator it = matArray.begin() ; it != matArray.end() ; it++)
    {
        // This function will do nothing if the material is already loaded
        // But this function must be used to check if the material is loaded
        // Un simple test isLoaded() ne permet pas de savoir assurément si la texture
        // n'est pas justement en train d'etre détruite.

        // Cette fonction incrémente le compteur de référence du matériau
        rtLoadMaterial(*it);
    }

    // Now we load the colormap
    // De meme cette fonction permet d'éviter les conflits entre Load/Release
    // Cette fonction augmente le compteur de référence
    rtLoadColormap(_pZone);

    // And finally, we load the lightmap
    rtLoadLightmap(_pZone);

    // We throw the event
    onZoneLoaded(lpLabel);
}

void PCLODTextureManager::rtReleaseZone(PCLODTopZone * _pZone)
{
    O3D_ASSERT(_pZone != nullptr);

    // First we unload all materials
    T_MaterialSet matSet;
    _pZone->getMaterialSet(matSet);

    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::WRITE_LOCKER);

    for (IT_MaterialSet it = matSet.begin() ; it != matSet.end() ; it++)
    {
        IT_MaterialMap itMat = m_materialMap.find(*it);
        O3D_ASSERT(itMat != m_materialMap.end());

        itMat->second->releaseIt();

        // La libération des matériaux est automatique et gérer par évenements;
        // Si le compteur atteint 0, il sera traité dans la boucle update gui
    }

    // Next the colormap
    IT_ColormapMap itColormap = m_colormapMap.find(_pZone->getId());
    O3D_ASSERT(itColormap != m_colormapMap.end());

    itColormap->second->releaseIt();

    // Next the lightmap
    IT_LightmapMap itLightmap = m_lightmapMap.find(_pZone->getId());
    O3D_ASSERT(itLightmap != m_lightmapMap.end());

    itLightmap->second->releaseIt();
    // De meme si c'était la dernière instance, un événement a été levé et sera traité a l'update gui

    // On enleve le label si il existe
    IT_LabelMap itLabel = m_labelMap.find(_pZone->getId());
    PCLODDebugLabel * lpLabel = (itLabel != m_labelMap.end() ? itLabel->second : nullptr);

    if (itLabel != m_labelMap.end())
        m_labelMap.erase(itLabel);

    onZoneRelease(lpLabel);
}

/* Return a material */
PCLODMaterial * PCLODTextureManager::rtGetMaterial(UInt32 _matId)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

    IT_MaterialMap it = m_materialMap.find(_matId);
    O3D_ASSERT(it != m_materialMap.end());

    return it->second;
}

PCLODColormap * PCLODTextureManager::rtGetColormap(UInt32 _zoneId)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

    IT_ColormapMap it = m_colormapMap.find(_zoneId);
    O3D_ASSERT(it != m_colormapMap.end());

    return it->second;
}

PCLODLightmap * PCLODTextureManager::rtGetLightmap(UInt32 _zoneId)
{
    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

    IT_LightmapMap it = m_lightmapMap.find(_zoneId);
    O3D_ASSERT(it != m_lightmapMap.end());

    return it->second;
}

/* Events */

void PCLODTextureManager::rteOnColormapUnused()
{
    PCLODColormap * lColormap = static_cast<PCLODColormap*>(getSender());
    O3D_ASSERT(lColormap != nullptr);

    PCLOD_MESSAGE(String("TextureManager : Event textureUnused : Colormap ") << lColormap->getZoneId());

    rtRemoveColormap(lColormap->getZoneId());
}

void PCLODTextureManager::rteOnLightmapUnused()
{
    PCLODLightmap * lLightmap = static_cast<PCLODLightmap*>(getSender());
    O3D_ASSERT(lLightmap != nullptr);

    PCLOD_MESSAGE(String("TextureManager : Event textureUnused : Lightmap") << lLightmap->getZoneId());

    rtRemoveLightmap(lLightmap->getZoneId());
}

void PCLODTextureManager::rteOnMaterialUnused()
{
    PCLODMaterial * lMaterial = static_cast<PCLODMaterial*>(getSender());
    O3D_ASSERT(lMaterial != nullptr);

    PCLOD_MESSAGE(String("TextureManager : Event textureUnused : Material ") << lMaterial->getMatId());

    rtUnloadMaterial(lMaterial->getMatId());
}

/* Event thrown when the quadtree changes due to the camera movement */
void PCLODTextureManager::rteOnQuadTreeMoved()
{
    // Si aucune fonction de lod n'est définie, c'est que l'option n'est pas activée
    if ((m_lightmapLodCurve.size() == 0) || !getTerrain()->getCurrentConfigs().lightningEnabled())
        return;

    // Pour chaque lightmap, on détermine la distance à la position actuelle en nombre de zone.
    // On utilise la courbe définissant le lod des lightmaps pour déterminer quel niveau de lod va utiliser
    // la lightmap
    Vector2ui currentPosition;
    m_pZoneManager->getCurrentPosition(currentPosition);

    ReadWriteLocker lLocker(m_semaphore, ReadWriteLocker::READ_LOCKER);

    for (IT_LightmapMap itLightmap = m_lightmapMap.begin() ; itLightmap != m_lightmapMap.end() ; itLightmap++)
    {
        Vector2ui zonePosition = itLightmap->second->getZone()->getZoneIndex();
        Vector2i delta(	Int32(currentPosition[X]) - Int32(zonePosition[X]),
                Int32(currentPosition[Y]) - Int32(zonePosition[Y]));

        UInt32 distance = delta.normInf();

        itLightmap->second->setLod(m_lightmapLodCurve.size() > distance ?
                m_lightmapLodCurve[distance] : m_lightmapLodCurve.back());
    }
}

/* Event thrown when a texture (colormap/materials) was destroyed, and
 * need to release the GL texture Id */
void PCLODTextureManager::rteOnLightmapDeletion(PCLODLightmap* _pLightmap)
{
    PCLOD_MESSAGE(String("TextureManager : Event OnLightmapDeletion : Lightmap <") <<
            _pLightmap->getZoneId() << "> destroyed");

    _pLightmap->mtUnload();
    delete _pLightmap;
}

void PCLODTextureManager::rteOnColormapDeletion(PCLODColormap* _pColormap)
{
    PCLOD_MESSAGE(String("TextureManager : Event OnColormapDeletion : Colormap <") <<
            _pColormap->getZoneId() << "> destroyed");

    _pColormap->mtUnload();
    delete _pColormap;
}

void PCLODTextureManager::rteOnMaterialDeletion(Texture2D* _pMaterialTex)
{
    // Les matériaux ne sont jamais supprimés totalement. On libère juste éventuellement
    // les ressources graphiques.
    PCLOD_MESSAGE(String("TextureManager : Event OnMaterialDeletion : Material texture <") <<
            _pMaterialTex->getTextureId() << "> destroyed");

    delete _pMaterialTex;
}

void PCLODTextureManager::rteOnZoneLoaded(PCLODDebugLabel* _pLabel)
{
    if (_pLabel != nullptr)
    {
        Node *lNode = getScene()->getHierarchyTree()->addNode(_pLabel);
        O3D_ASSERT(lNode != nullptr);

        // add a local transform
        MTransform *transform = new MTransform;
        lNode->addTransform(transform);

        // initialize the position
        transform->setPosition(_pLabel->getPosition());

        // @todo luhtor the SetPosition might modify the transform, right ?
    }
}

void PCLODTextureManager::rteOnZoneRelease(PCLODDebugLabel* _pLabel)
{
    if (_pLabel != nullptr)
    {
        BaseNode *lNode = _pLabel->getNode();
        O3D_ASSERT(lNode != nullptr);

        deletePtr(lNode);
    }
}

