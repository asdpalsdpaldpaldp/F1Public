#pragma once

#include "steam/SteamTypes.h"
#include "baseHeaders.h"

#include "Vector.h"

//-----------------------------------------------------------------------------
// forward declaraions
//-----------------------------------------------------------------------------

struct Ray_t;
class KeyValues;
class IMaterialVar;
class ITexture;
class IMaterialProxy;
class Vector;

// undefined types
class MorphFormat_t;
class VertexFormat_t;
class MaterialPropertyTypes_t;

typedef float matrix3x4[3][4];

#define MAXSTUDIOSKINS	32	// total textures


//-----------------------------------------------------------------------------
// Shader state flags can be read from the FLAGS materialvar
// Also can be read or written to with the Set/GetMaterialVarFlags() call
// Also make sure you add/remove a string associated with each flag below to CShaderSystem::ShaderStateString in ShaderSystem.cpp
//-----------------------------------------------------------------------------
enum class MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

enum class ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

								// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

	#if defined( _X360 )
								// Depth-stencil texture formats
	IMAGE_FORMAT_X360_DST16,
	IMAGE_FORMAT_X360_DST24,
	IMAGE_FORMAT_X360_DST24F,
	// supporting these specific formats as non-tiled for procedural cpu access
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,
	#endif

	IMAGE_FORMAT_DXT1_RUNTIME,
	IMAGE_FORMAT_DXT5_RUNTIME,

	NUM_IMAGE_FORMATS
};



//-----------------------------------------------------------------------------
// Internal flags not accessible from outside the material system. Stored in Flags2
//-----------------------------------------------------------------------------
enum MaterialVarFlags2_t
{
	// NOTE: These are for $flags2!!!!!
	//	UNUSED											= (1 << 0),

	MATERIAL_VAR2_LIGHTING_UNLIT = 0,
	MATERIAL_VAR2_LIGHTING_VERTEX_LIT = (1 << 1),
	MATERIAL_VAR2_LIGHTING_LIGHTMAP = (1 << 2),
	MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP = (1 << 3),
	MATERIAL_VAR2_LIGHTING_MASK =
	(MATERIAL_VAR2_LIGHTING_VERTEX_LIT |
		MATERIAL_VAR2_LIGHTING_LIGHTMAP |
		MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP),

	// FIXME: Should this be a part of the above lighting enums?
	MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL = (1 << 4),
	MATERIAL_VAR2_USES_ENV_CUBEMAP = (1 << 5),
	MATERIAL_VAR2_NEEDS_TANGENT_SPACES = (1 << 6),
	MATERIAL_VAR2_NEEDS_SOFTWARE_LIGHTING = (1 << 7),
	// GR - HDR path puts lightmap alpha in separate texture...
	MATERIAL_VAR2_BLEND_WITH_LIGHTMAP_ALPHA = (1 << 8),
	MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS = (1 << 9),
	MATERIAL_VAR2_USE_FLASHLIGHT = (1 << 10),
	MATERIAL_VAR2_USE_FIXED_FUNCTION_BAKED_LIGHTING = (1 << 11),
	MATERIAL_VAR2_NEEDS_FIXED_FUNCTION_FLASHLIGHT = (1 << 12),
	MATERIAL_VAR2_USE_EDITOR = (1 << 13),
	MATERIAL_VAR2_NEEDS_POWER_OF_TWO_FRAME_BUFFER_TEXTURE = (1 << 14),
	MATERIAL_VAR2_NEEDS_FULL_FRAME_BUFFER_TEXTURE = (1 << 15),
	MATERIAL_VAR2_IS_SPRITECARD = (1 << 16),
	MATERIAL_VAR2_USES_VERTEXID = (1 << 17),
	MATERIAL_VAR2_SUPPORTS_HW_SKINNING = (1 << 18),
	MATERIAL_VAR2_SUPPORTS_FLASHLIGHT = (1 << 19),
};


