
#include <stdio.h>

#include <windows.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#include "studio.h"
#include "studio_model.h"

extern int g_chrome[MAXSTUDIOVERTS][2];

studiohdr_t* StudioModel::ExposeTextureHeader(void)
{
	return m_ptexturehdr;
}

void StudioModel::ExposeTextureData(void)
{
	float s, t;

	//
	// check for external textures (to load the external header)
	//
	studiohdr_t* header;

	if (m_pstudiohdr->textureindex == 0)
		header = m_ptexturehdr;
	else
		header = m_pstudiohdr;

	mstudiotexture_t* ptexture = (mstudiotexture_t*)((byte*)header + header->textureindex);
	
	for (int j = 0; j < header->numtextures; j++)
	{
		s = 1.0 / (float)ptexture[j].width;
		t = 1.0 / (float)ptexture[j].height;

		glBindTexture(GL_TEXTURE_2D, ptexture[j].index);

		if (!m_filtered)
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0);
		glTexCoord2f(1, 0);
		glTexCoord2f(0, 1);
		glTexCoord2f(1, 1);
		glEnd();
	}
}

int StudioModel::SetSkin(int iValue)
{
	if (iValue < m_pstudiohdr->numskinfamilies)
	{
		return m_skinnum;
	}

	m_skinnum = iValue;

	return iValue;
}

void StudioModel::UploadTexture(mstudiotexture_t* ptexture, byte* data, byte* pal, int g_texnum)
{
	// unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight;
	int outwidth, outheight;
	int		i, j;
	int		row1[256], row2[256], col1[256], col2[256];
	byte* pix1, * pix2, * pix3, * pix4;
	byte* tex, * out;

	// convert texture to power of 2
	for (outwidth = 1; outwidth < ptexture->width; outwidth <<= 1)
		;

	if (outwidth > 256)
		outwidth = 256;

	for (outheight = 1; outheight < ptexture->height; outheight <<= 1)
		;

	if (outheight > 256)
		outheight = 256;

	tex = out = (byte*)malloc(outwidth * outheight * 4);

	if (!out)
		return;

	for (i = 0; i < outwidth; i++)
	{
		col1[i] = (i + 0.25) * (ptexture->width / (float)outwidth);
		col2[i] = (i + 0.75) * (ptexture->width / (float)outwidth);
	}

	for (i = 0; i < outheight; i++)
	{
		row1[i] = (int)((i + 0.25) * (ptexture->height / (float)outheight)) * ptexture->width;
		row2[i] = (int)((i + 0.75) * (ptexture->height / (float)outheight)) * ptexture->width;
	}

	// scale down and convert to 32bit RGB
	for (i = 0; i < outheight; i++)
	{
		for (j = 0; j < outwidth; j++, out += 4)
		{
			pix1 = &pal[data[row1[i] + col1[j]] * 3];
			pix2 = &pal[data[row1[i] + col2[j]] * 3];
			pix3 = &pal[data[row2[i] + col1[j]] * 3];
			pix4 = &pal[data[row2[i] + col2[j]] * 3];

			out[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0]) >> 2;
			out[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1]) >> 2;
			out[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2]) >> 2;
			out[3] = 0xFF;
		}
	}

	glBindTexture(GL_TEXTURE_2D, g_texnum);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, outwidth, outheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	ptexture->index = g_texnum;
	// g_texnum++;

	free(tex);
}

//////////////////////////////////////////////////////////////////

int flag_tex_map_element[STUDIO_NF_NUMFLAG];

int StudioModel::FindTextureFlags(int variable)
{
	int j = 0;

	for (int i = 0; i < STUDIO_NF_NUMFLAG; i++)
	{
		if (variable & flag_tex_map[i].type)
		{
			flag_tex_map_element[j] = i;
			j++;
		}
	}

	return j;
}

#ifndef _A
#define _A( a ) { a, #a }
#endif

flag_map_t flag_tex_map[STUDIO_NF_NUMFLAG] =
{
	_A(STUDIO_NF_FLATSHADE),
	_A(STUDIO_NF_CHROME),
	_A(STUDIO_NF_FULLBRIGHT),
	_A(STUDIO_NF_NOMIPS),
	_A(STUDIO_NF_ALPHA),
	_A(STUDIO_NF_ADDITIVE),
	_A(STUDIO_NF_MASKED)
};


/*

static int index4count = 0;

for (int i = 0; i < NUM_CONTROLLERS; i++)
{
	if (controller->index == 4)
		index4count++;
}

<...>

if (index4count > 1)
	[first index 4 mouth, 2nd regular controller]

*/
