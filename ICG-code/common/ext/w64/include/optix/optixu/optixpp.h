
/*
 * Copyright (c) 2008 - 2009 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and proprietary
 * rights in and to this software, related documentation and any modifications thereto.
 * Any use, reproduction, disclosure or distribution of this software and related
 * documentation without an express license agreement from NVIDIA Corporation is strictly
 * prohibited.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS*
 * AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY
 * SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR
 * INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES
 */

#ifndef __optixu_optixpp_h__
#define __optixu_optixpp_h__

#include <optix.h>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include<windows.h>
#  include<optix_d3d9_interop.h>
#  include<optix_d3d10_interop.h>
#  include<optix_d3d11_interop.h>
#endif

#include <string>
#include <vector>
#include <optix_math.h>
#include <optix_gl_interop.h>

//-----------------------------------------------------------------------------
// 
// Helper functions
//
//-----------------------------------------------------------------------------

inline size_t getElementSize( RTformat fmt )
{
  int m_element_size = 0;

  switch( fmt ){
  case RT_FORMAT_FLOAT:
    m_element_size = sizeof(float);
    break;
  case RT_FORMAT_FLOAT2:
    m_element_size = sizeof(float)*2;
    break;
  case RT_FORMAT_FLOAT3:
    m_element_size = sizeof(float)*3;
    break;
  case RT_FORMAT_FLOAT4:
    m_element_size = sizeof(float)*4;
    break;
  case RT_FORMAT_BYTE:
    m_element_size = sizeof(char);
    break;
  case RT_FORMAT_BYTE2:
    m_element_size = sizeof(char)*2;
    break;
  case RT_FORMAT_BYTE3:
    m_element_size = sizeof(char)*3;
    break;
  case RT_FORMAT_BYTE4:
    m_element_size = sizeof(char)*4;
    break;
  case RT_FORMAT_UNSIGNED_BYTE:
    m_element_size = sizeof(unsigned char);
    break;
  case RT_FORMAT_UNSIGNED_BYTE2:
    m_element_size = sizeof(unsigned char)*2;
    break;
  case RT_FORMAT_UNSIGNED_BYTE3:
    m_element_size = sizeof(unsigned char)*3;
    break;
  case RT_FORMAT_UNSIGNED_BYTE4:
    m_element_size = sizeof(unsigned char)*4;
    break;
  case RT_FORMAT_SHORT:
    m_element_size = sizeof(short);
    break;
  case RT_FORMAT_SHORT2:
    m_element_size = sizeof(short)*2;
    break;
  case RT_FORMAT_SHORT3:
    m_element_size = sizeof(short)*3;
    break;
  case RT_FORMAT_SHORT4:
    m_element_size = sizeof(short)*4;
    break;
  case RT_FORMAT_UNSIGNED_SHORT:
    m_element_size = sizeof(unsigned short);
    break;
  case RT_FORMAT_UNSIGNED_SHORT2:
    m_element_size = sizeof(unsigned short)*2;
    break;
  case RT_FORMAT_UNSIGNED_SHORT3:
    m_element_size = sizeof(unsigned short)*3;
    break;
  case RT_FORMAT_UNSIGNED_SHORT4:
    m_element_size = sizeof(unsigned short)*4;
    break;
  case RT_FORMAT_INT:
    m_element_size = sizeof(int);
    break;
  case RT_FORMAT_INT2:
    m_element_size = sizeof(int)*2;
    break;
  case RT_FORMAT_INT3:
    m_element_size = sizeof(int)*3;
    break;
  case RT_FORMAT_INT4:
    m_element_size = sizeof(int)*4;
    break;
  case RT_FORMAT_UNSIGNED_INT:
    m_element_size = sizeof(unsigned int);
    break;
  case RT_FORMAT_UNSIGNED_INT2:
    m_element_size = sizeof(unsigned int)*2;
    break;
  case RT_FORMAT_UNSIGNED_INT3:
    m_element_size = sizeof(unsigned int)*3;
    break;
  case RT_FORMAT_UNSIGNED_INT4:
    m_element_size = sizeof(unsigned int)*4;
    break;
  case RT_FORMAT_USER:
  case RT_FORMAT_UNKNOWN:
    // These will return 0
    break;
  }

  return m_element_size;
}

//-----------------------------------------------------------------------------
// 
// C++ API
//
//-----------------------------------------------------------------------------

namespace optixu {
  class AccelerationObj;
  class BufferObj;
  class ContextObj;
  class GeometryObj;
  class GeometryGroupObj;
  class GeometryInstanceObj;
  class GroupObj;
  class MaterialObj;
  class NodeObj;
  class ProgramObj;
  class SelectorObj;
  class TextureSamplerObj;
  class TransformObj;
  class VariableObj;

  class APIObj;
  class ScopedObj;

  template<class T>
  class Handle {
  public:
    Handle() : ptr(0) {}

    Handle(T* ptr) : ptr(ptr) { ref(); }
    template<class U>
      Handle(U* ptr) : ptr(ptr) { ref(); }

    Handle(const Handle<T>& copy) : ptr(copy.ptr) { ref(); }
    template<class U>
      Handle(const Handle<U>& copy) : ptr(copy.ptr) { ref(); }

    Handle<T>& operator=(const Handle<T>& copy) { if(ptr != copy.ptr) { unref(); ptr = copy.ptr; ref(); } return *this; }
    template<class U>
      Handle<T>& operator=( const Handle<U>& copy) { if(ptr != copy.ptr) { unref(); ptr = copy.ptr; ref(); } return *this; }

    ~Handle() { unref(); }
    static Handle<T> take( typename T::api_t p ) { return p? new T(p) : 0; }
    T* operator->() { return ptr; }
    T* get() { return ptr; }

    // Only valid for ScopeObjs
    Handle<VariableObj> operator[](const std::string& varname);

    // Only valid for context
    static Handle<T> create() { return T::create(); } // Only valid for context
    static unsigned int getDeviceCount() { return T::getDeviceCount(); } // only valid for context
  private:
    inline void ref() { if(ptr) ptr->addReference(); }
    inline void unref() { if(ptr && ptr->removeReference() == 0) delete ptr; }
    T* ptr;
  };


  typedef Handle<AccelerationObj> Acceleration;
  typedef Handle<BufferObj> Buffer;
  typedef Handle<ContextObj> Context;
  typedef Handle<GeometryObj> Geometry;
  typedef Handle<GeometryGroupObj> GeometryGroup;
  typedef Handle<GeometryInstanceObj> GeometryInstance;
  typedef Handle<GroupObj> Group;
  typedef Handle<MaterialObj> Material;
  typedef Handle<NodeObj> Node;
  typedef Handle<ProgramObj> Program;
  typedef Handle<SelectorObj> Selector;
  typedef Handle<TextureSamplerObj> TextureSampler;
  typedef Handle<TransformObj> Transform;
  typedef Handle<VariableObj> Variable;

  typedef Handle<APIObj> APIObject;
  typedef Handle<ScopedObj> ScopedObject;

  class Exception {
  public:
  Exception( const std::string& message ) : m_message(message) {}
    const std::string& getErrorString() const { return m_message; }
  private:
    std::string m_message;
  };

  class APIObj {
  public:
    APIObj() : ref_count(0) {}
    virtual ~APIObj() {}

    void addReference() { ++ref_count; }
    int removeReference() { return --ref_count; }

    static Exception makeException( RTresult code, RTcontext context );
  private:
    int ref_count;
  };

  class DestroyableObj : public APIObj {
  public:
    virtual ~DestroyableObj() {}
    virtual void destroy() = 0;
    virtual void validate() = 0;
  };

  class ScopedObj : public DestroyableObj {
  public:
    virtual ~ScopedObj() {}
    virtual Variable declareVariable (const std::string& name) = 0;
    virtual Variable queryVariable   (const std::string& name) = 0;
    virtual void     removeVariable  (Variable v) = 0;
    virtual unsigned int getVariableCount() = 0;
    virtual Variable getVariable     (unsigned int index) = 0;
  };

  class VariableObj : public APIObj {
  public:
    typedef RTvariable api_t;

    /* sets */
    void setFloat(float f1);
    void setFloat(float2 f);
    void setFloat(float f1, float f2);
    void setFloat(float3 f);
    void setFloat(float f1, float f2, float f3);
    void setFloat(float4 f);
    void setFloat(float f1, float f2, float f3, float f4);
    void set1fv(const float* f);
    void set2fv(const float* f);
    void set3fv(const float* f);
    void set4fv(const float* f);

    void setInt(int i1);
    void setInt(int i1, int i2);
    void setInt(int2 i);
    void setInt(int i1, int i2, int i3);
    void setInt(int3 i);
    void setInt(int i1, int i2, int i3, int i4);
    void setInt(int4 i);
    void set1iv(const int* i);
    void set2iv(const int* i);
    void set3iv(const int* i);
    void set4iv(const int* i);

    void setUint(unsigned int u1);
    void setUint(unsigned int u1, unsigned int u2);
    void setUint(unsigned int u1, unsigned int u2, unsigned int u3);
    void setUint(unsigned int u1, unsigned int u2, unsigned int u3, unsigned int u4);
    void set1uiv(const unsigned int* u);
    void set2uiv(const unsigned int* u);
    void set3uiv(const unsigned int* u);
    void set4uiv(const unsigned int* u);

    void setMatrix2x2fv(bool transpose, const float* m);
    void setMatrix2x3fv(bool transpose, const float* m);
    void setMatrix2x4fv(bool transpose, const float* m);
    void setMatrix3x2fv(bool transpose, const float* m);
    void setMatrix3x3fv(bool transpose, const float* m);
    void setMatrix3x4fv(bool transpose, const float* m);
    void setMatrix4x2fv(bool transpose, const float* m);
    void setMatrix4x3fv(bool transpose, const float* m);
    void setMatrix4x4fv(bool transpose, const float* m);

  /* gets */
    float getFloat();
    unsigned int getUint();
    int getInt();
#if 0
    // Not implemented yet...

