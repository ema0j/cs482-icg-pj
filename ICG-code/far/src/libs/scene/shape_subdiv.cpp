#include "shape_subdiv.h"

/*
    PSEUDOCODE
    --------------------

    * split (create new topological faces)
        - foreach face F
            - face vertex (VF')
                - add new vertex to vertex list
            - edge vertices (VE'[1..4])
                - if vertex has been created
                    - retrive vertex
                - otherwise
                    - add new vertex to vertex list
            - vertex verticex (VV'[1..4])
                - if vertex vertices has been created
                    - retrive vertex
                - otherwise
                    - add new vertex to vertex list
            - add new face F' [VV1', EV1', FV', EV4']
            - add new face F' [fill in]
            - add new face F' [fill in]
            - add new face F' [fill in]
*/

struct SubdivisionFace;
struct SubdivisionEdge;
struct SubdivisionVertex;

struct dxVertexData {
    Vec3f pos;
    Vec2f uv;

    dxVertexData() { }
    dxVertexData(const Vec3f& p, const Vec2f& u) { pos = p; uv = u; }

    dxVertexData operator+(const dxVertexData& vd) { return dxVertexData(pos+vd.pos, uv+vd.uv); }
    dxVertexData operator*(float s) { return dxVertexData(pos*s, uv*s); }
};

struct SubdivisionEdge {
    SubdivisionEdge() { }
};

struct SubdivisionVertex {
    SubdivisionVertex(const dxVertexData& d) : vd(d) { }

    dxVertexData vd;

    vector<SubdivisionFace*> faces;
};

// implicit vertex edge relation
#define N 4
struct SubdivisionFace {
    SubdivisionVertex* vertices[N];
    SubdivisionEdge* edges[N];

    SubdivisionFace() { for(int i = 0; i < N; i ++) { vertices[i] = 0; edges[i] = 0; } }

    SubdivisionEdge* getEdge(SubdivisionVertex* v0, SubdivisionVertex* v1) {
        // find v0
        int v0Idx = -1;
        for(int i = 0; i < N; i ++) {
            if(vertices[i] == v0) { v0Idx = i; break; }
        }
        if(v0Idx < 0) return 0;

        // check if v1 is successor
        if(vertices[(v0Idx+1)%N] == v1) return edges[v0Idx];
        
        // not found
        return 0;
    }

    bool adjacent(SubdivisionEdge* edge) {
        for(int e = 0; e < N; e ++) if(edges[e] == edge) return true;
        return false;
    }

    SubdivisionFace* adjacentFace(SubdivisionEdge* edge) {
        int edgeIdx = 0;
        for(edgeIdx = 0; edgeIdx < N; edgeIdx ++) if(edges[edgeIdx] == edge) break;
        for(size_t f = 0; f < vertices[edgeIdx]->faces.size(); f ++) {
            if(vertices[edgeIdx]->faces[f]->adjacent(edges[edgeIdx]) && vertices[edgeIdx]->faces[f] != this) return vertices[edgeIdx]->faces[f];
        }
        return 0;
    }

    int edgeIdx(SubdivisionEdge* edge) {
        for(int e = 0; e < N; e ++) if(edges[e] == edge) return e;
        return -1;
    }
};

struct SubdivisionMesh {
    shared_ptr<SubdivisionMesh> previous;
    shared_ptr<SubdivisionMesh> next;

    vector<shared_ptr<SubdivisionFace> > faces;
    vector<shared_ptr<SubdivisionEdge> > edges;
    vector<shared_ptr<SubdivisionVertex> > vertices;

    SubdivisionMesh() { }
    SubdivisionMesh(const carray<Vec3f>& pos, const carray<Vec2f>& uv, const carray<Vec4i> face);

    SubdivisionFace* addFace(SubdivisionVertex* v0, SubdivisionVertex* v1, SubdivisionVertex* v2, SubdivisionVertex* v3) {
        SubdivisionVertex* v[4] = { v0, v1, v2, v3 };
        return addFace(v);
    }
    SubdivisionFace* addFace(SubdivisionVertex* vertex[N]);
    SubdivisionVertex* addVertex(const dxVertexData& d);

    void toTesselation(shared_ptr<TesselationCache> cache);

    shared_ptr<SubdivisionMesh> subdivide();
};

SubdivisionVertex* SubdivisionMesh::addVertex(const dxVertexData& d) {
    shared_ptr<SubdivisionVertex> newVertex = shared_ptr<SubdivisionVertex>(new SubdivisionVertex(d));
    vertices.push_back(newVertex); 
    return newVertex.get(); 
}

