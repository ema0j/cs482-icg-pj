#include "element.h"

#include <misc/stdcommon.h>

carray<Vec3i> ElementOperations::QuadsToTriangles(const carray<Vec4i>& quads) {
    carray<Vec3i> triangles(quads.size()*2);
    for(uint64_t f = 0; f < quads.size(); f ++) {
        triangles[f*2+0] = Vec3i(quads[f][0],quads[f][1],quads[f][2]);
        triangles[f*2+1] = Vec3i(quads[f][0],quads[f][2],quads[f][3]);
    }
    return triangles;
}

carray<Vec3f> ElementOperations::TriangleNormals(const carray<Vec3f>& pos, const carray<Vec3i>& triangles) {
    carray<Vec3f> normals(triangles.size());
    for(uint64_t f = 0; f < triangles.size(); f ++) normals[f] = TriangleNormal(pos[triangles[f][0]],pos[triangles[f][1]],pos[triangles[f][2]]);
    return normals;
}

carray<Vec3f> ElementOperations::QuadNormals(const carray<Vec3f>& pos, const carray<Vec4i>& quads) {
    carray<Vec3f> normals(quads.size());
    for(uint64_t f = 0; f < quads.size(); f ++) normals[f] = QuadNormal(pos[quads[f][0]],pos[quads[f][1]],pos[quads[f][2]],pos[quads[f][3]]);
    return normals;
}

carray<Vec3f> ElementOperations::SegmentTangents(const carray<Vec3f>& pos, const carray<Vec2i>& segments) {
    carray<Vec3f> tangents(segments.size());
    for(uint64_t f = 0; f < segments.size(); f ++) tangents[f] = SegmentTangent(pos[segments[f][0]],pos[segments[f][1]]);
    return tangents;
}

carray<Vec3f> ElementOperations::SmoothTriangleNormals(const carray<Vec3f> pos, const carray<Vec3i>& triangles) {
    carray<Vec3f> faceNormals = TriangleNormals(pos,triangles);
    carray<Vec3f> normals(pos.size(), Vec3f());
    carray<int> avgCount(pos.size(), 0);
    for(uint64_t f = 0; f < triangles.size(); f ++) {
        for(int v = 0; v < 3; v ++) {
            uint64_t idx = triangles[f][v];
            normals[idx] += faceNormals[f];
            avgCount[idx] ++;
        }
    }
    for(uint64_t v = 0; v < normals.size(); v ++) {
        normals[v] /= (float)avgCount[v];
    }
    return normals;
}

//*** HACK to fix that vector classes use a less-than operator that does not produce a strict weak ordering
struct lessThanVec3i{
	bool operator()(const Vec3i s1, const Vec3i s2) const{
		if(s1.x < s2.x) return true;
		if(s1.x == s2.x){
			if(s1.y < s2.y) return true;
			if(s1.y == s2.y){
				if(s1.z < s2.z) return true;
			}
		}
		return false;
	}
};


void ElementOperations::ShareTriangleVertices(
                           const carray<Vec3f>& pos, const carray<Vec3f>& normal, 
                           const carray<Vec2f>& uv, const carray<Vec3i>& vertices,
                           carray<Vec3f>& trianglePos, carray<Vec3f>& triangleNormal, 
                           carray<Vec2f>& triangleUv, carray<Vec3i>& triangles) {
	//uses a hacked less-than operator to fix the problem mentioned in the above lessThanVec3i struct definition
	map<Vec3i, int, lessThanVec3i> v2t;

   vector<Vec3f> trianglePosV; vector<Vec3f> triangleNormalV;
   vector<Vec2f> triangleUvV; vector<Vec3i> trianglesV;

   // create vertices
   for(uint64_t v = 0; v < vertices.size(); v ++) {
       Vec3i v_idx = vertices[v];
       if(v2t.find(v_idx) == v2t.end()) {
            // make a new vertex
            uint64_t t_idx = trianglePosV.size();
            trianglePosV.push_back(pos[v_idx.x]);
            triangleNormalV.push_back(normal[v_idx.y]);
            triangleUvV.push_back(uv[v_idx.z]);
            v2t[v_idx] = static_cast<int>(t_idx);
	   }
   }

   // create faces
   for(uint64_t v = 0; v < vertices.size(); v += 3) {
       trianglesV.push_back(Vec3i(v2t[vertices[v+0]],v2t[vertices[v+1]],v2t[vertices[v+2]]));
   }

   // convert back
   trianglePos = carray<Vec3f>(trianglePosV);
   triangleNormal = carray<Vec3f>(triangleNormalV);
   triangleUv = carray<Vec2f>(triangleUvV);
   triangles = carray<Vec3i>(trianglesV);
}