    // The getFloat functions can be overloaded by paramter type.
    void getFloat(float* f);
    void getFloat(float* f1, float* f2);
    void getFloat(float2* f);
    void getFloat(float* f1, float* f2, float* f3);
    void getFloat(float3* f);
    void getFloat(float* f1, float* f2, float* f3, float* f4);
    void getFloat(float4* f);
    // This one will need a different name to distinquish it from 'float getFloat()'.
    float2 getFloat2();
    float3 getFloat3();
    float4 getFloat4();

    void get1fv(float* f);
    void get2fv(float* f);
    void get3fv(float* f);
    void get4fv(float* f);

    get1i (int* i1);
    get2i (int* i1, int* i2);
    get3i (int* i1, int* i2, int* i3);
    get4i (int* i1, int* i2, int* i3, int* i4);
    get1iv(int* i);
    get2iv(int* i);
    get3iv(int* i);
    get4iv(int* i);

    get1ui (unsigned int* u1);
    get2ui (unsigned int* u1, unsigned int* u2);
    get3ui (unsigned int* u1, unsigned int* u2, unsigned int* u3);
    get4ui (unsigned int* u1, unsigned int* u2, unsigned int* u3, unsigned int* u4);
    get1uiv(unsigned int* u);
    get2uiv(unsigned int* u);
    get3uiv(unsigned int* u);
    get4uiv(unsigned int* u);

    getMatrix2x2fv(bool transpose, float* m);
    getMatrix2x3fv(bool transpose, float* m);
    getMatrix2x4fv(bool transpose, float* m);
    getMatrix3x2fv(bool transpose, float* m);
    getMatrix3x3fv(bool transpose, float* m);
    getMatrix3x4fv(bool transpose, float* m);
    getMatrix4x2fv(bool transpose, float* m);
    getMatrix4x3fv(bool transpose, float* m);
    getMatrix4x4fv(bool transpose, float* m);
#endif

    void setBuffer(Buffer buffer);
    void set(Buffer buffer);
    void setNode(Node node);
    void set(Node node);
    void setTextureSampler(TextureSampler texturesample);
    void set(TextureSampler texturesample);

    // Additional overloads for node subclasses to avoid disambiguation issues
    void set(GeometryGroup group);
    void set(Group group);
    void set(Selector selector);
    void set(Transform transform);

    Buffer getBuffer();
    Node getNode();
    TextureSampler getTextureSampler();

    void setUserData(RTsize size, const void* ptr);
    void getUserData(RTsize size,       void* ptr);

    std::string getName();
    std::string getAnnotation();
    RTobjecttype getType();
    RTvariable get();
    void checkError(RTresult code);

  private:
    RTvariable m_variable;
    VariableObj(RTvariable variable) : m_variable(variable) {}
    friend class Handle<VariableObj>;

  };

  template<class T>
    Handle<VariableObj> Handle<T>::operator[](const std::string& varname)
  {
    Variable v = ptr->queryVariable( varname );
    if( v.operator->() == 0)
      v = ptr->declareVariable( varname );
    return v;
  }

  class ContextObj : public ScopedObj {
  public:
    static unsigned int getDeviceCount();

    static Context create();
    void destroy();
    void validate();

    Acceleration createAcceleration(const char* builder, const char* traverser);

    Buffer createBuffer(RTbuffertype type);
    Buffer createBuffer(RTbuffertype type, RTformat format, RTsize width);
    Buffer createBuffer(RTbuffertype type, RTformat format, RTsize width, RTsize height);
    Buffer createBuffer(RTbuffertype type, RTformat format, RTsize width, RTsize height, RTsize depth);

    Buffer createBufferFromGLBO(RTbuffertype type, unsigned int vbo);
    TextureSampler createTextureSamplerFromGLImage(unsigned int id, RTgltarget target );

#ifdef _WIN32

    Buffer createBufferFromD3D9Resource(RTbuffertype type, IDirect3DResource9 *pResource);
    Buffer createBufferFromD3D10Resource(RTbuffertype type, ID3D10Resource *pResource);
    Buffer createBufferFromD3D11Resource(RTbuffertype type, ID3D11Resource *pResource);

    TextureSampler createTextureSamplerFromD3D9Resource(IDirect3DResource9 *pResource);
    TextureSampler createTextureSamplerFromD3D10Resource(ID3D10Resource *pResource);
    TextureSampler createTextureSamplerFromD3D11Resource(ID3D11Resource *pResource);

    void setD3D9Device(IDirect3DDevice9* device);
    void setD3D10Device(ID3D10Device* device);
    void setD3D11Device(ID3D11Device* device);

#endif

    Geometry createGeometry();
    GeometryInstance createGeometryInstance();
    template<class Iterator>
      GeometryInstance createGeometryInstance( Geometry geometry, Iterator matlbegin, Iterator matlend);
    Group createGroup();
    template<class Iterator>
      Group createGroup( Iterator childbegin, Iterator childend );
    GeometryGroup createGeometryGroup();
    template<class Iterator>
      GeometryGroup createGeometryGroup( Iterator childbegin, Iterator childend );
    Transform createTransform();
    Material createMaterial();
    Program createProgramFromPTXFile  ( const std::string& ptx, const std::string& program_name );
    Program createProgramFromPTXString( const std::string& ptx, const std::string& program_name );
    Selector createSelector();
    TextureSampler createTextureSampler();

    std::string getErrorString( RTresult code );
    template<class Iterator>
      void setDevices(Iterator begin, Iterator end);

    void setStackSize(RTsize  stack_size_bytes);
    RTsize getStackSize();

    void setEntryPointCount(unsigned int  num_entry_points);
    unsigned int getEntryPointCount();

    void setRayGenerationProgram(unsigned int entry_point_index, Program  program);
    Program getRayGenerationProgram(unsigned int entry_point_index);

    void setExceptionProgram(unsigned int entry_point_index, Program  program);
    Program getExceptionProgram(unsigned int entry_point_index);

    void setExceptionEnabled( RTexception exception, bool enabled );
    bool getExceptionEnabled( RTexception exception );

    void setRayTypeCount(unsigned int  num_ray_types);
    unsigned int getRayTypeCount();

    void setMissProgram(unsigned int ray_type_index, Program  program);
    Program getMissProgram(unsigned int ray_type_index);

    void compile();

    void launch(unsigned int entry_point_index, RTsize image_width);
    void launch(unsigned int entry_point_index, RTsize image_width, RTsize image_height);
    void launch(unsigned int entry_point_index, RTsize image_width, RTsize image_height, RTsize image_depth);

    int getRunningState();

    void setPrintEnabled(bool enabled);
    bool getPrintEnabled();
    void setPrintBufferSize(RTsize buffer_size_bytes);
    RTsize getPrintBufferSize();
    void setPrintLaunchIndex(int x, int y=-1, int z=-1);
    int3 getPrintLaunchIndex();

    Variable declareVariable (const std::string& name);
    Variable queryVariable   (const std::string& name);
    void     removeVariable  (Variable v);
    unsigned int getVariableCount();
    Variable getVariable     (unsigned int index);

    void checkError(RTresult code);

    RTcontext get();
  private:
    typedef RTcontext api_t;

    virtual ~ContextObj() {}
    RTcontext m_context;
    ContextObj(RTcontext context) : m_context(context) {}
    friend class Handle<ContextObj>;
  };

  class ProgramObj : public ScopedObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    Variable declareVariable (const std::string& name);
    Variable queryVariable   (const std::string& name);
    void     removeVariable  (Variable v);
    unsigned int getVariableCount();
    Variable getVariable     (unsigned int index);

