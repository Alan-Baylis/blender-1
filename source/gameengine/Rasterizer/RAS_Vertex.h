/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contributor(s): Tristan Porteries.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file RAS_Vertex.h
 *  \ingroup bgerast
 */

#ifndef __RAS_TEXVERT_H__
#define __RAS_TEXVERT_H__

#include "RAS_VertexData.h"

#include "mathfu.h"

#include "BLI_math_vector.h"

/// Struct used to pass the vertex format to functions.
struct RAS_VertexFormat
{
	uint8_t uvSize;
	uint8_t colorSize;
};

/// Operators used to compare the contents (uv size, color size, ...) of two vertex formats.
bool operator== (const RAS_VertexFormat& format1, const RAS_VertexFormat& format2);
bool operator!= (const RAS_VertexFormat& format1, const RAS_VertexFormat& format2);

class RAS_VertexInfo
{
public:
	enum {
		FLAT = 1,
	};

private:
	unsigned int m_origindex;
	short m_softBodyIndex;
	uint8_t m_flag;

public:
	RAS_VertexInfo(unsigned int origindex, bool flat);
	~RAS_VertexInfo();

	inline const unsigned int GetOrigIndex() const
	{
		return m_origindex;
	}

	inline short int GetSoftBodyIndex() const
	{
		return m_softBodyIndex;
	}

	inline void SetSoftBodyIndex(short int sbIndex)
	{
		m_softBodyIndex = sbIndex;
	}

	inline const uint8_t GetFlag() const
	{
		return m_flag;
	}

	inline void SetFlag(const uint8_t flag)
	{
		m_flag = flag;
	}
};

class RAS_Vertex
{
public:
	enum {
		MAX_UNIT = 8
	};

private:
	RAS_IVertexData *m_data;
	RAS_VertexFormat m_format;

	inline float *GetUvInternal(const unsigned short index) const
	{
		return (float *)(intptr_t(m_data) + (sizeof(RAS_VertexDataBasic) + sizeof(float[2]) * index));
	}

	inline unsigned int *GetColorInternal(const unsigned short index) const
	{
		return (unsigned int *)(intptr_t(m_data) + (sizeof(RAS_VertexDataBasic) + sizeof(float[2]) * m_format.uvSize + sizeof(unsigned int) * index));
	}

public:
	RAS_Vertex(RAS_IVertexData *data, const RAS_VertexFormat& format)
		:m_data(data),
		m_format(format)
	{
	}

	~RAS_Vertex()
	{
	}

	inline RAS_IVertexData *GetData() const
	{
		return m_data;
	}

	inline const RAS_VertexFormat& GetFormat() const
	{
		return m_format;
	}

	inline const float (&GetXYZ() const)[3]
	{
		return m_data->position;
	}

	inline const float (&GetNormal() const)[3]
	{
		return m_data->normal;
	}

	inline const float (&GetTangent() const)[4]
	{
		return m_data->tangent;
	}

	inline mt::vec3 xyz() const
	{
		return mt::vec3(m_data->position);
	}

	inline void SetXYZ(const mt::vec3& xyz)
	{
		xyz.Pack(m_data->position);
	}

	inline void SetXYZ(const float xyz[3])
	{
		copy_v3_v3(m_data->position, xyz);
	}

	inline void SetNormal(const mt::vec3& normal)
	{
		normal.Pack(m_data->normal);
	}

	inline void SetNormal(const float normal[3])
	{
		copy_v3_v3(m_data->normal, normal);
	}

	inline void SetTangent(const mt::vec4& tangent)
	{
		tangent.Pack(m_data->tangent);
	}

	inline void SetTangent(const float tangent[4])
	{
		copy_v4_v4(m_data->tangent, tangent);
	}

	inline const float (&GetUv(const int index) const)[2]
	{
		return reinterpret_cast<float (&)[2]>(*GetUvInternal(index));
	}

	inline void SetUV(const int index, const mt::vec2& uv)
	{
		uv.Pack(GetUvInternal(index));
	}

	inline void SetUV(const int index, const float uv[2])
	{
		copy_v2_v2(GetUvInternal(index), uv);
	}

	inline const unsigned char (&GetColor(const int index) const)[4]
	{
		return reinterpret_cast<const unsigned char (&)[4]>(*GetColorInternal(index));
	}

	inline const unsigned int GetRawColor(const int index) const
	{
		return *GetColorInternal(index);
	}

	inline void SetColor(const int index, const unsigned int color)
	{
		*GetColorInternal(index) = color;
	}

	inline void SetColor(const int index, const mt::vec4& color)
	{
		unsigned char *colp = (unsigned char *)GetColorInternal(index);
		colp[0] = (unsigned char)(color[0] * 255.0f);
		colp[1] = (unsigned char)(color[1] * 255.0f);
		colp[2] = (unsigned char)(color[2] * 255.0f);
		colp[3] = (unsigned char)(color[3] * 255.0f);
	}

	// compare two vertices, to test if they can be shared, used for
	// splitting up based on uv's, colors, etc
	inline const bool CloseTo(const RAS_Vertex& other)
	{
		BLI_assert(m_format == other.GetFormat());
		static const float eps = FLT_EPSILON;
		for (int i = 0, size = m_format.uvSize; i < size; ++i) {
			if (!compare_v2v2(GetUv(i), other.GetUv(i), eps)) {
				return false;
			}
		}

		for (int i = 0, size = m_format.colorSize; i < size; ++i) {
			if (GetRawColor(i) != other.GetRawColor(i)) {
				return false;
			}
		}

		return (/* m_flag == other->m_flag && */
				/* at the moment the face only stores the smooth/flat setting so don't bother comparing it */
				compare_v3v3(m_data->normal, other.m_data->normal, eps) &&
				compare_v3v3(m_data->tangent, other.m_data->tangent, eps)
				/* don't bother comparing m_data->position since we know there from the same vert */
				/* && compare_v3v3(m_data->position, other->m_data->position, eps))*/
				);
	}

	inline void Transform(const mt::mat4& mat, const mt::mat4& nmat)
	{
		SetXYZ(mat * mt::vec3(m_data->position));
		SetNormal(nmat * mt::vec3(m_data->normal));
		SetTangent(nmat * mt::vec4(m_data->tangent));
	}

	inline void TransformUv(const int index, const mt::mat4& mat)
	{
		SetUV(index, (mat * mt::vec3(GetUv(index)[0], GetUv(index)[1], 0.0f)).xy());
	}
};

#endif  // __RAS_TEXVERT_H__
