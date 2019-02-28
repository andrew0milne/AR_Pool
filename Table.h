#include <vector>
#include <graphics/renderer_3d.h>

#include "GameObject.h"


#include <sony_sample_framework.h>
#include <sony_tracking.h>

struct Markers
{
	gef::Matrix44 transform_;
	gef::Matrix44 local_origin_offset;
	bool found_;
	float angle_;
	int id_;
};

#pragma once
class Table
{
public:
	Table();
	Table(gef::Platform& platform_, PrimitiveBuilder* primitive_builder_);
	~Table();

	void Init();

	void Update(float frame_time);
	void Render(gef::Renderer3D* renderer_3d_);
	void Lock();

	gef::Matrix44 Table::GetLocalTransform(gef::Matrix44 origin, gef::Matrix44 matrix);

	float GetAngle(gef::Vector4 A, gef::Vector4 C);
	gef::Vector4 GetMidPointTransform(bool local_space);
	gef::Matrix44 GetMidPointTransform(gef::Matrix44 marker_transform_0, gef::Matrix44 marker_transform_1, GameObject* wall_);
	std::vector<Markers> GetMarkerOrder();

	gef::Matrix44 SetUpPocket(gef::Matrix44 marker_transform);

	float GetMaxDistance();

	int NumberOfWalls();

	float GetDistance(gef::Vector4 pos_1_, gef::Vector4 pos_2_);

	gef::Matrix44 GetLocalOrigin();
	int GetLockedMarkerID();

	GameObject* walls_[6];
	GameObject* pockets_[6];
	float max_distance_;

	bool marker_found_;
	bool more_than_three_markers;

private:

	PrimitiveBuilder * primitive_builder_;

	Markers markers_[6];
	std::vector<int> marker_order_;
	std::vector<Markers> visible_markers_;

	gef::Vector4 midpoint_;

	gef::Matrix44 local_origin_;

	gef::Material wall_mat_;
	gef::Material pocket_mat_;

	bool locked_;
	int locked_marker_id_;
	int marker_count_;
};

