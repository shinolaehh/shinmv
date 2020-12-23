/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/
// updates:
// 1-4-99	fixed file texture load and file read bug
// 2-8-99	fixed demand loaded sequence bug (thanks to Frans 'Otis' Bouma)

////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>

#include "mathlib.h"
#include "studio.h"
#include "studio_model.h"

studiohdr_t *StudioModel::LoadModel( char *modelname )
{
	FILE *fp;
	long size;
	void *buffer;

	// load the model
	if( (fp = fopen( modelname, "rb" )) == NULL)
	{
		printf("unable to open %s\n", modelname );
		exit(1);
	}

	fseek( fp, 0, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	buffer = malloc( size );
	fread( buffer, size, 1, fp );

	int					i;
	byte				*pin;
	studiohdr_t			*phdr;
	mstudiotexture_t	*ptexture;

	pin = (byte *)buffer;
	phdr = (studiohdr_t *)pin;

	ptexture = (mstudiotexture_t *)(pin + phdr->textureindex);
	if (phdr->textureindex != 0)
	{
		for (i = 0; i < phdr->numtextures; i++)
		{
			// strcpy( name, mod->name );
			// strcpy( name, ptexture[i].name );
			UploadTexture( &ptexture[i], pin + ptexture[i].index, pin + ptexture[i].width * ptexture[i].height + ptexture[i].index, i );
		}
	}

	// UNDONE: free texture memory

	fclose( fp );

	return (studiohdr_t *)buffer;
}


studioseqhdr_t *StudioModel::LoadDemandSequences( char *modelname )
{
	FILE *fp;
	long size;
	void *buffer;

	// load the model
	if( (fp = fopen( modelname, "rb" )) == NULL)
	{
		printf("unable to open %s\n", modelname );
		exit(1);
	}

	fseek( fp, 0, SEEK_END );
	size = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	buffer = malloc( size );
	fread( buffer, size, 1, fp );

	fclose( fp );

	return (studioseqhdr_t *)buffer;
}


void StudioModel::Init( char *modelname )
{ 
	m_pstudiohdr = LoadModel( modelname );

	// preload textures
	if (m_pstudiohdr->numtextures == 0)
	{
		char texturename[256];

		strcpy( texturename, modelname );
		strcpy( &texturename[strlen(texturename) - 4], "T.mdl" );

		m_ptexturehdr = LoadModel( texturename );
	}
	else
	{
		m_ptexturehdr = m_pstudiohdr;
	}

	// preload animations
	if (m_pstudiohdr->numseqgroups > 1)
	{
		for (int i = 1; i < m_pstudiohdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy( seqgroupname, modelname );
			sprintf( &seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i );

			m_panimhdr[i] = LoadDemandSequences( seqgroupname );
		}
	}
}


////////////////////////////////////////////////////////////////////////

int StudioModel::GetSequence( )
{
	return m_sequence;
}

int StudioModel::SetSequence( int iSequence )
{
	if (iSequence > m_pstudiohdr->numseq)
		iSequence = 0;
	if (iSequence < 0)
		iSequence = m_pstudiohdr->numseq-1;

	m_sequence = iSequence;
	m_frame = 0;

	return m_sequence;
}


void StudioModel::ExtractBbox( float *mins, float *maxs )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);
	
	mins[0] = pseqdesc[ m_sequence ].bbmin[0];
	mins[1] = pseqdesc[ m_sequence ].bbmin[1];
	mins[2] = pseqdesc[ m_sequence ].bbmin[2];

	maxs[0] = pseqdesc[ m_sequence ].bbmax[0];
	maxs[1] = pseqdesc[ m_sequence ].bbmax[1];
	maxs[2] = pseqdesc[ m_sequence ].bbmax[2];
}



void StudioModel::GetSequenceInfo( float *pflFrameRate, float *pflGroundSpeed )
{
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}