SubdivisionFace* SubdivisionMesh::addFace(SubdivisionVertex* faceVertices[N]) {
    // add face
    shared_ptr<SubdivisionFace> newFace = shared_ptr<SubdivisionFace>(new SubdivisionFace());
    faces.push_back(newFace);
    SubdivisionFace* face = newFace.get();

    // link up vertices
    for(int v = 0; v < N; v ++) {
        face->vertices[v] = faceVertices[v];
        faceVertices[v]->faces.push_back(face);
    }

    // link up edges
    for(int v = 0; v < N; v ++) {
        SubdivisionVertex* v0 = faceVertices[v];
        SubdivisionVertex* v1 = faceVertices[(v+1)%N];
        // check if any other face is adjacent to this one
        SubdivisionEdge* edge = 0;
        for(size_t f = 0; f < v1->faces.size(); f ++) {
            edge = v1->faces[f]->getEdge(v1, v0);
            if(edge) break;
        }
        if(!edge) {
            // not found, create one
            shared_ptr<SubdivisionEdge> newEdge = shared_ptr<SubdivisionEdge>(new SubdivisionEdge());
            edges.push_back(newEdge);
            edge = newEdge.get();
        }
        face->edges[v] = edge;
    }

    return face;
}

SubdivisionMesh::SubdivisionMesh(const carray<Vec3f>& pos, const carray<Vec2f>& uv, const carray<Vec4i> face) {
    for(uint32_t v = 0; v < pos.size(); v ++) {
        if(!uv.empty()) addVertex(dxVertexData(pos[v], uv[v]));
        else addVertex(dxVertexData(pos[v], Vec2f(0,0)));
    }
    for(uint32_t f = 0; f < face.size(); f ++) {
        addFace(vertices[face[f][0]].get(), vertices[face[f][1]].get(), vertices[face[f][2]].get(), vertices[face[f][3]].get());
    }
}

shared_ptr<SubdivisionMesh> SubdivisionMesh::subdivide() {
    shared_ptr<SubdivisionMesh> subdiv = shared_ptr<SubdivisionMesh>(new SubdivisionMesh());
    
    // for each face
    map<SubdivisionEdge*, SubdivisionVertex*> edgeVertices;
    map<SubdivisionVertex*, SubdivisionVertex*> vertexVertices;
    for(size_t f = 0; f < faces.size(); f ++) {
        SubdivisionFace* face = faces[f].get();

        // face vertex (average over 4 vertices)
        dxVertexData faceVertexData;
        for(int v = 0; v < N; v ++) {
            faceVertexData = faceVertexData + face->vertices[v]->vd * 0.25;
        }
        SubdivisionVertex* faceVertex = subdiv->addVertex(faceVertexData);

        // edge vertex
        SubdivisionVertex* edgeVertex[N];
        for(int e = 0; e < N; e ++) {
            // get the edge
            SubdivisionEdge* edge = face->edges[e];
            // check if vertex exists
            if(edgeVertices.find(edge) == edgeVertices.end()) {
                // compute value using adjacency
                dxVertexData edgeVertexData;
                // grab vertices on this edge
                SubdivisionVertex* ve0 = face->vertices[e];
                SubdivisionVertex* ve1 = face->vertices[(e+1)%N];
                // grad adjacent face if exists
                SubdivisionFace* fe1 = face->adjacentFace(edge);
                if(fe1) {
                    SubdivisionFace* fe0 = face;
                    SubdivisionVertex* vf0 = fe0->vertices[(e+2)%N];
                    SubdivisionVertex* vf1 = fe0->vertices[(e+3)%N];
                    int e1 = fe1->edgeIdx(edge);
                    SubdivisionVertex* vf2 = fe1->vertices[(e1+2)%N];
                    SubdivisionVertex* vf3 = fe1->vertices[(e1+3)%N];
                    edgeVertices[edge] = subdiv->addVertex((ve0->vd + ve1->vd) * (3.0f/8.0f) +
                        (vf0->vd + vf1->vd + vf2->vd + vf3->vd) * (1.0f/16.0f));
                } else {
                    edgeVertices[edge] = subdiv->addVertex((ve0->vd + ve1->vd) * 0.5f);
                }
            }
            edgeVertex[e] = edgeVertices[edge];
        }

        // vertex vertex
        SubdivisionVertex* vertexVertex[N];
        for(int v = 0; v < N; v ++) {
            // get the vertex
            SubdivisionVertex* vertex = face->vertices[v];
            // check if the vertex exists
            if(vertexVertices.find(vertex) == vertexVertices.end()) {
                // check if boundary
                bool boundary = false;
                for(size_t f = 0; f < vertex->faces.size(); f ++) {
                    // get vertex index
                    int vIdx = 0;
                    for(vIdx = 0; vIdx < N; vIdx ++) if(vertex->faces[f]->vertices[vIdx] == vertex) break;
                    // if the corresponding edge s open, then there is an open boundary
                    if(!vertex->faces[f]->adjacentFace(vertex->faces[f]->edges[vIdx]) || 
                       !vertex->faces[f]->adjacentFace(vertex->faces[f]->edges[(vIdx+N-1)%N])) { 
                       boundary = true; 
                       break; 
                    }
                }
                if(!boundary) {
                    dxVertexData vdBeta, vdGamma;
                    int k = 0;
                    for(size_t f = 0; f < vertex->faces.size(); f ++) {
                        // get vertex index
                        int vIdx = 0;
                        for(vIdx = 0; vIdx < N; vIdx ++) if(vertex->faces[f]->vertices[vIdx] == vertex) break;
                        // accumulate other vertices (note that first ring is counted twice
                        vdBeta = vdBeta + vertex->faces[f]->vertices[(vIdx+1)%N]->vd; 
                        vdGamma = vdGamma + vertex->faces[f]->vertices[(vIdx+2)%N]->vd;
                        k ++;
                    }
                    float beta = 3.0f / (2.0f * k);
                    float gamma = 1.0f / (4.0f * k);
                    dxVertexData vd = vertex->vd * (1-beta-gamma) + vdBeta * (beta / k) + vdGamma * (gamma / k);
                    vertexVertices[vertex] = subdiv->addVertex(vd);
                } else {
                    dxVertexData vdOuter;
                    int k = 0;
                    for(size_t f = 0; f < vertex->faces.size(); f ++) {
                        // get vertex index
                        int vIdx = 0;
                        for(vIdx = 0; vIdx < N; vIdx ++) if(vertex->faces[f]->vertices[vIdx] == vertex) break;
                        // if the corresponding edge s open, then there is an open boundary
                        if(!vertex->faces[f]->adjacentFace(vertex->faces[f]->edges[vIdx])) {
                            vdOuter = vdOuter + vertex->faces[f]->vertices[(vIdx+1)%N]->vd;
                            k ++;
                        } 
                        if(!vertex->faces[f]->adjacentFace(vertex->faces[f]->edges[(vIdx+N-1)%N])) {
                            vdOuter = vdOuter + vertex->faces[f]->vertices[(vIdx+N-1)%N]->vd;
                            k ++;
                        }
                    }
                    assert(k == 2);
                    dxVertexData vd = vertex->vd * (3.0f/4.0f) + vdOuter * (1.0f/8.0f);
                    vertexVertices[vertex] = subdiv->addVertex(vd);
                }
            }
            vertexVertex[v] = vertexVertices[vertex];
        }

        // add 4 faces to the mesh
        for(int v = 0; v < 4; v ++) {
            subdiv->addFace(vertexVertex[v], edgeVertex[v], faceVertex, edgeVertex[(v+N-1)%N]);
        }
    }

    return subdiv;
}

