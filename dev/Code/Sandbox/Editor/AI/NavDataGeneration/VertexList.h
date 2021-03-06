/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#ifndef CRYINCLUDE_EDITOR_AI_NAVDATAGENERATION_VERTEXLIST_H
#define CRYINCLUDE_EDITOR_AI_NAVDATAGENERATION_VERTEXLIST_H
#pragma once


#include "IAgent.h"
#include "GraphStructures.h"
#include "HashSpace.h"

class CCryFile;


class CVertexList
{
public:
    CVertexList();
    ~CVertexList();
    int AddVertex(const ObstacleData& od);

    const ObstacleData& GetVertex(int index) const;
    ObstacleData& ModifyVertex(int index);
    int FindVertex(const ObstacleData& od) const;

    bool IsIndexValid(int index) const {return index >= 0 && index < m_obstacles.size(); }

    void WriteToFile(const char* fileName);
    bool ReadFromFile(const char* fileName);

    void Clear() {m_obstacles.clear(); }
    void Reset();
    int GetSize() {return m_obstacles.size(); }
    int GetCapacity() {return m_obstacles.capacity(); }

    void GetVerticesInRange(std::vector<std::pair<float, unsigned> >& vertsOut, const Vec3& pos, float range, unsigned char flags);

private:

    struct SVertexRecord
    {
        inline SVertexRecord() {}
        inline SVertexRecord(unsigned vertIndex)
            : vertIndex(vertIndex) {}
        inline const Vec3& GetPos(CVertexList& vertList) const { return vertList.GetVertex(vertIndex).vPos; }
        inline bool operator==(const SVertexRecord& rhs) const { return rhs.vertIndex == vertIndex; }
        unsigned vertIndex;
    };

    class VertexHashSpaceTraits
    {
    public:
        inline VertexHashSpaceTraits(CVertexList& vertexList)
            : vertexList(vertexList) {}
        inline const Vec3& operator() (const SVertexRecord& item) const {return item.GetPos(vertexList); }
        CVertexList& vertexList;
    };

    struct SVertCollector
    {
        SVertCollector(CVertexList& vertList, std::vector<std::pair<float, unsigned> >& verts, unsigned char flags)
            : vertList(vertList)
            , verts(verts)
            , flags(flags) {}

        void operator()(const SVertexRecord& record, float distSq)
        {
            if (vertList.GetVertex(record.vertIndex).flags & flags)
            {
                verts.push_back(std::make_pair(distSq, record.vertIndex));
            }
        }

        CVertexList& vertList;
        unsigned char flags;
        std::vector<std::pair<float, unsigned> >& verts;
    };


    /// Our spatial structure
    CHashSpace<SVertexRecord, VertexHashSpaceTraits>* m_hashSpace;

    Obstacles m_obstacles;
};

#endif // CRYINCLUDE_EDITOR_AI_NAVDATAGENERATION_VERTEXLIST_H