float StudioModel::SetController( int iController, float flValue )
{
	int i;
	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the index
	for (i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= m_pstudiohdr->numbonecontrollers)
		return flValue;

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = 255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;
	m_controller[iController] = setting;

	return setting * (1.0 / 255.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float StudioModel::SetMouth( float flValue )
{
	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the mouth
	for (int i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == 4)
			break;
	}

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = 64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 64) setting = 64;
	m_mouth = setting;

	return setting * (1.0 / 64.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}


float StudioModel::SetBlending( int iBlender, float flValue )
{
#if 0
	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		// does the controller not wrap?
		if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
				flValue = flValue + 360;
		}
	}

	int setting = 255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	m_blending[iBlender] = setting;

	return setting * (1.0 / 255.0) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
#endif

	m_blending[iBlender] = RoundBlend(iBlender, flValue, false);
	return RoundBlend(iBlender, flValue, true);
}



int StudioModel::SetBodygroup( int iGroup, int iValue )
{
	if (iGroup > m_pstudiohdr->numbodyparts)
		return -1;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + iGroup;

	int iCurrent = (m_bodynum / pbodypart->base) % pbodypart->nummodels;

	if (iValue >= pbodypart->nummodels)
		return iCurrent;

	m_bodynum = (m_bodynum - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));

	return iValue;
}


int StudioModel::MaxSequence(void)
{
	return m_pstudiohdr->numseq - 1;
}


mstudioseqdesc_t* StudioModel::ExposeSequence(void)
{
	mstudioseqdesc_t* pseqdesc = (mstudioseqdesc_t*)((byte*)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;
	return pseqdesc;
}


studiohdr_t* StudioModel::ExposeHeader(void)
{
	return m_pstudiohdr;
}

mstudiomodel_t* StudioModel::ExposeInfo(void)
{
	return m_pmodel;
}

int StudioModel::MaxBlend(void)
{
	mstudioseqdesc_t* pseqdesc = (mstudioseqdesc_t*)((byte*)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	return (pseqdesc->numblends - 1);
}


byte* StudioModel::GetBlendInfo(void)
{
	return m_blending;
}

float StudioModel::RoundBlend(int iBlender, float flValue, bool bDegrees)
{
	mstudioseqdesc_t* pseqdesc = (mstudioseqdesc_t*)((byte*)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;
	
	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		// does the controller not wrap?
		if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
				flValue = flValue + 360;
		}
	}

	int setting = 255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	if (bDegrees)
		return setting * (1.0 / 255.0) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
	else
		return setting;
}

float StudioModel::RoundController(int iController, float flValue, bool bDegrees)
{
	int i;
	mstudiobonecontroller_t* pbonecontroller = (mstudiobonecontroller_t*)((byte*)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	// find first controller that matches the index
	for (i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= m_pstudiohdr->numbonecontrollers)
		return flValue;

	// wrap 0..360 if it's a rotational controller
	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = 255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	if (bDegrees)
		return setting * (1.0 / 255.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
	else
		return setting;
}

//////////////////////////////////////////////////////////////////

int flag_map_element[STUDIO_NUMFLAG];

int StudioModel::FindFrameMotionFlags(int variable)
{
	int j = 0;

	for (int i = 0; i < STUDIO_NUMFLAG; i++)
	{
		if (variable & flag_map[i].type)
		{
			flag_map_element[j] = i;
			j++;
		}
	}

	return j;
}

#ifndef _A
#define _A( a ) { a, #a }
#endif

flag_map_t flag_map[STUDIO_NUMFLAG] =
{
	_A(STUDIO_X), _A(STUDIO_Y), _A(STUDIO_Z),
	_A(STUDIO_XR), _A(STUDIO_YR), _A(STUDIO_ZR),
	_A(STUDIO_LX), _A(STUDIO_LY), _A(STUDIO_LZ),
	_A(STUDIO_AX), _A(STUDIO_AY), _A(STUDIO_AZ),
	_A(STUDIO_AXR), _A(STUDIO_AYR), _A(STUDIO_AZR),

	_A(STUDIO_TYPES),
	_A(STUDIO_RLOOP),
};
