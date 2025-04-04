/*
Open Asset Import Library (assimp)
----------------------------------------------------------------------

Copyright (c) 2006-2025, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above
copyright notice, this list of conditions and the
following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other
materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
contributors may be used to endorse or promote products
derived from this software without specific prior
written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/
/// \file   X3DImporter_Geometry2D.cpp
/// \brief  Parsing data from nodes of "Geometry2D" set of X3D.
/// date   2015-2016
/// author smal.root@gmail.com

#ifndef ASSIMP_BUILD_NO_X3D_IMPORTER

#include "X3DImporter.hpp"
#include "X3DImporter_Macro.hpp"
#include "X3DXmlHelper.h"
#include "X3DGeoHelper.h"

namespace Assimp {

// <Arc2D
// DEF=""              ID
// USE=""              IDREF
// endAngle="1.570796" SFFloat [initializeOnly]
// radius="1"          SFFloat [initializeOnly]
// startAngle="0"      SFFloat [initializeOnly]
// />
// The Arc2D node specifies a linear circular arc whose center is at (0,0) and whose angles are measured starting at the positive x-axis and sweeping
// towards the positive y-axis. The radius field specifies the radius of the circle of which the arc is a portion. The arc extends from the startAngle
// counterclockwise to the endAngle. The values of startAngle and endAngle shall be in the range [-2pi, 2pi] radians (or the equivalent if a different
// angle base unit has been specified). If startAngle and endAngle have the same value, a circle is specified.
void X3DImporter::readArc2D(XmlNode &node) {
    std::string def, use;
    float endAngle = AI_MATH_HALF_PI_F;
    float radius = 1;
    float startAngle = 0;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    XmlParser::getFloatAttribute(node, "endAngle", endAngle);
    XmlParser::getFloatAttribute(node, "radius", radius);
    XmlParser::getFloatAttribute(node, "startAngle", startAngle);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_Arc2D, ne);
    } else {
        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_Arc2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        // create point list of geometry object and convert it to line set.
        std::list<aiVector3D> tlist;

        X3DGeoHelper::make_arc2D(startAngle, endAngle, radius, 10, tlist); ///TODO: IME - AI_CONFIG for NumSeg
        X3DGeoHelper::extend_point_to_line(tlist, ((X3DNodeElementGeometry2D *)ne)->Vertices);
        ((X3DNodeElementGeometry2D *)ne)->NumIndices = 2;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "Arc2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <ArcClose2D
// DEF=""              ID
// USE=""              IDREF
// closureType="PIE"   SFString [initializeOnly], {"PIE", "CHORD"}
// endAngle="1.570796" SFFloat  [initializeOnly]
// radius="1"          SFFloat  [initializeOnly]
// solid="false"       SFBool   [initializeOnly]
// startAngle="0"      SFFloat  [initializeOnly]
// />
// The ArcClose node specifies a portion of a circle whose center is at (0,0) and whose angles are measured starting at the positive x-axis and sweeping
// towards the positive y-axis. The end points of the arc specified are connected as defined by the closureType field. The radius field specifies the radius
// of the circle of which the arc is a portion. The arc extends from the startAngle counterclockwise to the endAngle. The value of radius shall be greater
// than zero. The values of startAngle and endAngle shall be in the range [-2pi, 2pi] radians (or the equivalent if a different default angle base unit has
// been specified). If startAngle and endAngle have the same value, a circle is specified and closureType is ignored. If the absolute difference between
// startAngle and endAngle is greater than or equal to 2pi, a complete circle is produced with no chord or radial line(s) drawn from the center.
// A closureType of "PIE" connects the end point to the start point by defining two straight line segments first from the end point to the center and then
// the center to the start point. A closureType of "CHORD" connects the end point to the start point by defining a straight line segment from the end point
// to the start point. Textures are applied individually to each face of the ArcClose2D. On the front (+Z) and back (-Z) faces of the ArcClose2D, when
// viewed from the +Z-axis, the texture is mapped onto each face with the same orientation as if the image were displayed normally in 2D.
void X3DImporter::readArcClose2D(XmlNode &node) {
    std::string def, use;
    std::string closureType("PIE");
    float endAngle = AI_MATH_HALF_PI_F;
    float radius = 1;
    bool solid = false;
    float startAngle = 0;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    XmlParser::getStdStrAttribute(node, "closureType", closureType);
    XmlParser::getFloatAttribute(node, "endAngle", endAngle);
    XmlParser::getFloatAttribute(node, "endAngle", endAngle);
    XmlParser::getFloatAttribute(node, "radius", radius);
    XmlParser::getBoolAttribute(node, "solid", solid);
    XmlParser::getFloatAttribute(node, "startAngle", startAngle);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_ArcClose2D, ne);
    } else {
        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_ArcClose2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        ((X3DNodeElementGeometry2D *)ne)->Solid = solid;
        // create point list of geometry object.
        X3DGeoHelper::make_arc2D(startAngle, endAngle, radius, 10, ((X3DNodeElementGeometry2D *)ne)->Vertices); ///TODO: IME - AI_CONFIG for NumSeg
        // add chord or two radiuses only if not a circle was defined
        if (!((std::fabs(endAngle - startAngle) >= AI_MATH_TWO_PI_F) || (endAngle == startAngle))) {
            std::list<aiVector3D> &vlist = ((X3DNodeElementGeometry2D *)ne)->Vertices; // just short alias.

            if ((closureType == "PIE") || (closureType == "\"PIE\""))
                vlist.emplace_back(static_cast<ai_real>(0), static_cast<ai_real>(0), static_cast<ai_real>(0)); // center point - first radial line
            else if ((closureType != "CHORD") && (closureType != "\"CHORD\""))
                Throw_IncorrectAttrValue("ArcClose2D", "closureType");

            vlist.push_back(*vlist.begin()); // arc first point - chord from first to last point of arc(if CHORD) or second radial line(if PIE).
        }

        ((X3DNodeElementGeometry2D *)ne)->NumIndices = ((X3DNodeElementGeometry2D *)ne)->Vertices.size();
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "ArcClose2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <Circle2D
// DEF=""     ID
// USE=""     IDREF
// radius="1" SFFloat  [initializeOnly]
// />
void X3DImporter::readCircle2D(XmlNode &node) {
    std::string def, use;
    float radius = 1;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    XmlParser::getFloatAttribute(node, "radius", radius);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_Circle2D, ne);
    } else {
        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_Circle2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        // create point list of geometry object and convert it to line set.
        std::list<aiVector3D> tlist;

        X3DGeoHelper::make_arc2D(0, 0, radius, 10, tlist); ///TODO: IME - AI_CONFIG for NumSeg
        X3DGeoHelper::extend_point_to_line(tlist, ((X3DNodeElementGeometry2D *)ne)->Vertices);
        ((X3DNodeElementGeometry2D *)ne)->NumIndices = 2;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "Circle2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <Disk2D
// DEF=""          ID
// USE=""          IDREF
// innerRadius="0" SFFloat  [initializeOnly]
// outerRadius="1" SFFloat  [initializeOnly]
// solid="false"   SFBool   [initializeOnly]
// />
// The Disk2D node specifies a circular disk which is centred at (0, 0) in the local coordinate system. The outerRadius field specifies the radius of the
// outer dimension of the Disk2D. The innerRadius field specifies the inner dimension of the Disk2D. The value of outerRadius shall be greater than zero.
// The value of innerRadius shall be greater than or equal to zero and less than or equal to outerRadius. If innerRadius is zero, the Disk2D is completely
// filled. Otherwise, the area within the innerRadius forms a hole in the Disk2D. If innerRadius is equal to outerRadius, a solid circular line shall
// be drawn using the current line properties. Textures are applied individually to each face of the Disk2D. On the front (+Z) and back (-Z) faces of
// the Disk2D, when viewed from the +Z-axis, the texture is mapped onto each face with the same orientation as if the image were displayed normally in 2D.
void X3DImporter::readDisk2D(XmlNode &node) {
    std::string def, use;
    float innerRadius = 0;
    float outerRadius = 1;
    bool solid = false;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    XmlParser::getFloatAttribute(node, "innerRadius", innerRadius);
    XmlParser::getFloatAttribute(node, "outerRadius", outerRadius);
    XmlParser::getBoolAttribute(node, "solid", solid);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_Disk2D, ne);
    } else {
        std::list<aiVector3D> tlist_o, tlist_i;

        if (innerRadius > outerRadius) Throw_IncorrectAttrValue("Disk2D", "innerRadius");

        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_Disk2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        // create point list of geometry object.
        ///TODO: IME - AI_CONFIG for NumSeg
        X3DGeoHelper::make_arc2D(0, 0, outerRadius, 10, tlist_o); // outer circle
        if (innerRadius == 0.0f) { // make filled disk
            // in tlist_o we already have points of circle. just copy it and sign as polygon.
            ((X3DNodeElementGeometry2D *)ne)->Vertices = tlist_o;
            ((X3DNodeElementGeometry2D *)ne)->NumIndices = tlist_o.size();
        } else if (innerRadius == outerRadius) { // make circle
            // in tlist_o we already have points of circle. convert it to line set.
            X3DGeoHelper::extend_point_to_line(tlist_o, ((X3DNodeElementGeometry2D *)ne)->Vertices);
            ((X3DNodeElementGeometry2D *)ne)->NumIndices = 2;
        } else { // make disk
            std::list<aiVector3D> &vlist = ((X3DNodeElementGeometry2D *)ne)->Vertices; // just short alias.

            X3DGeoHelper::make_arc2D(0, 0, innerRadius, 10, tlist_i); // inner circle
            //
            // create quad list from two point lists
            //
            if (tlist_i.size() < 2) {
                // tlist_i and tlist_o has equal size.
                throw DeadlyImportError("Disk2D. Not enough points for creating quad list.");
            }

            // add all quads except last
            for (std::list<aiVector3D>::iterator it_i = tlist_i.begin(), it_o = tlist_o.begin(); it_i != tlist_i.end();) {
                // do not forget - CCW direction
                vlist.emplace_back(*it_i++); // 1st point
                vlist.emplace_back(*it_o++); // 2nd point
                vlist.emplace_back(*it_o); // 3rd point
                vlist.emplace_back(*it_i); // 4th point
            }

            // add last quad
            vlist.emplace_back(tlist_i.back()); // 1st point
            vlist.emplace_back(tlist_o.back()); // 2nd point
            vlist.emplace_back(tlist_o.front()); // 3rd point
            vlist.emplace_back(tlist_i.front()); // 4th point

            ((X3DNodeElementGeometry2D *)ne)->NumIndices = 4;
        }

        ((X3DNodeElementGeometry2D *)ne)->Solid = solid;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "Disk2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <Polyline2D
// DEF=""          ID
// USE=""          IDREF
// lineSegments="" MFVec2F [intializeOnly]
// />
void X3DImporter::readPolyline2D(XmlNode &node) {
    std::string def, use;
    std::list<aiVector2D> lineSegments;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    X3DXmlHelper::getVector2DListAttribute(node, "lineSegments", lineSegments);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_Polyline2D, ne);
    } else {
        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_Polyline2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        //
        // convert read point list of geometry object to line set.
        //
        std::list<aiVector3D> tlist;

        // convert vec2 to vec3
        for (std::list<aiVector2D>::iterator it2 = lineSegments.begin(); it2 != lineSegments.end(); ++it2)
            tlist.emplace_back(it2->x, it2->y, static_cast<ai_real>(0));

        // convert point set to line set
        X3DGeoHelper::extend_point_to_line(tlist, ((X3DNodeElementGeometry2D *)ne)->Vertices);
        ((X3DNodeElementGeometry2D *)ne)->NumIndices = 2;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "Polyline2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <Polypoint2D
// DEF=""   ID
// USE=""   IDREF
// point="" MFVec2F [inputOutput]
// />
void X3DImporter::readPolypoint2D(XmlNode &node) {
    std::string def, use;
    std::list<aiVector2D> point;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    X3DXmlHelper::getVector2DListAttribute(node, "point", point);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_Polypoint2D, ne);
    } else {
        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_Polypoint2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        // convert vec2 to vec3
        for (std::list<aiVector2D>::iterator it2 = point.begin(); it2 != point.end(); ++it2) {
            ((X3DNodeElementGeometry2D *)ne)->Vertices.emplace_back(it2->x, it2->y, static_cast<ai_real>(0));
        }

        ((X3DNodeElementGeometry2D *)ne)->NumIndices = 1;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "Polypoint2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <Rectangle2D
// DEF=""        ID
// USE=""        IDREF
// size="2 2"    SFVec2f [initializeOnly]
// solid="false" SFBool  [initializeOnly]
// />
void X3DImporter::readRectangle2D(XmlNode &node) {
    std::string def, use;
    aiVector2D size(2, 2);
    bool solid = false;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    X3DXmlHelper::getVector2DAttribute(node, "size", size);
    XmlParser::getBoolAttribute(node, "solid", solid);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_Rectangle2D, ne);
    } else {
        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_Rectangle2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        float x1 = -size.x / 2.0f;
        float x2 = size.x / 2.0f;
        float y1 = -size.y / 2.0f;
        float y2 = size.y / 2.0f;
        std::list<aiVector3D> &vlist = ((X3DNodeElementGeometry2D *)ne)->Vertices; // just short alias.

        vlist.emplace_back(x2, y1, static_cast<ai_real>(0)); // 1st point
        vlist.emplace_back(x2, y2, static_cast<ai_real>(0)); // 2nd point
        vlist.emplace_back(x1, y2, static_cast<ai_real>(0)); // 3rd point
        vlist.emplace_back(x1, y1, static_cast<ai_real>(0)); // 4th point
        ((X3DNodeElementGeometry2D *)ne)->Solid = solid;
        ((X3DNodeElementGeometry2D *)ne)->NumIndices = 4;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "Rectangle2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

// <TriangleSet2D
// DEF=""        ID
// USE=""        IDREF
// solid="false" SFBool  [initializeOnly]
// vertices=""   MFVec2F [inputOutput]
// />
void X3DImporter::readTriangleSet2D(XmlNode &node) {
    std::string def, use;
    bool solid = false;
    std::list<aiVector2D> vertices;
    X3DNodeElementBase *ne(nullptr);

    MACRO_ATTRREAD_CHECKUSEDEF_RET(node, def, use);
    X3DXmlHelper::getVector2DListAttribute(node, "vertices", vertices);
    XmlParser::getBoolAttribute(node, "solid", solid);

    // if "USE" defined then find already defined element.
    if (!use.empty()) {
        ne = MACRO_USE_CHECKANDAPPLY(node, def, use, ENET_TriangleSet2D, ne);
    } else {
        if (vertices.size() % 3) throw DeadlyImportError("TriangleSet2D. Not enough points for defining triangle.");

        // create and if needed - define new geometry object.
        ne = new X3DNodeElementGeometry2D(X3DElemType::ENET_TriangleSet2D, mNodeElementCur);
        if (!def.empty()) ne->ID = def;

        // convert vec2 to vec3
        for (std::list<aiVector2D>::iterator it2 = vertices.begin(); it2 != vertices.end(); ++it2) {
            ((X3DNodeElementGeometry2D *)ne)->Vertices.emplace_back(it2->x, it2->y, static_cast<ai_real>(0));
        }

        ((X3DNodeElementGeometry2D *)ne)->Solid = solid;
        ((X3DNodeElementGeometry2D *)ne)->NumIndices = 3;
        // check for X3DMetadataObject childs.
        if (!isNodeEmpty(node))
            childrenReadMetadata(node, ne, "TriangleSet2D");
        else
            mNodeElementCur->Children.push_back(ne); // add made object as child to current element

        NodeElement_List.push_back(ne); // add element to node element list because its a new object in graph
    } // if(!use.empty()) else
}

} // namespace Assimp

#endif // !ASSIMP_BUILD_NO_X3D_IMPORTER