//-----------------------------------------------------------------------------
// Preview image return values
//-----------------------------------------------------------------------------
enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};


//-----------------------------------------------------------------------------
// material interface
//-----------------------------------------------------------------------------
class IMaterial
{
public:
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char *	GetName() const = 0;
	virtual const char *	GetTextureGroupName() const = 0;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height,
		ImageFormat *imageFormat, bool* isTranslucent) const = 0;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data,
		int width, int height,
		ImageFormat imageFormat) const = 0;
	// 
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;

	virtual int				GetNumAnimationFrames() = 0;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage(void) = 0;
	virtual	void			GetMaterialOffset(float *pOffset) = 0;
	virtual void			GetMaterialScale(float *pScale) = 0;
	virtual IMaterial		*GetMaterialPage(void) = 0;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar *	FindVar(const char *varName, bool *found, bool complain = true) = 0;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount(void) = 0;
	virtual void			DecrementReferenceCount(void) = 0;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID(void) const = 0;

	virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots() = 0;

	// Are we translucent?
	virtual bool			IsTranslucent() = 0;

	// Are we alphatested?
	virtual bool			IsAlphaTested() = 0;

	// Are we vertex lit?
	virtual bool			IsVertexLit() = 0;

	// Gets the vertex format
	virtual VertexFormat_t	GetVertexFormat() const = 0;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy(void) const = 0;

	virtual bool			UsesEnvCubemap(void) = 0;

	virtual bool			NeedsTangentSpace(void) = 0;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning(void) = 0;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;

	// Material Var flags...
	virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
	virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector& reflect) = 0;

	// Gets material property flags
	virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided() = 0;

	// Sets the shader associated with the material
	virtual void			SetShader(const char *pShaderName) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses(void) = 0;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes(void) = 0;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh() = 0;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha(void) = 0;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting(void) = 0;

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const = 0;
	virtual IMaterialVar	**GetShaderParams(void) = 0;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const = 0;

	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float *r, float *g, float *b) = 0;

	// Gets the morph format
	virtual MorphFormat_t	GetMorphFormat() const = 0;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar *	FindVarFast(char const *pVarName, unsigned int *pToken) = 0;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(KeyValues *pKeyValues) = 0;
	virtual const char *	GetShaderName() const = 0;

	virtual void			DeleteIfUnreferenced() = 0;

	virtual bool			IsSpriteCard() = 0;

	virtual void			CallBindProxy(void *proxyData) = 0;

	virtual IMaterial		*CheckProxyReplacement(void *proxyData) = 0;

	virtual void			RefreshPreservingMaterialVars() = 0;

	virtual bool			WasReloadedFromWhitelist() = 0;

	virtual bool			IsPrecached() const = 0;
};


inline bool IsErrorMaterial(IMaterial *pMat)
{
	return !pMat || pMat->IsErrorMaterial();
}

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct model_t;
class IClientRenderable;
class Vector;
struct studiohdr_t;
class IMaterial;
class CStudioHdr;

typedef unsigned short ModelInstanceHandle_t;

enum
{
	MODEL_INSTANCE_INVALID = (ModelInstanceHandle_t)~0
};

struct ModelRenderInfo_t
{
	Vector origin;
	Vector angles;
	IClientRenderable *pRenderable;
	const model_t *pModel;
	const matrix3x4 *pModelToWorld;
	const matrix3x4 *pLightingOffset;
	const Vector *pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

struct StaticPropRenderInfo_t
{
	const matrix3x4			*pModelToWorld;
	const model_t			*pModel;
	IClientRenderable		*pRenderable;
	Vector					*pLightingOrigin;
	short					skin;
	ModelInstanceHandle_t	instance;
};

enum class OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

// These are given to FindMaterial to reference the texture groups that show up on the 
#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"				// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"	// Textures that weren't assigned a texture group.
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"


// get from engine.dll
class CModelRender
{
public:
	void ForcedMaterialOverride(IMaterial *mat, OverrideType_t type)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, IMaterial *, OverrideType_t);
		return getvfunc<OriginalFn>(this, 1)(this, mat, type);
	}

	void DrawModelExecute(void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, void *, ModelRenderInfo_t &, matrix3x4 *);
		return getvfunc<OriginalFn>(this, 19)(this, state, pInfo, pCustomBoneToWorld);
	}
};