    RTprogram get();
    void checkError(RTresult code);
  private:
    typedef RTprogram api_t;
    virtual ~ProgramObj() {}
    RTprogram m_program;
    ProgramObj(RTprogram program) : m_program(program) {}
    friend class Handle<ProgramObj>;
  };

  class NodeObj : public DestroyableObj {
  public:
    RTobject get();
    void checkError(RTresult code);
  protected:
    NodeObj() {}
    virtual ~NodeObj() {}
  private:
    typedef RTgroup api_t;
    friend class Handle<NodeObj>;
  };

  class GroupObj : public NodeObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setAcceleration(Acceleration acceleration);
    Acceleration getAcceleration();

    void setChildCount(unsigned int  count);
    unsigned int getChildCount();

    template< typename T > void setChild(unsigned int index, T child);
    template< typename T > T getChild(unsigned int index);

    RTgroup get();
    void checkError(RTresult code);

  private:
    typedef RTgroup api_t;
    virtual ~GroupObj() {}
    RTgroup m_group;
    GroupObj(RTgroup group) : m_group(group) {}
    friend class Handle<GroupObj>;
  };

  class GeometryGroupObj : public NodeObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setAcceleration(Acceleration acceleration);
    Acceleration getAcceleration();

    void setChildCount(unsigned int  count);
    unsigned int getChildCount();

    void setChild(unsigned int index, GeometryInstance geometryinstance);
    GeometryInstance getChild(unsigned int index);

    RTgeometrygroup get();
    void checkError(RTresult code);

  private:
    typedef RTgeometrygroup api_t;
    virtual ~GeometryGroupObj() {}
    RTgeometrygroup m_geometrygroup;
    GeometryGroupObj(RTgeometrygroup geometrygroup) : m_geometrygroup(geometrygroup) {}
    friend class Handle<GeometryGroupObj>;
  };

  class TransformObj : public NodeObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    template< typename T > void setChild(T child);
    template< typename T > T getChild();

    void setMatrix(bool transpose, const float* matrix, const float* inverse_matrix);
    void getMatrix(bool transpose, float* matrix, float* inverse_matrix);

    RTtransform get();
    void checkError(RTresult code);

  private:
    typedef RTtransform api_t;
    virtual ~TransformObj() {}
    RTtransform m_transform;
    TransformObj(RTtransform transform) : m_transform(transform) {}
    friend class Handle<TransformObj>;
  };

  class SelectorObj : public NodeObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setVisitProgram(Program  program);
    Program getVisitProgram();

    void setChildCount(unsigned int  count);
    unsigned int getChildCount();

    template< typename T > void setChild(unsigned int index, T child);
    template< typename T > T getChild(unsigned int index);

    Variable declareVariable (const std::string& name);
    Variable queryVariable   (const std::string& name);
    void     removeVariable  (Variable v);
    unsigned int getVariableCount();
    Variable getVariable     (unsigned int index);

    RTselector get();
    void checkError(RTresult code);

  public:
    typedef RTselector api_t;
    virtual ~SelectorObj() {}
    RTselector m_selector;
    SelectorObj(RTselector selector) : m_selector(selector) {}
    friend class Handle<SelectorObj>;
  };

  class AccelerationObj : public DestroyableObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void markDirty();
    bool isDirty();

    void        setProperty( const std::string& name, const std::string& value );
    std::string getProperty( const std::string& name );
    void        setBuilder(const std::string& builder);
    std::string getBuilder();
    void        setTraverser(const std::string& traverser);
    std::string getTraverser();

    RTsize getDataSize();
    void   getData( void* data );
    void   setData( const void* data, RTsize size );

    RTacceleration get();
    void checkError(RTresult code);

  public:
    typedef RTacceleration api_t;
    virtual ~AccelerationObj() {}
    RTacceleration m_acceleration;
    AccelerationObj(RTacceleration acceleration) : m_acceleration(acceleration) {}
    friend class Handle<AccelerationObj>;
  };

  class GeometryInstanceObj : public ScopedObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setGeometry(Geometry  geometry);
    Geometry getGeometry();

    void setMaterialCount(unsigned int  count);
    unsigned int getMaterialCount();

    void setMaterial(unsigned int idx, Material  material);
    Material getMaterial(unsigned int idx);

    // Adds the material and returns the index to the added material.
    unsigned int addMaterial(Material material);

    Variable declareVariable (const std::string& name);
    Variable queryVariable   (const std::string& name);
    void     removeVariable  (Variable v);
    unsigned int getVariableCount();
    Variable getVariable     (unsigned int index);

    RTgeometryinstance get();
    void checkError(RTresult code);

  public:
    typedef RTgeometryinstance api_t;
    virtual ~GeometryInstanceObj() {}
    RTgeometryinstance m_geometryinstance;
    GeometryInstanceObj(RTgeometryinstance geometryinstance) : m_geometryinstance(geometryinstance) {}
    friend class Handle<GeometryInstanceObj>;
  };

  class GeometryObj : public ScopedObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void markDirty();
    bool isDirty();

    void setPrimitiveCount(unsigned int  num_primitives);
    unsigned int getPrimitiveCount();

    void setBoundingBoxProgram(Program  program);
    Program getBoundingBoxProgram();

    void setIntersectionProgram(Program  program);
    Program getIntersectionProgram();

    Variable declareVariable (const std::string& name);
    Variable queryVariable   (const std::string& name);
    void     removeVariable  (Variable v);
    unsigned int getVariableCount();
    Variable getVariable     (unsigned int index);

    RTgeometry get();
    void checkError(RTresult code);

  private:
    typedef RTgeometry api_t;
    virtual ~GeometryObj() {}
    RTgeometry m_geometry;
    GeometryObj(RTgeometry geometry) : m_geometry(geometry) {}
    friend class Handle<GeometryObj>;
  };

  class MaterialObj : public ScopedObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setClosestHitProgram(unsigned int ray_type_index, Program  program);
    Program getClosestHitProgram(unsigned int ray_type_index);

    void setAnyHitProgram(unsigned int ray_type_index, Program  program);
    Program getAnyHitProgram(unsigned int ray_type_index);

    Variable declareVariable (const std::string& name);
    Variable queryVariable   (const std::string& name);
    void     removeVariable  (Variable v);
    unsigned int getVariableCount();
    Variable getVariable     (unsigned int index);

    RTmaterial get();
    void checkError(RTresult code);
  private:
    typedef RTmaterial api_t;
    virtual ~MaterialObj() {}
    RTmaterial m_material;
    MaterialObj(RTmaterial material) : m_material(material) {}
    friend class Handle<MaterialObj>;
  };

  class TextureSamplerObj : public DestroyableObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setMipLevelCount (unsigned int  num_mip_levels);
    unsigned int getMipLevelCount ();

    void setArraySize(unsigned int  num_textures_in_array);
    unsigned int getArraySize();

    void setWrapMode(unsigned int dim, RTwrapmode wrapmode);
    RTwrapmode getWrapMode(unsigned int dim);

    void setFilteringModes(RTfiltermode  minification, RTfiltermode  magnification, RTfiltermode  mipmapping);
    void getFilteringModes(RTfiltermode& minification, RTfiltermode& magnification, RTfiltermode& mipmapping);

    void setMaxAnisotropy(float value);
    float getMaxAnisotropy();

    void setReadMode(RTtexturereadmode  readmode);
    RTtexturereadmode getReadMode();

    void setIndexingMode(RTtextureindexmode  indexmode);
    RTtextureindexmode getIndexingMode();

    void setBuffer(unsigned int texture_array_idx, unsigned int mip_level, Buffer buffer);
    Buffer getBuffer(unsigned int texture_array_idx, unsigned int mip_level);

    RTtexturesampler get();
    void checkError(RTresult code);

    void registerGLTexture();
    void unregisterGLTexture();

#ifdef _WIN32

    void registerD3D9Texture();
    void registerD3D10Texture();
    void registerD3D11Texture();

    void unregisterD3D9Texture();
    void unregisterD3D10Texture();
    void unregisterD3D11Texture();

#endif

  private:
    typedef RTtexturesampler api_t;
    virtual ~TextureSamplerObj() {}
    RTtexturesampler m_texturesampler;
    TextureSamplerObj(RTtexturesampler texturesampler) : m_texturesampler(texturesampler) {}
    friend class Handle<TextureSamplerObj>;
  };

  class BufferObj : public DestroyableObj {
  public:
    void destroy();
    void validate();
    Context getContext();

    void setFormat    (RTformat  format);
    RTformat getFormat();

    void setElementSize  (RTsize size_of_element);
    RTsize getElementSize();

    void setSize(RTsize  width);
    void getSize(RTsize& width);
    void setSize(RTsize  width, RTsize  height);
    void getSize(RTsize& width, RTsize& height);
    void setSize(RTsize  width, RTsize  height, RTsize  depth);
    void getSize(RTsize& width, RTsize& height, RTsize& depth);
    void setSize(unsigned int dimensionality, const RTsize* dims);
    void getSize(unsigned int dimensionality,       RTsize* dims);

    unsigned int getDimensionality();

    unsigned int getGLBOId();

    void registerGLBuffer();
    void unregisterGLBuffer();

#ifdef _WIN32

    void registerD3D9Buffer();
    void registerD3D10Buffer();
    void registerD3D11Buffer();

    void unregisterD3D9Buffer();
    void unregisterD3D10Buffer();
    void unregisterD3D11Buffer();

    IDirect3DResource9* getD3D9Resource();
    ID3D10Resource* getD3D10Resource();
    ID3D11Resource* getD3D11Resource();

#endif

    void* map();
    void unmap();

    RTbuffer get();
    void checkError(RTresult code);
  public:
    typedef RTbuffer api_t;
    virtual ~BufferObj() {}
    RTbuffer m_buffer;
    BufferObj(RTbuffer buffer) : m_buffer(buffer) {}
    friend class Handle<BufferObj>;
  };

  inline Exception APIObj::makeException( RTresult code, RTcontext context )
  {
    const char* str;
    rtContextGetErrorString( context, code, &str);
    return Exception( std::string(str) );
  }

  inline void ContextObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS)
      throw APIObj::makeException( code, m_context );
  }

  inline unsigned int ContextObj::getDeviceCount()
  {
    unsigned int count;
    if( RTresult code = rtDeviceGetDeviceCount(&count) )
      throw APIObj::makeException( code, 0 );

    return count;
  }


  inline Context ContextObj::create()
  {
    RTcontext c;
    if( RTresult code = rtContextCreate(&c) )
      throw APIObj::makeException( code, 0 );

    return Context::take(c);
  }

  inline void ContextObj::destroy()
  {
    checkError( rtContextDestroy( m_context ) );
    m_context = 0;
  }

  inline void ContextObj::validate()
  {
    checkError( rtContextValidate( m_context ) );
  }

  inline Acceleration ContextObj::createAcceleration(const char* builder, const char* traverser)
  {
    RTacceleration acceleration;
    checkError( rtAccelerationCreate( m_context, &acceleration ) );
    checkError( rtAccelerationSetBuilder( acceleration, builder ) );
    checkError( rtAccelerationSetTraverser( acceleration, traverser ) );
    return Acceleration::take(acceleration);
  }


  inline Buffer ContextObj::createBuffer(RTbuffertype type)
  {
    RTbuffer buffer;
    checkError( rtBufferCreate( m_context, type, &buffer ) );
    return Buffer::take(buffer);
  }

  inline Buffer ContextObj::createBuffer(RTbuffertype type, RTformat format, RTsize width)
  {
    RTbuffer buffer;
    checkError( rtBufferCreate( m_context, type, &buffer ) );
    checkError( rtBufferSetFormat( buffer, format ) );
    checkError( rtBufferSetSize1D( buffer, width ) );
    return Buffer::take(buffer);
  }

  inline Buffer ContextObj::createBuffer(RTbuffertype type, RTformat format, RTsize width, RTsize height)
  {
    RTbuffer buffer;
    checkError( rtBufferCreate( m_context, type, &buffer ) );
    checkError( rtBufferSetFormat( buffer, format ) );
    checkError( rtBufferSetSize2D( buffer, width, height ) );
    return Buffer::take(buffer);
  }

  inline Buffer ContextObj::createBuffer(RTbuffertype type, RTformat format, RTsize width, RTsize height, RTsize depth)
  {
    RTbuffer buffer;
    checkError( rtBufferCreate( m_context, type, &buffer ) );
    checkError( rtBufferSetFormat( buffer, format ) );
    checkError( rtBufferSetSize3D( buffer, width, height, depth ) );
    return Buffer::take(buffer);
  }

  inline Buffer ContextObj::createBufferFromGLBO(RTbuffertype type, unsigned int vbo)
  {
    RTbuffer buffer;
    checkError( rtBufferCreateFromGLBO( m_context, type, vbo, &buffer ) );
    return Buffer::take(buffer);
  }

