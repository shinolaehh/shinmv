/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/

#ifndef _STUDIO_MODEL_H_
#define _STUDIO_MODEL_H_

#include "mathlib.h"

class StudioModel
{
public:
	void					Init( char *modelname );
	void					DrawModel( void );
	void					ExtractBbox( float *mins, float *maxs );

	studiohdr_t*			ExposeHeader( void );
	mstudiomodel_t*			ExposeInfo( void );

	int						SetSequence( int iSequence );
	int						GetSequence( void );
	void					GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed );
	mstudioseqdesc_t*		ExposeSequence( void );
	int						MaxSequence( void );

	mstudioevent_t*			GetEventInfo( int iNumber );
	int						MaxEvent( void );
	bool					FindEvent( int event );

	float					SetBlending( int iBlender, float flValue );
	byte*					GetBlendInfo( void );
	int						MaxBlend( void );
	float					RoundBlend( int iBlender, float flValue, bool bDegrees );

	int						SetFrame( int iFrame );
	float					GetFrame( void );
	void					AdvanceFrame( float dt );
	void					AdvanceFrameFrom( float dt, float frame );
	bool					IsFramePaused( void );
	void					ModifyPause( void );
	int						FindFrameMotionFlags( int variable );

	bool					IsFiltered( void );
	void					ModifyFilter( void );

	float					SetController( int iController, float flValue );
	float					RoundController( int iController, float flValue, bool bDegrees );
	float					SetMouth( float flValue );
	int						SetBodygroup( int iGroup, int iValue );
	int						SetSkin( int iValue );

	void					DrawTexture( int iTexture );

private:
	// entity settings
	vec3_t					m_origin;
	vec3_t					m_angles;	
	int						m_sequence;			// sequence index
	float					m_frame;			// frame
	int						m_bodynum;			// bodypart selection	
	int						m_skinnum;			// skin group selection
	byte					m_controller[4];	// bone controllers
	byte					m_blending[2];		// animation blending
	byte					m_mouth;			// mouth position

	// new entity settings
	bool					m_framepaused = false;
	bool					m_filtered = true;

	// internal data
	studiohdr_t				*m_pstudiohdr;
	mstudiomodel_t			*m_pmodel;

	studiohdr_t				*m_ptexturehdr;
	studioseqhdr_t			*m_panimhdr[32];

	vec4_t					m_adj;				// FIX: non persistant, make static

	studiohdr_t				*LoadModel( char *modelname );
	studioseqhdr_t			*LoadDemandSequences( char *modelname );

	void					CalcBoneAdj( void );
	void					CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q );
	void					CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos );
	void					CalcRotations ( vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f );
	mstudioanim_t			*GetAnim( mstudioseqdesc_t *pseqdesc );
	void					SlerpBones( vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s );
	void					SetUpBones ( void );

	void					DrawPoints( void );

	void					Lighting (float *lv, int bone, int flags, vec3_t normal);
	void					Chrome (int *chrome, int bone, vec3_t normal);

	void					SetupLighting( void );

	void					SetupModel ( int bodypart );

	void					UploadTexture( mstudiotexture_t *ptexture, byte *data, byte *pal );
};

extern vec3_t g_vright;		// needs to be set to viewer's right in order for chrome to work
extern float g_lambert;		// modifier for pseudo-hemispherical lighting

#endif
