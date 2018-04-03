// This file is part of the AliceVision project.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include <aliceVision/mvsData/image.hpp>
#include <aliceVision/mvsData/Point2d.hpp>
#include <aliceVision/mvsData/Point3d.hpp>
#include <aliceVision/mvsData/StaticVector.hpp>
#include <aliceVision/mvsData/Voxel.hpp>
#include <aliceVision/mvsUtils/ImagesCache.hpp>
#include <aliceVision/mesh/Mesh.hpp>
#include <aliceVision/mesh/meshVisibility.hpp>

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace aliceVision {
namespace mesh {

/**
 * @brief Available mesh unwrapping methods
 */
enum class EUnwrapMethod {
    Basic = 0, // Basic unwrapping based on visibilities
    ABF = 1,   // Geogram: ABF++
    LSCM = 2   // Geogram: Spectral LSCM
};

/**
 * @brief returns the EUnwrapMethod enum from a string.
 * @param[in] method the input string.
 * @return the associated EUnwrapMethod enum.
 */
EUnwrapMethod EUnwrapMethod_stringToEnum(const std::string& method);

/**
 * @brief converts an EUnwrapMethod enum to a string.
 * @param[in] method the EUnwrapMethod enum to convert.
 * @return the string associated to the EUnwrapMethod enum.
 */
std::string EUnwrapMethod_enumToString(EUnwrapMethod method);


struct TexturingParams
{
    unsigned int textureSide = 8192;
    unsigned int padding = 15;
    unsigned int downscale = 2;
    bool fillHoles = false;
};

struct Texturing
{
    TexturingParams texParams;

    int nmtls = 0;
    StaticVector<int>* trisMtlIds = nullptr;
    StaticVector<Point2d>* uvCoords = nullptr;
    StaticVector<Voxel>* trisUvIds = nullptr;
    StaticVector<Point3d>* normals = nullptr;
    StaticVector<Voxel>* trisNormalsIds = nullptr;
    PointsVisibility* pointsVisibilities = nullptr;
    Mesh* me = nullptr;

    /// texture atlas to 3D triangle ids
    std::vector<std::vector<int>> _atlases;

    ~Texturing()
    {
        delete trisMtlIds;
        delete uvCoords;
        delete trisUvIds;
        delete normals;
        delete trisNormalsIds;
        delete pointsVisibilities;
        delete me;
    }

public:
    /// Load a mesh from a .obj file and initialize internal structures
    void loadFromOBJ(const std::string& filename, bool flipNormals=false);

    /**
     * @brief Load a mesh from a dense reconstruction.
     *
     * @param meshFilepath the path to the .bin mesh file
     * @param visibilitiesFilepath the path to the .bin points visibilities file
     */
    void loadFromMeshing(const std::string& meshFilepath, const std::string& visibilitiesFilepath);

    /**
     * @brief Replace inner mesh with the mesh loaded from 'otherMeshPath'
     *        and remap visibilities from the first to the second
     *
     * @param otherMeshPath the mesh to load
     * @param flipNormals whether to flip normals when loading the mesh
     */
    void replaceMesh(const std::string& otherMeshPath, bool flipNormals=false);

    /// Returns whether UV coordinates are available
    inline bool hasUVs() const { return uvCoords != nullptr && !uvCoords->empty(); }

    /**
     * @brief Unwrap mesh with the given 'method'.
     *
     * Requires internal mesh 'me' to be initialized.
     */
    void unwrap(mvsUtils::MultiViewParams& mp, EUnwrapMethod method);

    /**
     * @brief Generate automatic texture atlasing and UV coordinates based on points visibilities
     *
     * Requires internal mesh 'me' to be initialized.
     *
     * @param mp
     */
    void generateUVs(mvsUtils::MultiViewParams &mp);

    /// Generate texture files for all texture atlases
    void generateTextures(const mvsUtils::MultiViewParams& mp,
                          const bfs::path &outPath, EImageFileType textureFileType = EImageFileType::PNG);

    /// Generate texture files for the given texture atlas index
    void generateTexture(const mvsUtils::MultiViewParams& mp,
                         size_t atlasID, mvsUtils::ImagesCache& imageCache,
                         const bfs::path &outPath, EImageFileType textureFileType = EImageFileType::PNG);

    /// Save textured mesh as an OBJ + MTL file
    void saveAsOBJ(const bfs::path& dir, const std::string& basename, EImageFileType textureFileType = EImageFileType::PNG);
};

} // namespace mesh
} // namespace aliceVision