// get from materialsystem.dll
class CMaterialSystem
{
public:
	IMaterial *FindMaterial(char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = NULL)
	{
		typedef IMaterial *(__thiscall *OriginalFn)(PVOID, const char *, const char *, bool, const char *);
		return getvfunc<OriginalFn>(this, 71)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}
};

class CRenderView
{
public:

	void SetColorModulation(float *col)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, float *);
		return getvfunc<OriginalFn>(this, 7)(this, col);
	}
};

struct studiohdr_t
{
	int					id;
	int					version;

	int					checksum;		// this has to be the same in the phy and vtx files to load!

	//inline const char *	pszName(void) const { if(studiohdr2index && pStudioHdr2()->pszName()) return pStudioHdr2()->pszName(); else return name; }
	char				name[64];
	int					length;


	Vector				eyeposition;	// ideal eye position

	Vector				illumposition;	// illumination center

	Vector				hull_min;		// ideal movement hull size
	Vector				hull_max;

	Vector				view_bbmin;		// clipping bounding box
	Vector				view_bbmax;

	int					flags;

	int					numbones;			// bones
	int					boneindex;
	//inline void *pBone(int i) const { Assert(i >= 0 && i < numbones); return (void *)(((byte *)this) + boneindex) + i; };
	int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone
	int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;
	//inline void *pBonecontroller(int i) const { Assert(i >= 0 && i < numbonecontrollers); return (void *)(((byte *)this) + bonecontrollerindex) + i; };

	int					numhitboxsets;
	int					hitboxsetindex;

	// Look up hitbox set by index
	//void	*pHitboxSet(int i) const
	//{
	//	Assert(i >= 0 && i < numhitboxsets);
	//	return (void *)(((byte *)this) + hitboxsetindex) + i;
	//};

	// Calls through to hitbox to determine size of specified set
	//inline void *pHitbox(int i, int set) const
	//{
	//	mstudiohitboxset_t const *s = pHitboxSet(set);
	//	if(!s)
	//		return NULL;

	//	return s->pHitbox(i);
	//};

	// Calls through to set to get hitbox count for set
	//inline int			iHitboxCount(int set) const
	//{
	//	mstudiohitboxset_t const *s = pHitboxSet(set);
	//	if(!s)
	//		return 0;

	//	return s->numhitboxes;
	//};

	// file local animations? and sequences
	//private:
	int					numlocalanim;			// animations/poses
	int					localanimindex;		// animation descriptions
	//inline mstudioanimdesc_t *pLocalAnimdesc(int i) const { if(i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t *)(((byte *)this) + localanimindex) + i; };

	int					numlocalseq;				// sequences
	int					localseqindex;
	//inline mstudioseqdesc_t *pLocalSeqdesc(int i) const { if(i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + localseqindex) + i; };