//*** HACK to fix that vector classes use a less-than operator that does not produce a strict weak ordering
struct lessThanVec2i{
	bool operator()(const Vec2i s1, const Vec2i s2) const{
		if(s1.x < s2.x) return true;
		if(s1.x == s2.x){
			if(s1.y < s2.y) return true;
		}
		return false;
	}
};

void ElementOperations::ShareTriangleVertices(
                           const carray<Vec3f>& pos, const carray<Vec3f>& normal, const carray<Vec2i>& vertices,
                           carray<Vec3f>& trianglePos, carray<Vec3f>& triangleNormal, carray<Vec3i>& triangles) {
	//uses a hacked less-than operator to fix the problem mentioned in the above lessThanVec3i struct definition
	map<Vec2i, int, lessThanVec2i> v2t;

   vector<Vec3f> trianglePosV; 
   vector<Vec3f> triangleNormalV; 
   vector<Vec3i> trianglesV;

   // create vertices
   for(uint64_t v = 0; v < vertices.size(); v ++) {
       Vec2i v_idx = vertices[v];
       if(v2t.find(v_idx) == v2t.end()) {
            // make a new vertex
            uint64_t t_idx = trianglePosV.size();
            trianglePosV.push_back(pos[v_idx.x]);
            triangleNormalV.push_back(normal[v_idx.y]);
            v2t[v_idx] = static_cast<int>(t_idx);
	   }
   }

   // create faces
   for(uint64_t v = 0; v < vertices.size(); v += 3) {
       trianglesV.push_back(Vec3i(v2t[vertices[v+0]],v2t[vertices[v+1]],v2t[vertices[v+2]]));
   }

   // convert back
   trianglePos = carray<Vec3f>(trianglePosV);
   triangleNormal = carray<Vec3f>(triangleNormalV);
   triangles = carray<Vec3i>(trianglesV);
}

carray<Vec3f> ElementOperations::NormalizeVerticesToUnitCube(const carray<Vec3f>& pos) {
    return NormalizeVerticesToBox(pos,Range3f(Vec3f(-1,-1,-1),Vec3f(1,1,1)));
}

// this shuld be implemented using the one below
// left here since too close to deadline to change
// FIXME
carray<Vec3f> ElementOperations::NormalizeVerticesToBox(const carray<Vec3f>& pos, const Range3f& box) {
    carray<Vec3f> npos(pos);

    // compute the bbox
    Range3f bbox = VerticesBoundingBox(npos);

    // compute translation/scale
    Vec3f t = box.GetCenter() - bbox.GetCenter();
    float s = (box.GetSize() / bbox.GetSize()).MinComponent();

    // apply xform
    for(uint64_t i = 0; i < npos.size(); i ++) {
        npos[i] = (npos[i] + t) * s;
    }

    return npos;
}

carray<Vec3f> ElementOperations::NormalizeVerticesToBoxFromRef(const carray<Vec3f>& pos, const Range3f& box, const Range3f& ref) {
    carray<Vec3f> npos(pos);

    // compute translation/scale
    Vec3f t = box.GetCenter() - ref.GetCenter();
    float s = (box.GetSize() / ref.GetSize()).MinComponent();

    // apply xform
    for(uint64_t i = 0; i < npos.size(); i ++) {
        npos[i] = (npos[i] + t) * s;
    }

    return npos;
}