#ifdef _WIN32

  inline Buffer ContextObj::createBufferFromD3D9Resource(RTbuffertype type, IDirect3DResource9 *pResource)
  {
    RTbuffer buffer;
    checkError( rtBufferCreateFromD3D9Resource( m_context, type, pResource, &buffer ) );
    return Buffer::take(buffer);
  }

  inline Buffer ContextObj::createBufferFromD3D10Resource(RTbuffertype type, ID3D10Resource *pResource)
  {
    RTbuffer buffer;
    checkError( rtBufferCreateFromD3D10Resource( m_context, type, pResource, &buffer ) );
    return Buffer::take(buffer);
  }

  inline Buffer ContextObj::createBufferFromD3D11Resource(RTbuffertype type, ID3D11Resource *pResource)
  {
    RTbuffer buffer;
    checkError( rtBufferCreateFromD3D11Resource( m_context, type, pResource, &buffer ) );
    return Buffer::take(buffer);
  }

  inline TextureSampler ContextObj::createTextureSamplerFromD3D9Resource(IDirect3DResource9 *pResource)
  {
    RTtexturesampler textureSampler;
    checkError( rtTextureSamplerCreateFromD3D9Resource(m_context, pResource, &textureSampler));
    return TextureSampler::take(textureSampler);
  }

  inline TextureSampler ContextObj::createTextureSamplerFromD3D10Resource(ID3D10Resource *pResource)
  {
    RTtexturesampler textureSampler;
    checkError( rtTextureSamplerCreateFromD3D10Resource(m_context, pResource, &textureSampler));
    return TextureSampler::take(textureSampler);
  }

  inline TextureSampler ContextObj::createTextureSamplerFromD3D11Resource(ID3D11Resource *pResource)
  {
    RTtexturesampler textureSampler;
    checkError( rtTextureSamplerCreateFromD3D11Resource(m_context, pResource, &textureSampler));
    return TextureSampler::take(textureSampler);
  }

  inline void ContextObj::setD3D9Device(IDirect3DDevice9* device)
  {
    checkError( rtContextSetD3D9Device( m_context, device ) );
  }

  inline void ContextObj::setD3D10Device(ID3D10Device* device)
  {
    checkError( rtContextSetD3D10Device( m_context, device ) );
  }

  inline void ContextObj::setD3D11Device(ID3D11Device* device)
  {
    checkError( rtContextSetD3D11Device( m_context, device ) );
  }