void SubdivisionMesh::toTesselation(shared_ptr<TesselationCache> cache) {
    // vertex map
    map<SubdivisionVertex*, int> vertexIdMap;
    for(size_t v = 0; v < vertices.size(); v ++) {
        vertexIdMap[vertices[v].get()] = static_cast<int>(v);
    }
    
    // output quads
    carray<Vec3f> pos((uint32_t)vertices.size());
    carray<Vec2f> uv((uint32_t)vertices.size());
    carray<Vec4i> idx((uint32_t)faces.size());
    
    // vertices
    for(uint32_t v = 0; v < vertices.size(); v ++) {
        pos[v] = vertices[v]->vd.pos;
        uv[v] = vertices[v]->vd.uv;
    }
    
    // faces
    for(uint32_t f = 0; f < faces.size(); f ++) {
        idx[f] = Vec4i(vertexIdMap[faces[f]->vertices[0]],
                       vertexIdMap[faces[f]->vertices[1]],
                       vertexIdMap[faces[f]->vertices[2]],
                       vertexIdMap[faces[f]->vertices[3]]);
    }
    
    carray<Vec3f> normal;
    return cache->AddQuads(pos, normal, uv, idx);
}

Range3f CatmullClarkShape::ComputeBoundingBox(float time) {
    Range3f ret;
    for(uint32_t i = 0; i < posArray.size(); i ++) ret.Grow(posArray[i]);
    return ret;
}

void CatmullClarkShape::Tesselate(shared_ptr<TesselationCache> cache, float time) {
    // copy into tesselation data structure
    shared_ptr<SubdivisionMesh> subdiv = shared_ptr<SubdivisionMesh>(new SubdivisionMesh(posArray, uvArray, faceArray));

    // subdivide
    shared_ptr<SubdivisionMesh> current = subdiv;
    for(int i = 0; i < subdivision; i ++) {
        current = current->subdivide();
    }

    // convert to tesselation
    current->toTesselation(cache);
}

string CatmullClarkShape::serialize_typename() { return "CatmullClarkShape"; } 

void CatmullClarkShape::serialize(Archive* a) {
	a->member("pos", posArray);
	a->optional("normal", normalArray, carray<Vec3f>());
	a->optional("uv", uvArray, carray<Vec2f>());
	a->member("quads", faceArray);
    a->member("subdivision", subdivision);
}