	//public:
	bool				SequencesAvailable() const;
	int					GetNumSeq() const;
	mstudioanimdesc_t	&pAnimdesc(int i) const;
	mstudioseqdesc_t	&pSeqdesc(int i) const;
	int					iRelativeAnim(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

																			//private:
	mutable int			activitylistversion;	// initialization flag - have the sequences been indexed?
	mutable int			eventsindexed;
	//public:
	int					GetSequenceActivity(int iSequence);
	void				SetSequenceActivity(int iSequence, int iActivity);
	int					GetActivityListVersion(void);
	void				SetActivityListVersion(int version) const;
	int					GetEventListVersion(void);
	void				SetEventListVersion(int version);

	// raw textures
	int					numtextures;
	int					textureindex;
	//inline void *pTexture(int i) const { Assert(i >= 0 && i < numtextures); return (void *)(((byte *)this) + textureindex) + i; };


	// raw textures search paths
	int					numcdtextures;
	int					cdtextureindex;
	inline char			*pCdtexture(int i) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

	// replaceable textures tables
	int					numskinref;
	int					numskinfamilies;
	int					skinindex;
	inline short		*pSkinref(int i) const { return (short *)(((byte *)this) + skinindex) + i; };

	int					numbodyparts;
	int					bodypartindex;
	//inline void	*pBodypart(int i) const { return (void *)(((byte *)this) + bodypartindex) + i; };

	// queryable attachable points
	//private:
	int					numlocalattachments;
	int					localattachmentindex;
	//inline void	*pLocalAttachment(int i) const { Assert(i >= 0 && i < numlocalattachments); return (void *)(((byte *)this) + localattachmentindex) + i; };
	//public:
	int					GetNumAttachments(void) const;
	//const mstudioattachment_t &pAttachment(int i) const;
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	// animation node to animation node transition graph
	//private:
	int					numlocalnodes;
	int					localnodeindex;
	int					localnodenameindex;
	inline char			*pszLocalNodeName(int iNode) const { Assert(iNode >= 0 && iNode < numlocalnodes); return (((char *)this) + *((int *)(((byte *)this) + localnodenameindex) + iNode)); }
	inline byte			*pLocalTransition(int i) const { Assert(i >= 0 && i < (numlocalnodes * numlocalnodes)); return (byte *)(((byte *)this) + localnodeindex) + i; };

	//public:
	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char				*pszNodeName(int iNode);
	int					GetTransition(int iFrom, int iTo) const;

	int					numflexdesc;
	int					flexdescindex;
	//inline void *pFlexdesc(int i) const { Assert(i >= 0 && i < numflexdesc); return (void *)(((byte *)this) + flexdescindex) + i; };

	int					numflexcontrollers;
	int					flexcontrollerindex;
	//inline void *pFlexcontroller(void i) const { Assert(numflexcontrollers == 0 || (i >= 0 && i < numflexcontrollers)); return (void *)(((byte *)this) + flexcontrollerindex) + i; };

	int					numflexrules;
	int					flexruleindex;
	//inline void *pFlexRule(int i) const { Assert(i >= 0 && i < numflexrules); return (void *)(((byte *)this) + flexruleindex) + i; };

	int					numikchains;
	int					ikchainindex;
	//inline void *pIKChain(int i) const { Assert(i >= 0 && i < numikchains); return (void *)(((byte *)this) + ikchainindex) + i; };

	int					nummouths;
	int					mouthindex;
	//inline void *pMouth(int i) const { Assert(i >= 0 && i < nummouths); return (void *)(((byte *)this) + mouthindex) + i; };

	//private:
	int					numlocalposeparameters;
	int					localposeparamindex;
	//inline void *pLocalPoseParameter(int i) const { Assert(i >= 0 && i < numlocalposeparameters); return (void *)(((byte *)this) + localposeparamindex) + i; };
	//public:
	int					GetNumPoseParameters(void) const;
	//const void &pPoseParameter(int i);
	int					GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int					surfacepropindex;
	inline char * const pszSurfaceProp(void) const { return ((char *)this) + surfacepropindex; }

	// Key values
	int					keyvalueindex;
	int					keyvaluesize;
	inline const char * KeyValueText(void) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

	int					numlocalikautoplaylocks;
	int					localikautoplaylockindex;
	//inline void *pLocalIKAutoplayLock(int i) const { Assert(i >= 0 && i < numlocalikautoplaylocks); return (void *)(((byte *)this) + localikautoplaylockindex) + i; };

	int					GetNumIKAutoplayLocks(void) const;
	//const void &pIKAutoplayLock(int i);
	int					CountAutoplaySequences() const;
	int					CopyAutoplaySequences(unsigned short *pOut, int outCount) const;
	int					GetAutoplayList(unsigned short **pOut) const;

	// The collision model mass that jay wanted
	float				mass;
	int					contents;

	// external animations, models, etc.
	int					numincludemodels;
	int					includemodelindex;
	//inline void *pModelGroup(int i) const { Assert(i >= 0 && i < numincludemodels); return (void *)(((byte *)this) + includemodelindex) + i; };
	// implementation specific call to get a named model
	const studiohdr_t	*FindModel(void **cache, char const *modelname) const;

	// implementation specific back pointer to virtual data
	mutable void		*virtualModel;
	void		*GetVirtualModel(void) const;

	// for demand loaded animation blocks
	int					szanimblocknameindex;
	inline char * const pszAnimBlockName(void) const { return ((char *)this) + szanimblocknameindex; }
	int					numanimblocks;
	int					animblockindex;
	//inline void *pAnimBlock(int i) const { Assert(i > 0 && i < numanimblocks); return (void *)(((byte *)this) + animblockindex) + i; };
	mutable void		*animblockModel;
	byte *				GetAnimBlock(int i) const;

	int					bonetablebynameindex;
	inline const byte	*GetBoneTableSortedByName() const { return (byte *)this + bonetablebynameindex; }

	// used by tools only that don't cache, but persist mdl's peer data
	// engine uses virtualModel to back link to cache pointers
	void				*pVertexBase;
	void				*pIndexBase;

	// if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
	// this value is used to calculate directional components of lighting 
	// on static props
	byte				constdirectionallightdot;

	// set during load of mdl data to track *desired* lod configuration (not actual)
	// the *actual* clamped root lod is found in studiohwdata
	// this is stored here as a global store to ensure the staged loading matches the rendering
	byte				rootLOD;

	// set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
	// to be set as root LOD:
	//	numAllowedRootLODs = 0	means no restriction, any lod can be set as root lod.
	//	numAllowedRootLODs = N	means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
	byte				numAllowedRootLODs;

	byte				unused[1];

	int					unused4; // zero out if version < 47

	int					numflexcontrollerui;
	int					flexcontrolleruiindex;
	//void *pFlexControllerUI(int i) const { Assert(i >= 0 && i < numflexcontrollerui); return (void *)(((byte *)this) + flexcontrolleruiindex) + i; }

	float				flVertAnimFixedPointScale;
	//inline float		VertAnimFixedPointScale() const { return (flags & STUDIOHDR_FLAGS_VERT_ANIM_FIXED_POINT_SCALE) ? flVertAnimFixedPointScale : 1.0f / 4096.0f; }

	int					unused3[1];

	// FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
	int					studiohdr2index;
	void*		pStudioHdr2() const { return (void *)(((byte *)this) + studiohdr2index); }

	// Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
	//int					NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->void : 0; }
	//const void* SrcBoneTransform(int i) const { Assert(i >= 0 && i < NumSrcBoneTransforms()); return (void *)(((byte *)this) + pStudioHdr2()->void) + i; }

	//inline int			IllumPositionAttachmentIndex() const { return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0; }

	//inline float		MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; } // default to cos(30) if not set

	//inline mstudiolinearbone_t *pLinearBones() const { return studiohdr2index ? pStudioHdr2()->pLinearBones() : NULL; }

	//inline int			BoneFlexDriverCount() const { return studiohdr2index ? pStudioHdr2()->m_nBoneFlexDriverCount : 0; }
	//inline const mstudioboneflexdriver_t* BoneFlexDriver(int i) const { Assert(i >= 0 && i < BoneFlexDriverCount()); return studiohdr2index ? pStudioHdr2()->pBoneFlexDriver(i) : NULL; }

	// NOTE: No room to add stuff? Up the .mdl file format version 
	// [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
	// or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
	int					unused2[1];

	studiohdr_t() {}

private:
	// No copy constructors allowed
	studiohdr_t(const studiohdr_t& vOther);

	friend struct virtualmodel_t;
};