#endif

  inline TextureSampler ContextObj::createTextureSamplerFromGLImage(unsigned int id, RTgltarget target)
  {
    RTtexturesampler textureSampler;
    checkError( rtTextureSamplerCreateFromGLImage(m_context, id, target, &textureSampler));
    return TextureSampler::take(textureSampler);
  }

  inline Geometry ContextObj::createGeometry()
  {
    RTgeometry geometry;
    checkError( rtGeometryCreate( m_context, &geometry ) );
    return Geometry::take(geometry);
  }

  inline GeometryInstance ContextObj::createGeometryInstance()
  {
    RTgeometryinstance geometryinstance;
    checkError( rtGeometryInstanceCreate( m_context, &geometryinstance ) );
    return GeometryInstance::take(geometryinstance);
  }

  template<class Iterator>
    GeometryInstance ContextObj::createGeometryInstance( Geometry geometry, Iterator matlbegin, Iterator matlend)
  {
    GeometryInstance result = createGeometryInstance();
    result->setGeometry( geometry );
    unsigned int count = 0;
    for( Iterator iter = matlbegin; iter != matlend; ++iter )
      ++count;
    result->setMaterialCount( count );
    unsigned int index = 0;
    for(Iterator iter = matlbegin; iter != matlend; ++iter, ++index )
      result->setMaterial( index, *iter );
    return result;
  }

  inline Group ContextObj::createGroup()
  {
    RTgroup group;
    checkError( rtGroupCreate( m_context, &group ) );
    return Group::take(group);
  }

  template<class Iterator>
    inline Group ContextObj::createGroup( Iterator childbegin, Iterator childend )
  {
    Group result = createGroup();
    unsigned int count = 0;
    for(Iterator iter = childbegin; iter != childend; ++iter )
      ++count;
    result->setChildCount( count );
    unsigned int index = 0;
    for(Iterator iter = childbegin; iter != childend; ++iter, ++index )
      result->setChild( index, *iter );
    return result;
  }

  inline GeometryGroup ContextObj::createGeometryGroup()
  {
    RTgeometrygroup gg;
    checkError( rtGeometryGroupCreate( m_context, &gg ) );
    return GeometryGroup::take( gg );
  }

  template<class Iterator>
  inline GeometryGroup ContextObj::createGeometryGroup( Iterator childbegin, Iterator childend )
  {
    GeometryGroup result = createGeometryGroup();
    unsigned int count = 0;
    for(Iterator iter = childbegin; iter != childend; ++iter )
      ++count;
    result->setChildCount( count );
    unsigned int index = 0;
    for(Iterator iter = childbegin; iter != childend; ++iter, ++index )
      result->setChild( index, *iter );
    return result;
  }

  inline Transform ContextObj::createTransform()
  {
    RTtransform t;
    checkError( rtTransformCreate( m_context, &t ) );
    return Transform::take( t );
  }

  inline Material ContextObj::createMaterial()
  {
    RTmaterial material;
    checkError( rtMaterialCreate( m_context, &material ) );
    return Material::take(material);
  }

  inline Program ContextObj::createProgramFromPTXFile( const std::string& filename, const std::string& program_name )
  {
    RTprogram program;
    checkError( rtProgramCreateFromPTXFile( m_context, filename.c_str(), program_name.c_str(), &program ) );
    return Program::take(program);
  }

  inline Program ContextObj::createProgramFromPTXString( const std::string& ptx, const std::string& program_name )
  {
    RTprogram program;
    checkError( rtProgramCreateFromPTXString( m_context, ptx.c_str(), program_name.c_str(), &program ) );
    return Program::take(program);
  }

  inline Selector ContextObj::createSelector()
  {
    RTselector selector;
    checkError( rtSelectorCreate( m_context, &selector ) );
    return Selector::take(selector);
  }

  inline TextureSampler ContextObj::createTextureSampler()
  {
    RTtexturesampler texturesampler;
    checkError( rtTextureSamplerCreate( m_context, &texturesampler ) );
    return TextureSampler::take(texturesampler);
  }

  inline std::string ContextObj::getErrorString( RTresult code )
  {
    const char* str;
    rtContextGetErrorString( m_context, code, &str);
    return std::string(str);
  }

  template<class Iterator> inline
    void ContextObj::setDevices(Iterator begin, Iterator end)
  {
    std::vector<int> devices;
    std::copy( begin, end, std::insert_iterator<std::vector<int> >( devices, devices.begin() ) );
    checkError( rtContextSetDevices( m_context, static_cast<unsigned int>(devices.size()), &devices[0]) );
  }

  inline void ContextObj::setStackSize(RTsize  stack_size_bytes)
  {
    checkError(rtContextSetStackSize(m_context, stack_size_bytes) );
  }

  inline RTsize ContextObj::getStackSize()
  {
    RTsize result;
    checkError( rtContextGetStackSize( m_context, &result ) );
    return result;
  }

  inline void ContextObj::setEntryPointCount(unsigned int  num_entry_points)
  {
    checkError( rtContextSetEntryPointCount( m_context, num_entry_points ) );
  }

  inline unsigned int ContextObj::getEntryPointCount()
  {
    unsigned int result;
    checkError( rtContextGetEntryPointCount( m_context, &result ) );
    return result;
  }


  inline void ContextObj::setRayGenerationProgram(unsigned int entry_point_index, Program  program)
  {
    checkError( rtContextSetRayGenerationProgram( m_context, entry_point_index, program->get() ) );
  }

  inline Program ContextObj::getRayGenerationProgram(unsigned int entry_point_index)
  {
    RTprogram result;
    checkError( rtContextGetRayGenerationProgram( m_context, entry_point_index, &result ) );
    return Program::take( result );
  }


  inline void ContextObj::setExceptionProgram(unsigned int entry_point_index, Program  program)
  {
    checkError( rtContextSetExceptionProgram( m_context, entry_point_index, program->get() ) );
  }

  inline Program ContextObj::getExceptionProgram(unsigned int entry_point_index)
  {
    RTprogram result;
    checkError( rtContextGetExceptionProgram( m_context, entry_point_index, &result ) );
    return Program::take( result );
  }


  inline void ContextObj::setExceptionEnabled( RTexception exception, bool enabled )
  {
    checkError( rtContextSetExceptionEnabled( m_context, exception, enabled ) );
  }

  inline bool ContextObj::getExceptionEnabled( RTexception exception )
  {
    int enabled;
    checkError( rtContextGetExceptionEnabled( m_context, exception, &enabled ) );
    return enabled != 0;
  }


  inline void ContextObj::setRayTypeCount(unsigned int  num_ray_types)
  {
    checkError( rtContextSetRayTypeCount( m_context, num_ray_types ) );
  }

  inline unsigned int ContextObj::getRayTypeCount()
  {
    unsigned int result;
    checkError( rtContextGetRayTypeCount( m_context, &result ) );
    return result;
  }

  inline void ContextObj::setMissProgram(unsigned int ray_type_index, Program  program)
  {
    checkError( rtContextSetMissProgram( m_context, ray_type_index, program->get() ) );
  }

  inline Program ContextObj::getMissProgram(unsigned int ray_type_index)
  {
    RTprogram result;
    checkError( rtContextGetMissProgram( m_context, ray_type_index, &result ) );
    return Program::take( result );
  }

  inline void ContextObj::compile()
  {
    checkError( rtContextCompile( m_context ) );
  }

  inline void ContextObj::launch(unsigned int entry_point_index, RTsize image_width)
  {
    checkError( rtContextLaunch1D( m_context, entry_point_index, image_width ) );
  }

  inline void ContextObj::launch(unsigned int entry_point_index, RTsize image_width, RTsize image_height)
  {
    checkError( rtContextLaunch2D( m_context, entry_point_index, image_width, image_height ) );
  }

  inline void ContextObj::launch(unsigned int entry_point_index, RTsize image_width, RTsize image_height, RTsize image_depth)
  {
    checkError( rtContextLaunch3D( m_context, entry_point_index, image_width, image_height, image_depth ) );
  }


  inline int ContextObj::getRunningState()
  {
    int result;
    checkError( rtContextGetRunningState( m_context, &result ) );
    return result;
  }

  inline void ContextObj::setPrintEnabled(bool enabled)
  {
    checkError( rtContextSetPrintEnabled( m_context, enabled ) );
  }

  inline bool ContextObj::getPrintEnabled()
  {
    int enabled;
    checkError( rtContextGetPrintEnabled( m_context, &enabled ) );
    return enabled != 0;
  }

  inline void ContextObj::setPrintBufferSize(RTsize buffer_size_bytes)
  {
    checkError( rtContextSetPrintBufferSize( m_context, buffer_size_bytes ) );
  }

  inline RTsize ContextObj::getPrintBufferSize()
  {
    RTsize result;
    checkError( rtContextGetPrintBufferSize( m_context, &result ) );
    return result;
  }

  inline void ContextObj::setPrintLaunchIndex(int x, int y, int z)
  {
    checkError( rtContextSetPrintLaunchIndex( m_context, x, y, z ) );
  }

  inline int3 ContextObj::getPrintLaunchIndex()
  {
    int3 result;
    checkError( rtContextGetPrintLaunchIndex( m_context, &result.x, &result.y, &result.z ) );
    return result;
  }

  inline Variable ContextObj::declareVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtContextDeclareVariable( m_context, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline Variable ContextObj::queryVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtContextQueryVariable( m_context, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline void ContextObj::removeVariable(Variable v)
  {
    checkError( rtContextRemoveVariable( m_context, v->get() ) );
  }

  inline unsigned int ContextObj::getVariableCount()
  {
    unsigned int result;
    checkError( rtContextGetVariableCount( m_context, &result ) );
    return result;
  }

  inline Variable ContextObj::getVariable(unsigned int index)
  {
    RTvariable v;
    checkError( rtContextGetVariable( m_context, index, &v ) );
    return Variable::take( v );
  }


  inline RTcontext ContextObj::get()
  {
    return m_context;
  }

  inline void ProgramObj::destroy()
  {
    checkError( rtProgramDestroy( m_program ) );
  }

  inline void ProgramObj::validate()
  {
    checkError( rtProgramValidate( m_program ) );
  }

  inline Context ProgramObj::getContext()
  {
    RTcontext c;
    checkError( rtProgramGetContext( m_program, &c ) );
    return Context::take( c );
  }

  inline Variable ProgramObj::declareVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtProgramDeclareVariable( m_program, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline Variable ProgramObj::queryVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtProgramQueryVariable( m_program, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline void ProgramObj::removeVariable(Variable v)
  {
    checkError( rtProgramRemoveVariable( m_program, v->get() ) );
  }

  inline unsigned int ProgramObj::getVariableCount()
  {
    unsigned int result;
    checkError( rtProgramGetVariableCount( m_program, &result ) );
    return result;
  }

  inline Variable ProgramObj::getVariable(unsigned int index)
  {
    RTvariable v;
    checkError( rtProgramGetVariable( m_program, index, &v ) );
    return Variable::take(v);
  }

  inline RTprogram ProgramObj::get()
  {
    return m_program;
  }

  inline void ProgramObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtProgramGetContext( m_program, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void GroupObj::destroy()
  {
    checkError( rtGroupDestroy( m_group ) );
  }

  inline void GroupObj::validate()
  {
    checkError( rtGroupValidate( m_group ) );
  }

  inline Context GroupObj::getContext()
  {
    RTcontext c;
    checkError( rtGroupGetContext( m_group, &c) );
    return Context::take(c);
  }

  inline void SelectorObj::destroy()
  {
    checkError( rtSelectorDestroy( m_selector ) );
  }

  inline void SelectorObj::validate()
  {
    checkError( rtSelectorValidate( m_selector ) );
  }

  inline Context SelectorObj::getContext()
  {
    RTcontext c;
    checkError( rtSelectorGetContext( m_selector, &c ) );
    return Context::take( c );
  }

  inline void SelectorObj::setVisitProgram(Program program)
  {
    checkError( rtSelectorSetVisitProgram( m_selector, program->get() ) );
  }

  inline Program SelectorObj::getVisitProgram()
  {
    RTprogram result;
    checkError( rtSelectorGetVisitProgram( m_selector, &result ) );
    return Program::take( result );
  }

  inline void SelectorObj::setChildCount(unsigned int count)
  {
    checkError( rtSelectorSetChildCount( m_selector, count) );
  }

  inline unsigned int SelectorObj::getChildCount()
  {
    unsigned int result;
    checkError( rtSelectorGetChildCount( m_selector, &result ) );
    return result;
  }

  template< typename T >
  inline void SelectorObj::setChild(unsigned int index, T child)
  {
    checkError( rtSelectorSetChild( m_selector, index, child->get() ) );
  }

  template< typename T >
  inline T SelectorObj::getChild(unsigned int index)
  {
    RTobject result;
    checkError( rtSelectorGetChild( m_selector, index, &result ) );
    return T::take( result );
  }

  inline Variable SelectorObj::declareVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtSelectorDeclareVariable( m_selector, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline Variable SelectorObj::queryVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtSelectorQueryVariable( m_selector, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline void SelectorObj::removeVariable(Variable v)
  {
    checkError( rtSelectorRemoveVariable( m_selector, v->get() ) );
  }

  inline unsigned int SelectorObj::getVariableCount()
  {
    unsigned int result;
    checkError( rtSelectorGetVariableCount( m_selector, &result ) );
    return result;
  }

  inline Variable SelectorObj::getVariable(unsigned int index)
  {
    RTvariable v;
    checkError( rtSelectorGetVariable( m_selector, index, &v ) );
    return Variable::take( v );
  }

  inline RTselector SelectorObj::get()
  {
    return m_selector;
  }

  inline void SelectorObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtSelectorGetContext( m_selector, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void GroupObj::setAcceleration(Acceleration acceleration)
  {
    checkError( rtGroupSetAcceleration( m_group, acceleration->get() ) );
  }

  inline Acceleration GroupObj::getAcceleration()
  {
    RTacceleration result;
    checkError( rtGroupGetAcceleration( m_group, &result ) );
    return Acceleration::take( result );
  }

  inline void GroupObj::setChildCount(unsigned int  count)
  {
    checkError( rtGroupSetChildCount( m_group, count ) );
  }

  inline unsigned int GroupObj::getChildCount()
  {
    unsigned int result;
    checkError( rtGroupGetChildCount( m_group, &result ) );
    return result;
  }

  template< typename T >
  inline void GroupObj::setChild(unsigned int index, T child)
  {
    checkError( rtGroupSetChild( m_group, index, child->get() ) );
  }

  template< typename T >
  inline T GroupObj::getChild(unsigned int index)
  {
    RTobject result;
    checkError( rtGroupGetChild( m_group, index, &result) );
    return T::take( result );
  }

  inline RTgroup GroupObj::get()
  {
    return m_group;
  }

  inline void GroupObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtGroupGetContext( m_group, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void GeometryGroupObj::destroy()
  {
    checkError( rtGeometryGroupDestroy( m_geometrygroup ) );
  }

  inline void GeometryGroupObj::validate()
  {
    checkError( rtGeometryGroupValidate( m_geometrygroup ) );
  }

  inline Context GeometryGroupObj::getContext()
  {
    RTcontext c;
    checkError( rtGeometryGroupGetContext( m_geometrygroup, &c) );
    return Context::take(c);
  }

  inline void GeometryGroupObj::setAcceleration(Acceleration acceleration)
  {
    checkError( rtGeometryGroupSetAcceleration( m_geometrygroup, acceleration->get() ) );
  }

  inline Acceleration GeometryGroupObj::getAcceleration()
  {
    RTacceleration result;
    checkError( rtGeometryGroupGetAcceleration( m_geometrygroup, &result ) );
    return Acceleration::take( result );
  }

  inline void GeometryGroupObj::setChildCount(unsigned int  count)
  {
    checkError( rtGeometryGroupSetChildCount( m_geometrygroup, count ) );
  }

  inline unsigned int GeometryGroupObj::getChildCount()
  {
    unsigned int result;
    checkError( rtGeometryGroupGetChildCount( m_geometrygroup, &result ) );
    return result;
  }

  inline void GeometryGroupObj::setChild(unsigned int index, GeometryInstance child)
  {
    checkError( rtGeometryGroupSetChild( m_geometrygroup, index, child->get() ) );
  }

  inline GeometryInstance GeometryGroupObj::getChild(unsigned int index)
  {
    RTgeometryinstance result;
    checkError( rtGeometryGroupGetChild( m_geometrygroup, index, &result) );
    return GeometryInstance::take( result );
  }

  inline RTgeometrygroup GeometryGroupObj::get()
  {
    return m_geometrygroup;
  }

  inline void GeometryGroupObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtGeometryGroupGetContext( m_geometrygroup, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void TransformObj::destroy()
  {
    checkError( rtTransformDestroy( m_transform ) );
  }

  inline void TransformObj::validate()
  {
    checkError( rtTransformValidate( m_transform ) );
  }

  inline Context TransformObj::getContext()
  {
    RTcontext c;
    checkError( rtTransformGetContext( m_transform, &c) );
    return Context::take(c);
  }

  template< typename T >
  inline void TransformObj::setChild(T child)
  {
    checkError( rtTransformSetChild( m_transform, child->get() ) );
  }

  template< typename T >
  inline T TransformObj::getChild()
  {
    RTobject result;
    checkError( rtTransformGetChild( m_transform, &result) );
    return T::take( result );
  }

  inline void TransformObj::setMatrix(bool transpose, const float* matrix, const float* inverse_matrix)
  {
    rtTransformSetMatrix( m_transform, transpose, matrix, inverse_matrix );
  }

  inline void TransformObj::getMatrix(bool transpose, float* matrix, float* inverse_matrix)
  {
    rtTransformGetMatrix( m_transform, transpose, matrix, inverse_matrix );
  }

  inline RTtransform TransformObj::get()
  {
    return m_transform;
  }

  inline void TransformObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtTransformGetContext( m_transform, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void AccelerationObj::destroy()
  {
    checkError( rtAccelerationDestroy(m_acceleration) );
  }

  inline void AccelerationObj::validate()
  {
    checkError( rtAccelerationValidate(m_acceleration) );
  }

  inline Context AccelerationObj::getContext()
  {
    RTcontext c;
    checkError( rtAccelerationGetContext(m_acceleration, &c ) );
    return Context::take( c );
  }

  inline void AccelerationObj::markDirty()
  {
    checkError( rtAccelerationMarkDirty(m_acceleration) );
  }

  inline bool AccelerationObj::isDirty()
  {
    int dirty;
    checkError( rtAccelerationIsDirty(m_acceleration,&dirty) );
    return dirty != 0;
  }

  inline void AccelerationObj::setProperty( const std::string& name, const std::string& value )
  {
    checkError( rtAccelerationSetProperty(m_acceleration, name.c_str(), value.c_str() ) );
  }

  inline std::string AccelerationObj::getProperty( const std::string& name )
  {
    const char* s;
    checkError( rtAccelerationGetProperty(m_acceleration, name.c_str(), &s ) );
    return std::string( s );
  }

  inline void AccelerationObj::setBuilder(const std::string& builder)
  {
    checkError( rtAccelerationSetBuilder(m_acceleration, builder.c_str() ) );
  }

  inline std::string AccelerationObj::getBuilder()
  {
    const char* s;
    checkError( rtAccelerationGetBuilder(m_acceleration, &s ) );
    return std::string( s );
  }

  inline void AccelerationObj::setTraverser(const std::string& traverser)
  {
    checkError( rtAccelerationSetTraverser(m_acceleration, traverser.c_str() ) );
  }

  inline std::string AccelerationObj::getTraverser()
  {
    const char* s;
    checkError( rtAccelerationGetTraverser(m_acceleration, &s ) );
    return std::string( s );
  }

  inline RTsize AccelerationObj::getDataSize()
  {
    RTsize sz;
    checkError( rtAccelerationGetDataSize(m_acceleration, &sz) );
    return sz;
  }

  inline void AccelerationObj::getData( void* data )
  {
    checkError( rtAccelerationGetData(m_acceleration,data) );
  }

  inline void AccelerationObj::setData( const void* data, RTsize size )
  {
    checkError( rtAccelerationSetData(m_acceleration,data,size) );
  }

  inline RTacceleration AccelerationObj::get()
  {
    return m_acceleration;
  }

  inline void AccelerationObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtAccelerationGetContext( m_acceleration, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void GeometryInstanceObj::destroy()
  {
    checkError( rtGeometryInstanceDestroy( m_geometryinstance ) );
  }

  inline void GeometryInstanceObj::validate()
  {
    checkError( rtGeometryInstanceValidate( m_geometryinstance ) );
  }

  inline Context GeometryInstanceObj::getContext()
  {
    RTcontext c;
    checkError( rtGeometryInstanceGetContext( m_geometryinstance, &c ) );
    return Context::take( c );
  }

  inline void GeometryInstanceObj::setGeometry(Geometry geometry)
  {
    checkError( rtGeometryInstanceSetGeometry( m_geometryinstance, geometry->get() ) );
  }

  inline Geometry GeometryInstanceObj::getGeometry()
  {
    RTgeometry result;
    checkError( rtGeometryInstanceGetGeometry( m_geometryinstance, &result ) );
    return Geometry::take( result );
  }

  inline void GeometryInstanceObj::setMaterialCount(unsigned int  count)
  {
    checkError( rtGeometryInstanceSetMaterialCount( m_geometryinstance, count ) );
  }

  inline unsigned int GeometryInstanceObj::getMaterialCount()
  {
    unsigned int result;
    checkError( rtGeometryInstanceGetMaterialCount( m_geometryinstance, &result ) );
    return result;
  }

  inline void GeometryInstanceObj::setMaterial(unsigned int idx, Material  material)
  {
    checkError( rtGeometryInstanceSetMaterial( m_geometryinstance, idx, material->get()) );
  }

  inline Material GeometryInstanceObj::getMaterial(unsigned int idx)
  {
    RTmaterial result;
    checkError( rtGeometryInstanceGetMaterial( m_geometryinstance, idx, &result ) );
    return Material::take( result );
  }

  // Adds the material and returns the index to the added material.
  inline unsigned int GeometryInstanceObj::addMaterial(Material material)
  {
    unsigned int old_count = getMaterialCount();
    setMaterialCount(old_count+1);
    setMaterial(old_count, material);
    return old_count;
  }

  inline Variable GeometryInstanceObj::declareVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtGeometryInstanceDeclareVariable( m_geometryinstance, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline Variable GeometryInstanceObj::queryVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtGeometryInstanceQueryVariable( m_geometryinstance, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline void GeometryInstanceObj::removeVariable(Variable v)
  {
    checkError( rtGeometryInstanceRemoveVariable( m_geometryinstance, v->get() ) );
  }

  inline unsigned int GeometryInstanceObj::getVariableCount()
  {
    unsigned int result;
    checkError( rtGeometryInstanceGetVariableCount( m_geometryinstance, &result ) );
    return result;
  }

  inline Variable GeometryInstanceObj::getVariable(unsigned int index)
  {
    RTvariable v;
    checkError( rtGeometryInstanceGetVariable( m_geometryinstance, index, &v ) );
    return Variable::take( v );
  }

  inline RTgeometryinstance GeometryInstanceObj::get()
  {
    return m_geometryinstance;
  }

  inline void GeometryInstanceObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtGeometryInstanceGetContext( m_geometryinstance, &c) == RT_SUCCESS? c : 0 );
    }
  }


  inline void GeometryObj::destroy()
  {
    checkError( rtGeometryDestroy( m_geometry ) );
  }

  inline void GeometryObj::validate()
  {
    checkError( rtGeometryValidate( m_geometry ) );
  }

  inline Context GeometryObj::getContext()
  {
    RTcontext c;
    checkError( rtGeometryGetContext( m_geometry, &c ) );
    return Context::take( c );
  }

  inline void GeometryObj::setPrimitiveCount(unsigned int  num_primitives)
  {
    checkError( rtGeometrySetPrimitiveCount( m_geometry, num_primitives ) );
  }

  inline unsigned int GeometryObj::getPrimitiveCount()
  {
    unsigned int result;
    checkError( rtGeometryGetPrimitiveCount( m_geometry, &result ) );
    return result;
  }

  inline void GeometryObj::setBoundingBoxProgram(Program  program)
  {
    checkError( rtGeometrySetBoundingBoxProgram( m_geometry, program->get() ) );
  }

  inline Program GeometryObj::getBoundingBoxProgram()
  {
    RTprogram result;
    checkError( rtGeometryGetBoundingBoxProgram( m_geometry, &result ) );
    return Program::take( result );
  }

  inline void GeometryObj::setIntersectionProgram(Program  program)
  {
    checkError( rtGeometrySetIntersectionProgram( m_geometry, program->get() ) );
  }

  inline Program GeometryObj::getIntersectionProgram()
  {
    RTprogram result;
    checkError( rtGeometryGetIntersectionProgram( m_geometry, &result ) );
    return Program::take( result );
  }

  inline Variable GeometryObj::declareVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtGeometryDeclareVariable( m_geometry, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline Variable GeometryObj::queryVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtGeometryQueryVariable( m_geometry, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline void GeometryObj::removeVariable(Variable v)
  {
    checkError( rtGeometryRemoveVariable( m_geometry, v->get() ) );
  }

  inline unsigned int GeometryObj::getVariableCount()
  {
    unsigned int result;
    checkError( rtGeometryGetVariableCount( m_geometry, &result ) );
    return result;
  }

  inline Variable GeometryObj::getVariable(unsigned int index)
  {
    RTvariable v;
    checkError( rtGeometryGetVariable( m_geometry, index, &v ) );
    return Variable::take( v );
  }

  inline void GeometryObj::markDirty()
  {
    checkError( rtGeometryMarkDirty(m_geometry) );
  }

  inline bool GeometryObj::isDirty()
  {
    int dirty;
    checkError( rtGeometryIsDirty(m_geometry,&dirty) );
    return dirty != 0;
  }

  inline RTgeometry GeometryObj::get()
  {
    return m_geometry;
  }

  inline void GeometryObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtGeometryGetContext( m_geometry, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void MaterialObj::destroy()
  {
    checkError( rtMaterialDestroy( m_material ) );
  }

  inline void MaterialObj::validate()
  {
    checkError( rtMaterialValidate( m_material ) );
  }

  inline Context MaterialObj::getContext()
  {
    RTcontext c;
    checkError( rtMaterialGetContext( m_material, &c ) );
    return Context::take( c );
  }

  inline void MaterialObj::setClosestHitProgram(unsigned int ray_type_index, Program  program)
  {
    checkError( rtMaterialSetClosestHitProgram( m_material, ray_type_index, program->get() ) );
  }

  inline Program MaterialObj::getClosestHitProgram(unsigned int ray_type_index)
  {
    RTprogram result;
    checkError( rtMaterialGetClosestHitProgram( m_material, ray_type_index, &result ) );
    return Program::take( result );
  }

  inline void MaterialObj::setAnyHitProgram(unsigned int ray_type_index, Program  program)
  {
    checkError( rtMaterialSetAnyHitProgram( m_material, ray_type_index, program->get() ) );
  }

  inline Program MaterialObj::getAnyHitProgram(unsigned int ray_type_index)
  {
    RTprogram result;
    checkError( rtMaterialGetAnyHitProgram( m_material, ray_type_index, &result ) );
    return Program::take( result );
  }

  inline Variable MaterialObj::declareVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtMaterialDeclareVariable( m_material, name.c_str(), &v ) );
    return Variable::take( v );
  }

  inline Variable MaterialObj::queryVariable(const std::string& name)
  {
    RTvariable v;
    checkError( rtMaterialQueryVariable( m_material, name.c_str(), &v) );
    return Variable::take( v );
  }

  inline void MaterialObj::removeVariable(Variable v)
  {
    checkError( rtMaterialRemoveVariable( m_material, v->get() ) );
  }

  inline unsigned int MaterialObj::getVariableCount()
  {
    unsigned int result;
    checkError( rtMaterialGetVariableCount( m_material, &result ) );
    return result;
  }

  inline Variable MaterialObj::getVariable(unsigned int index)
  {
    RTvariable v;
    checkError( rtMaterialGetVariable( m_material, index, &v) );
    return Variable::take( v );
  }

  inline RTmaterial MaterialObj::get()
  {
    return m_material;
  }

  inline void MaterialObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtMaterialGetContext( m_material, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void TextureSamplerObj::destroy()
  {
    checkError( rtTextureSamplerDestroy( m_texturesampler ) );
  }

  inline void TextureSamplerObj::validate()
  {
    checkError( rtTextureSamplerValidate( m_texturesampler ) );
  }

  inline Context TextureSamplerObj::getContext()
  {
    RTcontext c;
    checkError( rtTextureSamplerGetContext( m_texturesampler, &c ) );
    return Context::take( c );
  }

  inline void TextureSamplerObj::setMipLevelCount(unsigned int  num_mip_levels)
  {
    checkError( rtTextureSamplerSetMipLevelCount(m_texturesampler, num_mip_levels ) );
  }

  inline unsigned int TextureSamplerObj::getMipLevelCount()
  {
    unsigned int result;
    checkError( rtTextureSamplerGetMipLevelCount( m_texturesampler, &result ) );
    return result;
  }

  inline void TextureSamplerObj::setArraySize(unsigned int  num_textures_in_array)
  {
    checkError( rtTextureSamplerSetArraySize( m_texturesampler, num_textures_in_array ) );
  }

  inline unsigned int TextureSamplerObj::getArraySize()
  {
    unsigned int result;
    checkError( rtTextureSamplerGetArraySize( m_texturesampler, &result ) );
    return result;
  }

  inline void TextureSamplerObj::setWrapMode(unsigned int dim, RTwrapmode wrapmode)
  {
    checkError( rtTextureSamplerSetWrapMode( m_texturesampler, dim, wrapmode ) );
  }

  inline RTwrapmode TextureSamplerObj::getWrapMode(unsigned int dim)
  {
    RTwrapmode wrapmode;
    checkError( rtTextureSamplerGetWrapMode( m_texturesampler, dim, &wrapmode ) );
    return wrapmode;
  }

  inline void TextureSamplerObj::setFilteringModes(RTfiltermode  minification, RTfiltermode  magnification, RTfiltermode  mipmapping)
  {
    checkError( rtTextureSamplerSetFilteringModes( m_texturesampler, minification, magnification, mipmapping ) );
  }

  inline void TextureSamplerObj::getFilteringModes(RTfiltermode& minification, RTfiltermode& magnification, RTfiltermode& mipmapping)
  {
    checkError( rtTextureSamplerGetFilteringModes( m_texturesampler, &minification, &magnification, &mipmapping ) );
  }

  inline void TextureSamplerObj::setMaxAnisotropy(float value)
  {
    checkError( rtTextureSamplerSetMaxAnisotropy(m_texturesampler, value ) );
  }

  inline float TextureSamplerObj::getMaxAnisotropy()
  {
    float result;
    checkError( rtTextureSamplerGetMaxAnisotropy( m_texturesampler, &result) );
    return result;
  }

  inline void TextureSamplerObj::setReadMode(RTtexturereadmode  readmode)
  {
    checkError( rtTextureSamplerSetReadMode( m_texturesampler, readmode ) );
  }

  inline RTtexturereadmode TextureSamplerObj::getReadMode()
  {
    RTtexturereadmode result;
    checkError( rtTextureSamplerGetReadMode( m_texturesampler, &result) );
    return result;
  }

  inline void TextureSamplerObj::setIndexingMode(RTtextureindexmode  indexmode)
  {
    checkError( rtTextureSamplerSetIndexingMode( m_texturesampler, indexmode ) );
  }

  inline RTtextureindexmode TextureSamplerObj::getIndexingMode()
  {
    RTtextureindexmode result;
    checkError( rtTextureSamplerGetIndexingMode( m_texturesampler, &result ) );
    return result;
  }

  inline void TextureSamplerObj::setBuffer(unsigned int texture_array_idx, unsigned int mip_level, Buffer buffer)
  {
    checkError( rtTextureSamplerSetBuffer( m_texturesampler, texture_array_idx, mip_level, buffer->get() ) );
  }

  inline Buffer TextureSamplerObj::getBuffer(unsigned int texture_array_idx, unsigned int mip_level)
  {
    RTbuffer result;
    checkError( rtTextureSamplerGetBuffer(m_texturesampler, texture_array_idx, mip_level, &result ) );
    return Buffer::take(result);
  }

  inline RTtexturesampler TextureSamplerObj::get()
  {
    return m_texturesampler;
  }

  inline void TextureSamplerObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtTextureSamplerGetContext( m_texturesampler, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void TextureSamplerObj::registerGLTexture()
  {
    checkError( rtTextureSamplerGLRegister( m_texturesampler ) );
  }

  inline void TextureSamplerObj::unregisterGLTexture()
  {
    checkError( rtTextureSamplerGLUnregister( m_texturesampler ) );
  }

#ifdef _WIN32

  inline void TextureSamplerObj::registerD3D9Texture()
  {
    checkError( rtTextureSamplerD3D9Register( m_texturesampler ) );
  }

  inline void TextureSamplerObj::registerD3D10Texture()
  {
    checkError( rtTextureSamplerD3D10Register( m_texturesampler ) );
  }

  inline void TextureSamplerObj::registerD3D11Texture()
  {
    checkError( rtTextureSamplerD3D11Register( m_texturesampler ) );
  }

  inline void TextureSamplerObj::unregisterD3D9Texture()
  {
    checkError( rtTextureSamplerD3D9Unregister( m_texturesampler ) );
  }

  inline void TextureSamplerObj::unregisterD3D10Texture()
  {
    checkError( rtTextureSamplerD3D10Unregister( m_texturesampler ) );
  }

  inline void TextureSamplerObj::unregisterD3D11Texture()
  {
    checkError( rtTextureSamplerD3D11Unregister( m_texturesampler ) );
  }

#endif

  inline void BufferObj::destroy()
  {
    checkError( rtBufferDestroy( m_buffer ) );
  }

  inline void BufferObj::validate()
  {
    checkError( rtBufferValidate( m_buffer ) );
  }

  inline Context BufferObj::getContext()
  {
    RTcontext c;
    checkError( rtBufferGetContext( m_buffer, &c ) );
    return Context::take( c );
  }

  inline void BufferObj::setFormat(RTformat format)
  {
    checkError( rtBufferSetFormat( m_buffer, format ) );
  }

  inline RTformat BufferObj::getFormat()
  {
    RTformat result;
    checkError( rtBufferGetFormat( m_buffer, &result ) );
    return result;
  }

  inline void BufferObj::setElementSize(RTsize size_of_element)
  {
    checkError( rtBufferSetElementSize ( m_buffer, size_of_element ) );
  }

  inline RTsize BufferObj::getElementSize()
  {
    RTsize result;
    checkError( rtBufferGetElementSize ( m_buffer, &result) );
    return result;
  }

  inline void BufferObj::setSize(RTsize width)
  {
    checkError( rtBufferSetSize1D( m_buffer, width ) );
  }

  inline void BufferObj::getSize(RTsize& width)
  {
    checkError( rtBufferGetSize1D( m_buffer, &width ) );
  }

  inline void BufferObj::setSize(RTsize width, RTsize height)
  {
    checkError( rtBufferSetSize2D( m_buffer, width, height ) );
  }

  inline void BufferObj::getSize(RTsize& width, RTsize& height)
  {
    checkError( rtBufferGetSize2D( m_buffer, &width, &height ) );
  }

  inline void BufferObj::setSize(RTsize width, RTsize height, RTsize depth)
  {
    checkError( rtBufferSetSize3D( m_buffer, width, height, depth ) );
  }

  inline void BufferObj::getSize(RTsize& width, RTsize& height, RTsize& depth)
  {
    checkError( rtBufferGetSize3D( m_buffer, &width, &height, &depth ) );
  }

  inline void BufferObj::setSize(unsigned int dimensionality, const RTsize* dims)
  {
    checkError( rtBufferSetSizev( m_buffer, dimensionality, dims ) );
  }

  inline void BufferObj::getSize(unsigned int dimensionality, RTsize* dims)
  {
    checkError( rtBufferGetSizev( m_buffer, dimensionality, dims ) );
  }

  inline unsigned int BufferObj::getDimensionality()
  {
    unsigned int result;
    checkError( rtBufferGetDimensionality( m_buffer, &result ) );
    return result;
  }

  inline unsigned int BufferObj::getGLBOId()
  {
    unsigned int result;
    checkError( rtBufferGetGLBOId( m_buffer, &result ) );
    return result;
  }

  inline void BufferObj::registerGLBuffer()
  {
    checkError( rtBufferGLRegister( m_buffer ) );
  }

  inline void BufferObj::unregisterGLBuffer()
  {
    checkError( rtBufferGLUnregister( m_buffer ) );
  }

#ifdef _WIN32

  inline void BufferObj::registerD3D9Buffer()
  {
    checkError( rtBufferD3D9Register( m_buffer ) );
  }

  inline void BufferObj::registerD3D10Buffer()
  {
    checkError( rtBufferD3D10Register( m_buffer ) );
  }

  inline void BufferObj::registerD3D11Buffer()
  {
    checkError( rtBufferD3D11Register( m_buffer ) );
  }

  inline void BufferObj::unregisterD3D9Buffer()
  {
    checkError( rtBufferD3D9Unregister( m_buffer ) );
  }

  inline void BufferObj::unregisterD3D10Buffer()
  {
    checkError( rtBufferD3D10Unregister( m_buffer ) );
  }

  inline void BufferObj::unregisterD3D11Buffer()
  {
    checkError( rtBufferD3D11Unregister( m_buffer ) );
  }

  inline IDirect3DResource9* BufferObj::getD3D9Resource()
  {
    IDirect3DResource9* result = NULL;
    checkError( rtBufferGetD3D9Resource( m_buffer, &result ) );
    return result;
  }
  
  inline ID3D10Resource* BufferObj::getD3D10Resource()
  {
    ID3D10Resource* result = NULL;
    checkError( rtBufferGetD3D10Resource( m_buffer, &result ) );
    return result;
  }
  
  inline ID3D11Resource* BufferObj::getD3D11Resource()
  {
    ID3D11Resource* result = NULL;
    checkError( rtBufferGetD3D11Resource( m_buffer, &result ) );
    return result;
  }

#endif

  inline void* BufferObj::map()
  {
    void* result;
    checkError( rtBufferMap( m_buffer, &result ) );
    return result;
  }

  inline void BufferObj::unmap()
  {
    checkError( rtBufferUnmap( m_buffer ) );
  }


  inline RTbuffer BufferObj::get()
  {
    return m_buffer;
  }

  inline void BufferObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtBufferGetContext( m_buffer, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void VariableObj::setUint(unsigned int u1)
  {
    checkError( rtVariableSet1ui( m_variable, u1 ) );
  }

  inline void VariableObj::setUint(unsigned int u1, unsigned int u2)
  {
    checkError( rtVariableSet2ui( m_variable, u1, u2 ) );
  }

  inline void VariableObj::setUint(unsigned int u1, unsigned int u2, unsigned int u3)
  {
    checkError( rtVariableSet3ui( m_variable, u1, u2, u3 ) );
  }

  inline void VariableObj::setUint(unsigned int u1, unsigned int u2, unsigned int u3, unsigned int u4)
  {
    checkError( rtVariableSet4ui( m_variable, u1, u2, u3, u4 ) );
  }

  inline void VariableObj::set1uiv(const unsigned int* u)
  {
    checkError( rtVariableSet1uiv( m_variable, u ) );
  }

  inline void VariableObj::set2uiv(const unsigned int* u)
  {
    checkError( rtVariableSet2uiv( m_variable, u ) );
  }

  inline void VariableObj::set3uiv(const unsigned int* u)
  {
    checkError( rtVariableSet3uiv( m_variable, u ) );
  }

  inline void VariableObj::set4uiv(const unsigned int* u)
  {
    checkError( rtVariableSet4uiv( m_variable, u ) );
  }

  inline void VariableObj::setMatrix2x2fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix2x2fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix2x3fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix2x3fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix2x4fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix2x4fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix3x2fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix3x2fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix3x3fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix3x3fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix3x4fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix3x4fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix4x2fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix4x2fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix4x3fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix4x3fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setMatrix4x4fv(bool transpose, const float* m)
  {
    checkError( rtVariableSetMatrix4x4fv( m_variable, (int)transpose, m ) );
  }

  inline void VariableObj::setFloat(float f1)
  {
    checkError( rtVariableSet1f( m_variable, f1 ) );
  }

  inline void VariableObj::setFloat(float2 f)
  {
    checkError( rtVariableSet2fv( m_variable, &f.x ) );
  }

  inline void VariableObj::setFloat(float f1, float f2)
  {
    checkError( rtVariableSet2f( m_variable, f1, f2 ) );
  }

  inline void VariableObj::setFloat(float3 f)
  {
    checkError( rtVariableSet3fv( m_variable, &f.x ) );
  }

  inline void VariableObj::setFloat(float f1, float f2, float f3)
  {
    checkError( rtVariableSet3f( m_variable, f1, f2, f3 ) );
  }

  inline void VariableObj::setFloat(float4 f)
  {
    checkError( rtVariableSet4fv( m_variable, &f.x ) );
  }

  inline void VariableObj::setFloat(float f1, float f2, float f3, float f4)
  {
    checkError( rtVariableSet4f( m_variable, f1, f2, f3, f4 ) );
  }

  inline void VariableObj::set1fv(const float* f)
  {
    checkError( rtVariableSet1fv( m_variable, f ) );
  }

  inline void VariableObj::set2fv(const float* f)
  {
    checkError( rtVariableSet2fv( m_variable, f ) );
  }

  inline void VariableObj::set3fv(const float* f)
  {
    checkError( rtVariableSet3fv( m_variable, f ) );
  }

  inline void VariableObj::set4fv(const float* f)
  {
    checkError( rtVariableSet4fv( m_variable, f ) );
  }

  ///////
  inline void VariableObj::setInt(int i1)
  {
    checkError( rtVariableSet1i( m_variable, i1 ) );
  }

  inline void VariableObj::setInt(int2 i)
  {
    checkError( rtVariableSet2iv( m_variable, &i.x ) );
  }

  inline void VariableObj::setInt(int i1, int i2)
  {
    checkError( rtVariableSet2i( m_variable, i1, i2 ) );
  }

  inline void VariableObj::setInt(int3 i)
  {
    checkError( rtVariableSet3iv( m_variable, &i.x ) );
  }

  inline void VariableObj::setInt(int i1, int i2, int i3)
  {
    checkError( rtVariableSet3i( m_variable, i1, i2, i3 ) );
  }

  inline void VariableObj::setInt(int4 i)
  {
    checkError( rtVariableSet4iv( m_variable, &i.x ) );
  }

  inline void VariableObj::setInt(int i1, int i2, int i3, int i4)
  {
    checkError( rtVariableSet4i( m_variable, i1, i2, i3, i4 ) );
  }

  inline void VariableObj::set1iv( const int* i )
  {
    checkError( rtVariableSet1iv( m_variable, i ) );
  }

  inline void VariableObj::set2iv( const int* i )
  {
    checkError( rtVariableSet2iv( m_variable, i ) );
  }

  inline void VariableObj::set3iv( const int* i )
  {
    checkError( rtVariableSet3iv( m_variable, i ) );
  }

  inline void VariableObj::set4iv( const int* i )
  {
    checkError( rtVariableSet4iv( m_variable, i ) );
  }

  inline float VariableObj::getFloat()
  {
    float f;
    checkError( rtVariableGet1f( m_variable, &f ) );
    return f;
  }

  inline unsigned int VariableObj::getUint()
  {
    unsigned int i;
    checkError( rtVariableGet1ui( m_variable, &i ) );
    return i;
  }

  inline int VariableObj::getInt()
  {
    int i;
    checkError( rtVariableGet1i( m_variable, &i ) );
    return i;
  }

  inline void VariableObj::checkError(RTresult code)
  {
    if( code != RT_SUCCESS) {
      RTcontext c;
      throw APIObj::makeException( code, rtVariableGetContext( m_variable, &c) == RT_SUCCESS? c : 0 );
    }
  }

  inline void VariableObj::setBuffer(Buffer buffer)
  {
    checkError( rtVariableSetObject( m_variable, buffer->get() ) );
  }

  inline void VariableObj::set(Buffer buffer)
  {
    checkError( rtVariableSetObject( m_variable, buffer->get() ) );
  }

  inline void VariableObj::setUserData(RTsize size, const void* ptr)
  {
    checkError( rtVariableSetUserData( m_variable, size, ptr ) );
  }

  inline void VariableObj::getUserData(RTsize size,       void* ptr)
  {
    checkError( rtVariableGetUserData( m_variable, size, ptr ) );
  }

  inline void VariableObj::setNode(Node node)
  {
    checkError( rtVariableSetObject( m_variable, node->get() ) );
  }

  inline void VariableObj::set(Node node)
  {
    checkError( rtVariableSetObject( m_variable, node->get() ) );
  }

  inline void VariableObj::setTextureSampler(TextureSampler texturesampler)
  {
    checkError( rtVariableSetObject( m_variable, texturesampler->get() ) );
  }

  inline void VariableObj::set(TextureSampler texturesampler)
  {
    checkError( rtVariableSetObject( m_variable, texturesampler->get() ) );
  }

  inline void VariableObj::set(GeometryGroup group)
  {
    checkError( rtVariableSetObject( m_variable, group->get() ) );
  }

  inline void VariableObj::set(Group group)
  {
    checkError( rtVariableSetObject( m_variable, group->get() ) );
  }

  inline void VariableObj::set(Selector sel)
  {
    checkError( rtVariableSetObject( m_variable, sel->get() ) );
  }

  inline void VariableObj::set(Transform tran)
  {
    checkError( rtVariableSetObject( m_variable, tran->get() ) );
  }

  inline Buffer VariableObj::getBuffer()
  {
    RTobject temp;
    checkError( rtVariableGetObject( m_variable, &temp ) );
    RTbuffer buffer = reinterpret_cast<RTbuffer>(temp);
    return Buffer::take(buffer);
  }

  inline std::string VariableObj::getName()
  {
    const char* name;
    checkError( rtVariableGetName( m_variable, &name ) );
    return std::string(name);
  }

  inline std::string VariableObj::getAnnotation()
  {
    const char* annotation;
    checkError( rtVariableGetAnnotation( m_variable, &annotation ) );
    return std::string(annotation);
  }

  inline RTobjecttype VariableObj::getType()
  {
    RTobjecttype type;
    checkError( rtVariableGetType( m_variable, &type ) );
    return type;
  }

  inline RTvariable VariableObj::get()
  {
    return m_variable;
  }


}

#endif /* __optixu_optixpp_h__ */

